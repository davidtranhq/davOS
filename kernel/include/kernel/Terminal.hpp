#pragma once

#include <cstdint>
#include <kpp/optional.hpp>
#include <kpp/StringView.hpp>
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

	Framebuffer(uint32_t* pixelBitmapBuffer, std::size_t widthInPixels, std::size_t heightInPixels)
		: m_pixels {pixelBitmapBuffer},
		m_width {widthInPixels},
		m_height {heightInPixels}
	{}

	inline void setPixel(Cursor cursor, uint32_t value)
	{
        // TODO: remove this dependency on kernel_assert: this Terminal class is written
        // to be used not just in kernel space but also in user space.
		kernel_assert(cursor.y < height() && cursor.x < width(), "Cursor out of bounds");
		m_pixels[width() * cursor.y + cursor.x] = value;
	}

	inline std::size_t width() const noexcept { return m_width; }
	inline std::size_t height() const noexcept { return m_height; }
    inline const uint32_t* data() const noexcept { return m_pixels; }

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
            // TODO: remove this dependency on kernel_assert: this Terminal class is written
            // to be used not just in kernel space but also in user space.
			kernel_assert(index < m_size, "Index out of bounds");
			return m_data[(m_front + index) % bufferCapacity];
		}

		std::size_t size() const { return m_size; }

	private:
		kpp::Array<T, bufferCapacity> m_data;
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
    Terminal(const Framebuffer&, const VGAFont& font = DefaultFonts::terminalFont);

    /**
     * Write the given character to the end of the text buffer. Depending on the configured
     * scroll mode, the viewport may be updated.
     */
    void write(char);

    /**
     * Write the given string to the end of the text buffer. Depending on the configured
     * scroll mode, the viewport may be updated.
     */
    void write(kpp::StringView<char>);

    /**
     * Scroll the viewport up the specified number of lines (so that previous lines become
     * visible and subsequent lines are not). A negative paramter corresponds to a downward
     * scroll.
     */
    void scrollUp(int numberOfLines);

    /**
     * Clear the viewport, but do not clear the text buffer.
     */
    void clearViewport();

    /**
     * Set the scroll mode of the terminal.
     */
    void setScrollMode(ScrollMode scrollMode) { m_scrollMode = scrollMode; }

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
	const BitmapFont<16>& m_font {DefaultFonts::terminalFont};
	ScrollMode m_scrollMode = ScrollMode::automatic;
	Framebuffer::Cursor m_paintCursor;

    /**
     * The index of the first character in the text buffer that is visible in the viewport.
     * This will always be the index of a character at the start of a line, that is,
     * the index of a character after a newline character.
     */
	std::size_t m_firstVisibleCharacter = 0;
	RingBuffer<char, 0x10000> m_characterBuffer;
};

namespace KernelTerminal {
void initialize();
inline kpp::Optional<Terminal> instance {};
} // namespace KernelTerminal
