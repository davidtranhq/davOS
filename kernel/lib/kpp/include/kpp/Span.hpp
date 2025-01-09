#pragma once

#include <cstddef>

namespace kpp {
template <typename T>
class Span {
public:
	Span(const T* data, std::size_t size)
		: m_data {data},
		  m_size {size}
	{}

	inline T& operator[](std::size_t index) { return m_data[index]; }
	inline const T& operator[](std::size_t index) const { return m_data[index]; }

	inline T* data() { return m_data; }
	inline const T* data() const { return m_data; }

	[[ nodiscard ]] inline size_t size() const { return m_size; }
	inline T* begin() { return m_data; }
	inline T* end() { return m_data + m_size; }

private:
	const T* m_data;
	std::size_t m_size;
};
} // namespace kpp
