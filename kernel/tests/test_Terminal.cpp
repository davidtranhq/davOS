#include <fstream>
#include <string>
#include <gtest/gtest.h>
#include <kernel/Terminal.hpp>

void kernel_panic(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    exit(1);
}

std::string stringifyFramebuffer(const Framebuffer &framebuffer)
{
    std::string result;
    for (std::size_t i = 0; i < framebuffer.width() * framebuffer.height(); ++i) {
        if (i && i % framebuffer.width() == 0)
            result += '\n';
        result += (framebuffer.data()[i] == 0xffffffff ? '#' : '.');
    }
    return result;
}

std::string getExpectedString(const std::string &filename)
{
    auto file = std::ifstream {"expected/terminal/" + filename};
    auto result = std::string {};
    for (std::string line; std::getline(file, line);)
        result += line + '\n';
    if (result.back() == '\n')
        result.pop_back();
    return result;
}

TEST(Terminal, HelloWorld)
{
    dav::Array<uint32_t, 320 * 40> pixelBuffer {};
    Framebuffer framebuffer {pixelBuffer.data(), 320, 40};
    Terminal terminal {framebuffer};
    terminal.write("Hello, world!\nGoodbye, world!");
    EXPECT_EQ(stringifyFramebuffer(framebuffer), getExpectedString("hello.txt"));
}

TEST(Terminal, AutomaticallyScrollsDown)
{
    dav::Array<uint32_t, 320 * 40> pixelBuffer {};
    Framebuffer framebuffer {pixelBuffer.data(), 320, 40};
    Terminal terminal {framebuffer};
    terminal.write("Line 1\nLine 2\nLine 3");
    EXPECT_EQ(stringifyFramebuffer(framebuffer), getExpectedString("automatically-scrolls-down.txt"));
}

TEST(Terminal, ScrollsUp)
{
    dav::Array<uint32_t, 320 * 40> pixelBuffer {};
    Framebuffer framebuffer {pixelBuffer.data(), 320, 40};
    Terminal terminal {framebuffer};
    terminal.write("Line 1\nLine 2\nLine 3");
    terminal.setScrollMode(Terminal::ScrollMode::manual);
    terminal.scrollUp(1);
    EXPECT_EQ(stringifyFramebuffer(framebuffer), getExpectedString("scrolls-up.txt"));
}

TEST(Terminal, ManuallyScrollsDown)
{
    dav::Array<uint32_t, 320 * 40> pixelBuffer {};
    Framebuffer framebuffer {pixelBuffer.data(), 320, 40};
    Terminal terminal {framebuffer};
    terminal.setScrollMode(Terminal::ScrollMode::manual);
    terminal.write("Line 1\nLine 2\nLine 3\nLine 4\nLine 5");
    terminal.scrollUp(-3);
    EXPECT_EQ(stringifyFramebuffer(framebuffer), getExpectedString("manually-scrolls-down.txt"));
}

TEST(Terminal, LineWraps)
{
    dav::Array<uint32_t, 160 * 40> pixelBuffer {};
    Framebuffer framebuffer {pixelBuffer.data(), 160, 40};
    Terminal terminal {framebuffer};
    terminal.setScrollMode(Terminal::ScrollMode::manual);
    terminal.write("This is a long line that should wrap around to the next line.");
    std::cout << stringifyFramebuffer(framebuffer) << std::endl;
}
