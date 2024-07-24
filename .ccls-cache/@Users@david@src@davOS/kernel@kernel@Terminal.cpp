#include <dav/math.hpp>
#include <kernel/Terminal.hpp>

Terminal::Terminal(const Framebuffer& framebuffer)
    : m_framebuffer(framebuffer)
{}

void Terminal::write(dav::StringView<char> string)
{
    for (auto character : string) {
        m_characterBuffer.push(character);
        if (paintCharacter(character) == PaintCharacterResult::cursorWrappedToNewLine && character != '\n')
            m_characterBuffer.push('\n');
    }
}

Terminal::PaintCharacterResult Terminal::paintCharacter(char character)
{
    auto wrapCursorToNewLine = [&] {
        m_paintCursor.x = 0;
        const auto lineHeight = m_font.characterTotalHeight();
        if (m_paintCursor.y + lineHeight > m_framebuffer.height())
            scrollUp(-1);
        else
            m_paintCursor.y += lineHeight;
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
            if (characterBitmap[line] & (1 << bit)) {
                constexpr auto whiteRGBA = 0xffffffff;
                m_framebuffer.setPixel({characterTopLeft.x + bit, characterTopLeft.y + line}, whiteRGBA);
            }
        }
    }
    m_paintCursor.x += m_font.characterTotalWidth();
    if (m_paintCursor.x > m_framebuffer.width()) {
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
			// Find the previous newline character
            while (firstCharacterOfNextLine && m_characterBuffer[firstCharacterOfNextLine] != '\n')
                --firstCharacterOfNextLine;
			// Don't scroll up if there are no lines above
			if (!firstCharacterOfNextLine)
				break;
			// Skip the newline character
			--firstCharacterOfNextLine;
        }
		m_firstVisibleCharacter = firstCharacterOfNextLine;
    }
	paintVisibleBuffer();
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
