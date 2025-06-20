#include <algorithm>

#include <kpp/cmath.hpp>
#include <kpp/StringView.hpp>
#include <kernel/Terminal.hpp>

Terminal::Terminal(const Framebuffer& framebuffer, const VGAFont& font)
    : m_framebuffer(framebuffer),
      m_font(font)
{
    m_lineBuffer.setLineSize(m_framebuffer.width() / m_font.characterTotalWidth());
    m_lineBuffer.setNumVisibleLines(m_framebuffer.height() / m_font.characterTotalHeight());
}

void Terminal::LineBuffer::setLineSize(size_t lineSize)
{
    m_lineSize = std::min(lineSize, s_maxColumns);
}

void Terminal::LineBuffer::setNumVisibleLines(size_t numVisibleLines)
{
    m_visibleLines = std::min(numVisibleLines, s_maxLines);
}

void Terminal::LineBuffer::scrollDown(int lines)
{
    if (lines >= 0)
        m_firstVisibleLine = std::min(m_size - 1, m_firstVisibleLine + lines);
    else
        m_firstVisibleLine = (size_t)kpp::abs(lines) > m_firstVisibleLine ? 0 : m_firstVisibleLine - (size_t)kpp::abs(lines);
}

Terminal::LineBuffer::WriteResult Terminal::LineBuffer::write(char character, ScrollMode scrollMode) {
    const bool lineWrap = m_currentColumn >= m_lineSize || character == '\n';
    const auto oldFirstVisibleLine = m_firstVisibleLine;
    if (lineWrap) {
        if (m_size >= s_maxLines) {
            // Buffer is full, overwrite the oldest line
            m_front = (m_front + 1) % s_maxLines;
            if (m_firstVisibleLine == m_front) {
                // If the first visual line is being overwritten, the terminal will
                // scroll down to erase it from the terminal.
                m_firstVisibleLine = (m_firstVisibleLine + 1) % s_maxLines;
            }
        } else {
            m_size++;
        }
        if (scrollMode == ScrollMode::automatic) {
            // If the line wrapped, we need to scroll down until the last line is visible.
            m_firstVisibleLine = std::max(m_firstVisibleLine, m_size > m_visibleLines ? m_size - m_visibleLines : 0);
        }
        m_currentColumn = 0;
    }
    // Write the character to the current line
    kpp::Array<char, s_maxColumns>& currentLine = m_lineBuffer[(m_front + m_size - 1) % s_maxLines];
    if (character != '\n') {
        currentLine[m_currentColumn++] = character;
    }
    return WriteResult {
        .lineWrapped = lineWrap,
        .needsViewportRedraw = m_firstVisibleLine != oldFirstVisibleLine
    };
}

size_t Terminal::LineBuffer::firstVisibleLineIndex() const
{
    return m_firstVisibleLine < m_front ? m_firstVisibleLine + s_maxLines : m_firstVisibleLine;
}

size_t Terminal::LineBuffer::lastVisibleLineIndex() const
{
    return (firstVisibleLineIndex() + m_visibleLines - 1);
}

const Terminal::LineBuffer::Line& Terminal::LineBuffer::getLine(size_t index) const
{
    return m_lineBuffer[(m_front + index) % s_maxLines];
}

void Terminal::write(char character)
{
    LineBuffer::WriteResult result = m_lineBuffer.write(character);
    if (result.needsViewportRedraw) {
        paintVisibleBuffer();
    } else {
        if (result.lineWrapped && character != '\n')
            paintCharacter('\n');
        paintCharacter(character);
    }

}

void Terminal::write(kpp::StringView<char> string)
{
    for (auto character : string)
        write(character);
}

void Terminal::scrollDown(int lines)
{
    m_lineBuffer.scrollDown(lines);
    paintVisibleBuffer();
};

void Terminal::paintCharacter(char character)
{
    if (character == '\n') {
        for (std::size_t x = m_paintCursor.x; x < m_framebuffer.width(); ++x) {
            constexpr Framebuffer::RGBAPixel blackRGBA = 0;
            for (std::size_t y = m_paintCursor.y; y < m_paintCursor.y + m_font.characterTotalHeight(); ++y) {
                m_framebuffer.setPixel({x, y}, blackRGBA);
            }
        }
        m_paintCursor.x = 0;
        if (m_paintCursor.y + m_font.characterTotalHeight() < m_framebuffer.height())
            m_paintCursor.y += m_font.characterTotalHeight();
        return;
    }

    auto characterTopLeft = m_paintCursor;
    characterTopLeft.x += m_font.characterLeftPadding;
    characterTopLeft.y += m_font.characterTopPadding;
    const auto characterBitmap = m_font.characterBitmap(character);
    const auto characterBitWidth = m_font.characterWidth;
    for (std::size_t line = 0; line < characterBitmap.size(); ++line) {
        for (std::size_t bit = 0; bit < characterBitWidth; ++bit) {
            if (characterBitmap[line] & (1 << (characterBitWidth - bit - 1))) {
       constexpr Framebuffer::RGBAPixel whiteRGBA = 0xffffffff;
                m_framebuffer.setPixel({characterTopLeft.x + bit, characterTopLeft.y + line}, whiteRGBA);
            } else {
                constexpr Framebuffer::RGBAPixel blackRGBA = 0;
                m_framebuffer.setPixel({characterTopLeft.x + bit, characterTopLeft.y + line}, blackRGBA);
            }
        }
    }
    m_paintCursor.x += m_font.characterWidth + m_font.characterLeftPadding + m_font.characterRightPadding;
}

void Terminal::paintVisibleBuffer()
{
	m_paintCursor = {0, 0};
    for (size_t line = m_lineBuffer.firstVisibleLineIndex(); line <= m_lineBuffer.lastVisibleLineIndex(); ++line) {
        const Terminal::LineBuffer::Line& currentLine = m_lineBuffer.getLine(line);
        for (char character : currentLine) {
            if (character == '\0') {
                break; // Skip empty characters
            }
            paintCharacter(character);
        }
        paintCharacter('\n');
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
