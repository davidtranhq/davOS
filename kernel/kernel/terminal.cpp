#include <dav/math.hpp>
#include <kernel/Terminal.hpp>

Terminal::Terminal(const Framebuffer& framebuffer, const VGAFont& font)
    : m_framebuffer(framebuffer),
      m_font(font)
{}

void Terminal::write(char character)
{
    m_characterBuffer.push(character);
    if (paintCharacter(character) == PaintCharacterResult::cursorWrappedToNewLine && character != '\n')
        m_characterBuffer.push('\n');
}

void Terminal::write(dav::StringView<char> string)
{
    for (auto character : string)
        write(character);
}

Terminal::PaintCharacterResult Terminal::paintCharacter(char character)
{
    auto wrapCursorToNewLine = [&] {
        m_paintCursor.x = 0;
        const auto lineHeight = m_font.characterTotalHeight();
        if (m_paintCursor.y + 2 * lineHeight > m_framebuffer.height()) {
            if (m_scrollMode == ScrollMode::automatic)
                scrollUp(-1);
        } else {
            m_paintCursor.y += lineHeight;
        }
    };

    if (character == '\n') {
        wrapCursorToNewLine();
        return PaintCharacterResult::cursorWrappedToNewLine;
    }

    auto characterTopLeft = m_paintCursor;
    characterTopLeft.x += m_font.characterLeftPadding;
    characterTopLeft.y += m_font.characterTopPadding;
    const auto characterBitmap = m_font.characterBitmap(character);
    const auto characterBitWidth = m_font.characterWidth;
    for (std::size_t line = 0; line < characterBitmap.size(); ++line) {
        for (std::size_t bit = 0; bit < characterBitWidth; ++bit) {
            if (characterBitmap[line] & (1 << (characterBitWidth - bit - 1))) {
                constexpr auto whiteRGBA = 0xffffffff;
                m_framebuffer.setPixel({characterTopLeft.x + bit, characterTopLeft.y + line}, whiteRGBA);
            } else {
                constexpr auto blackRGBA = 0;
                m_framebuffer.setPixel({characterTopLeft.x + bit, characterTopLeft.y + line}, blackRGBA);
            }
        }
    }
    m_paintCursor.x += m_font.characterTotalWidth();
    if (m_paintCursor.x + m_font.characterTotalWidth() >= m_framebuffer.width()) {
        wrapCursorToNewLine();
        return PaintCharacterResult::cursorWrappedToNewLine;
    }
    return PaintCharacterResult::normal;
}

void Terminal::scrollUp(int numberOfLines)
{
    if (!numberOfLines)
        return;
    const bool scrollDown = numberOfLines < 0;
    numberOfLines = dav::abs(numberOfLines);
    for (int linesScrolled = 0; linesScrolled < numberOfLines; ++linesScrolled) {
		auto firstCharacterOfNextLine = m_firstVisibleCharacter;
        if (scrollDown) {
			// Find the next newline character
            while (firstCharacterOfNextLine < m_characterBuffer.size() && m_characterBuffer[firstCharacterOfNextLine] != '\n') 
				++firstCharacterOfNextLine;
			// Skip the newline character
			++firstCharacterOfNextLine;
			// Don't scroll down if there are no more lines
			if (firstCharacterOfNextLine >= m_characterBuffer.size())
				break;
        } else {
            // If we are at the start of the buffer, there are no more lines to scroll up to
            if (!firstCharacterOfNextLine) {
                m_firstVisibleCharacter = 0;
                break;
            }
            // Move to the newline character preceding this line
            firstCharacterOfNextLine -= 1;
            // Skip over the newline character preceding this line and
            // find the newline character of the previous previous line
            while (firstCharacterOfNextLine && m_characterBuffer[--firstCharacterOfNextLine] != '\n')
                ;
			// Move one character past the newline character of the previous line, if there is a previous line
            if (firstCharacterOfNextLine)
			    ++firstCharacterOfNextLine;
        }
		m_firstVisibleCharacter = firstCharacterOfNextLine;
    }
    clearViewport();
	paintVisibleBuffer();
}

void Terminal::clearViewport()
{
    for (std::size_t y = 0; y < m_framebuffer.height(); ++y) {
        for (std::size_t x = 0; x < m_framebuffer.width(); ++x) {
            m_framebuffer.setPixel({x, y}, 0);
        }
    }
}

void Terminal::paintVisibleBuffer()
{
	m_paintCursor = {0, 0};
	size_t linesPainted = 0;
	const size_t maxVisibleLines = m_framebuffer.height() / m_font.characterTotalHeight();
	auto characterIndex = m_firstVisibleCharacter;
	while (characterIndex < m_characterBuffer.size() && linesPainted < maxVisibleLines) {
		linesPainted += paintCharacter(m_characterBuffer[characterIndex]) == PaintCharacterResult::cursorWrappedToNewLine;
		characterIndex += 1;
	}
}

void KernelTerminal::initialize()
{
    instance.emplace(Terminal {
        Framebuffer {
            reinterpret_cast<uint32_t *>(limine::framebuffers_info->framebuffers[0]->address),
            limine::framebuffers_info->framebuffers[0]->width,
            limine::framebuffers_info->framebuffers[0]->height
        },
        DefaultFonts::terminalFont
    });
}
