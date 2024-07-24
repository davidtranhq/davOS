#pragma once

#include <cstdint>
#include <dav/StringView.hpp>
#include <kernel/BitmapFont.hpp>
#include <kernel/kernel.h>
#include <kernel/limine.h>
#include <kernel/limine_features.h>

class Framebuffer {
public:
    struct Cursor {
		std::size_t x = 0;
		std::size_t y = 0;
    };

	Framebuffer(uint32_t* pixelBuffer, std::size_t width, std::size_t height)
		: m_pixels {pixelBuffer},
		m_width {width},
		m_height {height}
	{}

	inline void setPixel(Cursor cursor, uint32_t value)
	{
		kernel_assert(cursor.y < height() && cursor.x < width(), "Cursor out of bounds");
		m_pixels[width() * cursor.y + cursor.x] = value;
	}

	inline std::size_t width() const { return m_width; }
	inline std::size_t height() const { return m_height; }

private:
	uint32_t* m_pixels;
	std::size_t m_width;
	std::size_t m_height;
};

class Terminal
{
	template<typename T, std::size_t bufferCapacity>
	class RingBuffer {
	public:
		void push(char character)
		{
			if (m_size == bufferCapacity) {
				m_front = (m_front + 1) % bufferCapacity;
				--m_size;
			}
			m_data[(m_front + m_size) % bufferCapacity] = character;
			++m_size;
		}

		char operator[](std::size_t index) const
		{
			kernel_assert(index < m_size, "Index out of bounds");
			return m_data[(m_front + index) % bufferCapacity];
		}

		std::size_t size() const { return m_size; }

	private:
		dav::Array<T, bufferCapacity> m_data;
		std::size_t m_front = 0;
		std::size_t m_size = 0;
	};

public:
    /**
     * Dictates the behaviour of the viewport when text is written to the buffer.
     * Automatic: Scroll the terminal such that the last line is always visible.
     * Manual: Do not scroll the terminal automatically.
     */
    enum class ScrollMode {
        automatic,
        manual
    };

public:
    Terminal(const Framebuffer&);

    /**
     * Write the given string to the end of the text buffer. Depending on the configured
     * scroll mode, the viewport may be updated.
     */
    void write(dav::StringView<char>);

    /**
     * Scroll the viewport up the specified number of lines (so that previous lines become
     * visible and subsequent lines are not). A negative paramter corresponds to a downward
     * scroll.
     */
    void scrollUp(int numberOfLines);

private:
    /**
     * Paint a character at the curent paint cursor position, automatically advancing the cursor,
     * moving down a line if necessary.
     */
    enum class PaintCharacterResult {
        normal,
        cursorWrappedToNewLine
    };
    PaintCharacterResult paintCharacter(char);

	/**
	 * Paint the portion of the text buffer visible in the viewport, that is, the text buffer
	 * starting from the first visible character, to the last character that fits in the
	 * viewport.
	 */
	void paintVisibleBuffer();

private:

    Framebuffer m_framebuffer;
	BitmapFont<16> m_font {DefaultFonts::vgaFont};
	ScrollMode m_scrollMode = ScrollMode::automatic;
	Framebuffer::Cursor m_paintCursor;
	std::size_t m_firstVisibleCharacter = 0;
	std::size_t m_visibleLines = 0;
	RingBuffer<char, 0x10000> m_characterBuffer;
};

static Terminal globalTerminal {Framebuffer {
	reinterpret_cast<uint32_t *>(limine::framebuffers_info->framebuffers[0]->address),
	limine::framebuffers_info->framebuffers[0]->width,
	limine::framebuffers_info->framebuffers[0]->height
}};
