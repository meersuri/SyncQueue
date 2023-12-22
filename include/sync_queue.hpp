#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>

using namespace std::literals;

template <typename T>
class sync_queue {
    size_t m_max_size;
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    public:
        sync_queue<T>(size_t max_size=0): m_max_size(max_size) {};
        void push(T item);
        T pop();
};

template <typename T>
void sync_queue<T>::push(T item) {
    if (m_max_size > 0)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv.wait(lock, [this]() { return m_queue.size() < m_max_size; });
    }
    {
        std::lock_guard lock(m_mutex);
        m_queue.push(item);
    }
    m_cv.notify_one();
}

template <typename T>
T sync_queue<T>::pop() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.wait(lock, [this](){ return m_queue.size() > 0; }); 
    T item = m_queue.front();
    m_queue.pop();
    m_cv.notify_one();
    return item;
}
