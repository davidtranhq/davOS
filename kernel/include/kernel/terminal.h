#ifndef DAVOS_KERNEL_TERMINAL_H_INCLUDED
#define DAVOS_KERNEL_TERMINAL_H_INCLUDED

#include <stddef.h>

/**
 * @brief Initialize the terminal.
 */
void terminal_init(void);

/**
 * @brief Write a string to the end of the terminal. 
 * 
 * @param str String to write 
 * @param size size of the string
 */
void terminal_write(const char *str, size_t size);

template<typename Allocator>
class Terminal {
public:
    enum class ScrollMode {
        automatic,
        manual
    };

public:
    Terminal(limine_framebuffer*);

    void scrollUp(int numberOfLines); 
    void writeAtCursor(dav::StringView);
private:
    limine_framebuffer* m_frameBuffer = nullptr;
    dav::Vector<char, Allocator> m_textBuffer;
    size_t m_cursor = 0;
};

#endif
