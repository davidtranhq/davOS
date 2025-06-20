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

    using RGBAPixel = uint32_t;

	Framebuffer(uint32_t* pixelBitmapBuffer, std::size_t widthInPixels, std::size_t heightInPixels)
		: m_pixels {pixelBitmapBuffer},
		m_width {widthInPixels},
		m_height {heightInPixels}
	{}

	inline void setPixel(Cursor cursor, RGBAPixel value)
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
	RGBAPixel* m_pixels;
	std::size_t m_width;
	std::size_t m_height;
};

class Terminal
{
public:
    enum class ScrollMode {
        automatic, // Automatically scrolls the viewport when the buffer overflows
        manual     // Does not scroll the viewport, just adds new lines to the buffer
    };

private:
    /**
     * A circular buffer that stores lines of text. Each line is a fixed-size array of characters.
     * The buffer wraps around when it reaches the end, overwriting the oldest lines.
     */
    class LineBuffer {
    public:
        static constexpr size_t s_maxColumns = 200;
        static constexpr size_t s_maxLines = 256;
        using Line = kpp::Array<char, s_maxColumns>;
        /**
         * Set the maximum number of characters per line (terminal width).
         */
        void setLineSize(size_t lineSize);

        /**
         * Set the number of lines visible in the viewport (terminal height).
         */
        void setNumVisibleLines(size_t numVisibleLines);

        /**
         * Scroll the viewport down the specified number of lines (so that previous lines are
         * hidden and subsequent lines are visible). A negative paramter corresponds to a downward
         * scroll.
         */
        void scrollDown(int lines);

        /**
         * Write a character to the buffer. If the line is full, it will wrap to the next line.
         * If the buffer is full, it will overwrite the oldest line.
         */
        struct WriteResult {
            bool lineWrapped = false;
            bool needsViewportRedraw = false;
        };
        WriteResult write(char character, ScrollMode scrollMode = ScrollMode::automatic);

        /**
         * Returns the position of the first line expected to be visible in the viewport.
         * Note that this is the position in the circular buffer, not the raw index
         * into the underlying array.
         */
        size_t firstVisibleLineIndex() const;

        /**
         * Returns the position of the last line expected to be visible in the viewport.
         * Note that this is the position in the circular buffer, not the raw index
         * into the underlying array.
         */
        size_t lastVisibleLineIndex() const;

        const Line& getLine(size_t index) const;

    private:
        size_t m_lineSize = s_maxColumns;
        size_t m_visibleLines = 24;
        
        kpp::Array<Line, s_maxLines> m_lineBuffer;
        size_t m_front = 0;
        size_t m_size = 1;
        
        // The index of the first line that is currently visible in the viewport.
        // Note that this changes not only when the terminal scrolls, but
        // also it may change when our circular buffer runs out of room and 
        // needs to overwrite the first line.
        //
        // Note that this is the raw index into the circular buffer, not the
        // position of the line in the queue, similar to m_front and m_size
        // but different from what is returned from firstVisibleLine() and lastVisibleLine().
        size_t m_firstVisibleLine = 0;

        // The first empty column in the last line of the buffer:
        // the location where the next character will be writte nto.
        size_t m_currentColumn = 0;
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
     * Scroll the viewport down the specified number of lines (so that previous lines are
     * hidden and subsequent lines are visible). A negative paramter corresponds to a downward
     * scroll.
     */
    void scrollDown(int numberOfLines);

    /**
     * Set the scroll mode of the terminal.
     */
    void setScrollMode(ScrollMode scrollMode) { m_scrollMode = scrollMode; }

    /**
     * Clear the viewport, but do not clear the text buffer.
     */
    void clearViewport();

private:
    /**
     * Paint a character at the curent paint cursor position, automatically advancing the cursor,
     * moving down a line if necessary.
     */
    void paintCharacter(char);

	/**
	 * Paint the portion of the text buffer visible in the viewport, that is, the text buffer
	 * starting from the first visible character, to the last character that fits in the
	 * viewport.
	 */
	void paintVisibleBuffer();

private:
    size_t m_columnsPerLine = 100;

    Framebuffer m_framebuffer;
	const BitmapFont<16>& m_font {DefaultFonts::terminalFont};
	ScrollMode m_scrollMode = ScrollMode::automatic;
	Framebuffer::Cursor m_paintCursor;
    LineBuffer m_lineBuffer;

    /**
     * write character to terminal
     *      character does not create new line: just print it
     *      character does create new line:
     *          if the buffer is full, pop the oldest line, and if the "first line"
     *          points to this oldest line, update "first line" to the next line
     *          if automatic scrolling:
     *              scroll terminal so that there is an empty
     *              line at the botom
     *                  update "first line" to last line - terminalLines (clamp to 0)
     *                  if changed, repaint the whole terminal based on the buffer
     *          if manual scrolling:
     *              do not scroll, just add the character to the buffer
     *
     *          
     */
};

namespace KernelTerminal {
void initialize();
inline kpp::Optional<Terminal> instance {};
} // namespace KernelTerminal
