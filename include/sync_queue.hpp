#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <optional>

using namespace std::literals;

template <typename T>
class sync_queue {
    size_t m_max_size;
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    public:
        sync_queue<T>(size_t max_size=0): m_max_size(max_size) {};
        bool push(const T& item, std::chrono::milliseconds timeout=0ms);
        std::optional<T> pop(std::chrono::milliseconds timeout=0ms);
};

template <typename T>
bool sync_queue<T>::push(const T& item, std::chrono::milliseconds timeout) {
    if (m_max_size > 0)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (timeout > 0ms) {
            auto now = std::chrono::steady_clock::now();
            if (!m_cv.wait_until(lock, now + timeout, [this]() { return m_queue.size() < m_max_size; })) {
                return false;
            }
        }
        else {
            m_cv.wait(lock, [this]() { return m_queue.size() < m_max_size; });
        }
    }
    {
        std::lock_guard lock(m_mutex);
        m_queue.push(item);
    }
    m_cv.notify_one();
    return true;
}

template <typename T>
std::optional<T> sync_queue<T>::pop(std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (timeout > 0ms) {
        auto now = std::chrono::steady_clock::now();
        if (!m_cv.wait_until(lock, now + timeout, [this](){ return m_queue.size() > 0; })) {
            return std::optional<T>{};
        }
    }
    else {
        m_cv.wait(lock, [this](){ return m_queue.size() > 0; });
    }
    T item = std::move(m_queue.front());
    m_queue.pop();
    m_cv.notify_one();
    return std::optional<T>{item};
}
