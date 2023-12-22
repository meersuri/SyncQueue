#include <iostream>
#include <thread>
#include <gtest/gtest.h>
#include "sync_queue.hpp"

TEST(SimpleTests, SingleElement) {
    sync_queue<int> queue;
    queue.push(5);
    int item = queue.pop();
    EXPECT_EQ(item, 5);
}

TEST(SimpleTests, FIFO1) {
    sync_queue<int> queue;
    queue.push(5);
    queue.push(7);
    int item = queue.pop();
    EXPECT_EQ(item, 5);
    item = queue.pop();
    EXPECT_EQ(item, 7);
}

TEST(SimpleTests, FIFO2) {
    sync_queue<int> queue;
    queue.push(5);
    queue.push(7);
    int item = queue.pop();
    EXPECT_EQ(item, 5);
    queue.push(8);
    item = queue.pop();
    EXPECT_EQ(item, 7);
    item = queue.pop();
    EXPECT_EQ(item, 8);
}

TEST(Multithreaded, OneProducerThread) {
    sync_queue<int> queue;
    std::thread p([&](){ queue.push(5); });
    p.join();
    int item = queue.pop();
    EXPECT_EQ(item, 5);
}

TEST(Multithreaded, TwoProducerThreadsInSequence) {
    sync_queue<int> queue;
    std::thread p1([&](){ queue.push(5); });
    p1.join();
    std::thread p2([&](){ queue.push(7); });
    p2.join();
    int item = queue.pop();
    EXPECT_EQ(item, 5);
    item = queue.pop();
    EXPECT_EQ(item, 7);
}

TEST(Multithreaded, OneConsumerThread) {
    sync_queue<int> queue;
    queue.push(5);
    std::thread p([&](){
            int item = queue.pop();
            EXPECT_EQ(item, 5);
            });
    p.join();
}

TEST(Multithreaded, TwoConsumerThreadsInSequence) {
    sync_queue<int> queue;
    queue.push(5);
    queue.push(7);
    std::thread p1([&](){
            int item = queue.pop();
            EXPECT_EQ(item, 5);
            });
    p1.join();
    std::thread p2([&](){
            int item = queue.pop();
            EXPECT_EQ(item, 7);
            });
    p2.join();
}

