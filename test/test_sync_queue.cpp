#include <iostream>
#include <thread>
#include <gtest/gtest.h>
#include "sync_queue.hpp"

TEST(SimpleTests, SingleElement) {
    sync_queue<int> queue;
    EXPECT_TRUE(queue.push(5));
    int item = queue.pop().value();
    EXPECT_EQ(item, 5);
}

TEST(SimpleTests, FIFO1) {
    sync_queue<int> queue;
    EXPECT_TRUE(queue.push(5));
    EXPECT_TRUE(queue.push(7));
    int item = queue.pop().value();
    EXPECT_EQ(item, 5);
    item = queue.pop().value();
    EXPECT_EQ(item, 7);
}

TEST(SimpleTests, FIFO2) {
    sync_queue<int> queue;
    EXPECT_TRUE(queue.push(5));
    EXPECT_TRUE(queue.push(7));
    int item = queue.pop().value();
    EXPECT_EQ(item, 5);
    EXPECT_TRUE(queue.push(8));
    item = queue.pop().value();
    EXPECT_EQ(item, 7);
    item = queue.pop().value();
    EXPECT_EQ(item, 8);
}

TEST(SimpleTests, BlockingPushPop) {
    sync_queue<int> queue;
    EXPECT_TRUE(queue.push(5, 1ms));
    EXPECT_TRUE(queue.push(7, 2ms));
    EXPECT_EQ(queue.pop(1ms).value(), 5);
    EXPECT_TRUE(queue.push(8, 3ms));
    EXPECT_EQ(queue.pop(2ms).value(), 7);
    EXPECT_EQ(queue.pop(3ms).value(), 8);
}

TEST(Bounded, BlockingPushTimeout) {
    sync_queue<int> queue(1);
    EXPECT_TRUE(queue.push(5));
    EXPECT_FALSE(queue.push(7, 1ms));
    EXPECT_FALSE(queue.push(7, 5ms));
    EXPECT_FALSE(queue.push(7, 20ms));
    EXPECT_FALSE(queue.push(7, 100ms));
}

TEST(Bounded, BlockingPopTimeout) {
    sync_queue<int> queue;
    EXPECT_FALSE(queue.pop(1ms).has_value());
    EXPECT_FALSE(queue.pop(5ms).has_value());
    EXPECT_FALSE(queue.pop(20ms).has_value());
    EXPECT_FALSE(queue.pop(100ms).has_value());
}

TEST(Multithreaded, OneProducerThread) {
    sync_queue<int> queue;
    std::thread p([&](){ queue.push(5); });
    p.join();
    int item = queue.pop().value();
    EXPECT_EQ(item, 5);
}

TEST(Multithreaded, TwoProducerThreadsInSequence) {
    sync_queue<int> queue;
    std::thread p1([&](){ queue.push(5); });
    p1.join();
    std::thread p2([&](){ queue.push(7); });
    p2.join();
    int item = queue.pop().value();
    EXPECT_EQ(item, 5);
    item = queue.pop().value();
    EXPECT_EQ(item, 7);
}

TEST(Multithreaded, OneConsumerThread) {
    sync_queue<int> queue;
    queue.push(5);
    std::thread p([&](){
            int item = queue.pop().value();
            EXPECT_EQ(item, 5);
            });
    p.join();
}

TEST(Multithreaded, TwoConsumerThreadsInSequence) {
    sync_queue<int> queue;
    queue.push(5);
    queue.push(7);
    std::thread p1([&](){
            int item = queue.pop().value();
            EXPECT_EQ(item, 5);
            });
    p1.join();
    std::thread p2([&](){
            int item = queue.pop().value();
            EXPECT_EQ(item, 7);
            });
    p2.join();
}

TEST(Multithreaded, BlockingPushWithConsumerThread) {
    sync_queue<int> queue(1);
    EXPECT_TRUE(queue.push(5));
    EXPECT_FALSE(queue.push(7, 1ms));
    std::thread p([&](){
            std::this_thread::sleep_for(5ms);
            int item = queue.pop().value();
            EXPECT_EQ(item, 5);
            });
    EXPECT_TRUE(queue.push(6));
    EXPECT_EQ(queue.pop().value(), 6);
    p.join();
}

TEST(Multithreaded, BlockingPushConsumerTooSlow) {
    sync_queue<int> queue(1);
    EXPECT_TRUE(queue.push(5));
    EXPECT_FALSE(queue.push(7, 1ms));
    std::thread p([&](){
            std::this_thread::sleep_for(5ms);
            EXPECT_FALSE(queue.pop(1ms).has_value());
            });
    EXPECT_FALSE(queue.push(6, 1ms));
    EXPECT_EQ(queue.pop().value(), 5);
    p.join();
}

TEST(Multithreaded, BlockingPopWithProducerThread) {
    sync_queue<int> queue;
    EXPECT_FALSE(queue.pop(1ms).has_value());
    std::thread p([&](){
            std::this_thread::sleep_for(5ms);
            EXPECT_TRUE(queue.push(5));
            });
    EXPECT_EQ(queue.pop().value(), 5);
    p.join();
}

TEST(Multithreaded, BlockingPopProducerTooSlow) {
    sync_queue<int> queue;
    EXPECT_FALSE(queue.pop(1ms).has_value());
    std::thread p([&](){
            std::this_thread::sleep_for(5ms);
            EXPECT_TRUE(queue.push(5));
            });
    EXPECT_FALSE(queue.pop(1ms).has_value());
    EXPECT_EQ(queue.pop(5ms).value(), 5);
    p.join();
}
