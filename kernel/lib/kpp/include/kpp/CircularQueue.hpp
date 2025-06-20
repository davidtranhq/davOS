#pragma once

#include <cstdint>

namespace kpp {
template<typename T, std::size_t capacity>
class CircularQueue {
public:
    CircularQueue() : m_front(0), m_back(0), m_size(0) {}

    bool isEmpty() const { return m_size == 0; }
    bool isFull() const { return m_size == capacity; }
    std::size_t size() const { return m_size; }

    void enqueue(const T& item) {
        if (isFull()) {
            // Handle full queue, e.g., throw an exception or overwrite the oldest item
            // For simplicity, we will just return here.
            return;
        }
        m_data[m_back] = item;
        m_back = (m_back + 1) % capacity;
        ++m_size;
    }

    T dequeue() {
        if (isEmpty()) {
            // Handle empty queue, e.g., throw an exception or return a default value
            // For simplicity, we will just return a default-constructed T.
            return T();
        }
        T item = m_data[m_front];
        m_front = (m_front + 1) % capacity;
        --m_size;
        return item;
    }
private:
    T m_data[capacity];
    std::size_t m_front;
    std::size_t m_back;
    std::size_t m_size;
};
} // namespace kpp
