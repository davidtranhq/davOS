#pragma once

#include <cstddef>

namespace dav {
template <typename T>
class StringView {
public:
	constexpr StringView(const char* data, std::size_t size)
		: m_data {data},
		  m_size {size}
	{}

	constexpr StringView(const char* data)
		: m_data {data},
		  m_size {0}
	{
		while (m_data[m_size])
			++m_size;
	}

	constexpr char operator[](std::size_t index) const { return m_data[index]; }
	constexpr const char* data() const { return m_data; }
	constexpr std::size_t size() const { return m_size; }
	constexpr const char* begin() const { return m_data; }
	constexpr const char* end() const { return m_data + m_size; }
private:
	const T* m_data;
	std::size_t m_size;
};
} // namespace dav
