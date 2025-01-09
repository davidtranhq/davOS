#pragma once

#include <cstddef>

namespace kpp {
template <typename T>
class StringView {
public:
    /**
     * @brief Construct a new StringView from the first `size`
     * characters of the given data.
     */
	constexpr StringView(const char* data, std::size_t size)
		: m_data {data},
		  m_size {size}
	{}

    /**
     * @brief Construct a new StringView from a null-terminated string.
     * 
     * The new StringView will have a size equal to the length of the string,
     * not including the null-terminator.
     */
	constexpr StringView(const char* data)
		: m_data {data},
		  m_size {0}
	{
		while (m_data[m_size])
			++m_size;
	}

	constexpr char operator[](std::size_t index) const { return m_data[index]; }
	[[ nodiscard ]] constexpr const char* data() const { return m_data; }
	[[ nodiscard ]] constexpr std::size_t size() const { return m_size; }
	[[ nodiscard ]] constexpr const char* begin() const { return m_data; }
	[[ nodiscard ]] constexpr const char* end() const { return m_data + m_size; }
private:
	const T* m_data;
	std::size_t m_size;
};
} // namespace kpp
