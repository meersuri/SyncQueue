#include <iostream>
#include <gtest/gtest.h>
#include "sync_queue.hpp"

TEST(SimpleTests, SingleElement) {
    sync_queue<int> queue;
    queue.push(5);
    int item = queue.pop();
    EXPECT_EQ(item, 5);
}

TEST(SimpleTests, FrontOfQueue) {
    sync_queue<int> queue;
    queue.push(5);
    queue.push(7);
    int item = queue.pop();
    EXPECT_EQ(item, 5);
    item = queue.pop();
    EXPECT_EQ(item, 7);
}


