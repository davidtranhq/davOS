#ifndef KERNELCPP_DAV_RANDOM_ACCESS_QUEUE_HPP
#define KERNELCPP_DAV_RANDOM_ACCESS_QUEUE_HPP

namespace dav {
template<typename T>
class RandomAccessQueue {
public:
    template<typename U>
    void push(U&& value)
    {
        if (m_size == m_queue.size())
            resize(m_size * 2);
        auto const capacity = m_queue.size();
        m_queue[(m_front + m_size) % capacity] = value;
        m_size += 1;
    }

    void pop()
    {
        auto const capacity = m_queue.size();
        m_front = (m_front + 1) % capacity;
        m_size -= 1;
    }

    inline T front() const { return m_queue[m_front]; }
    inline T& operator[](size_t index) { return m_queue[(m_front + index) % m_front.size()]; }
    inline size_t size() const { return m_size; }
    inline bool empty() const { return size() == 0LL; }

private:
    dav::Vector<T> m_queue;
    size_t m_front;
    size_t m_size;
};
} // namespace dav

#endif
