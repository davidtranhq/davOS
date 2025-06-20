#pragma once

#include <kpp/CircularQueue.hpp>

#include <cstdint>

class KeyboardBuffer {
public:
    enum class Key {
        None = 0,

        // Printable ASCII characters
        a = 'a', b = 'b', c = 'c', d = 'd', e = 'e', f = 'f', g = 'g',
        h = 'h', i = 'i', j = 'j', k = 'k', l = 'l', m = 'm', n = 'n',
        o = 'o', p = 'p', q = 'q', r = 'r', s = 's', t = 't', u = 'u',
        v = 'v', w = 'w', x = 'x', y = 'y', z = 'z',

        A = 'A', B = 'B', C = 'C', D = 'D', E = 'E', F = 'F', G = 'G',
        H = 'H', I = 'I', J = 'J', K = 'K', L = 'L', M = 'M', N = 'N',
        O = 'O', P = 'P', Q = 'Q', R = 'R', S = 'S', T = 'T', U = 'U',
        V = 'V', W = 'W', X = 'X', Y = 'Y', Z = 'Z',

        num0 = '0', num1 = '1', num2 = '2', num3 = '3', num4 = '4',
        num5 = '5', num6 = '6', num7 = '7', num8 = '8', num9 = '9',

        space = ' ',
        tab = '\t',
        enter = '\n',
        backspace = '\b',
        escape = 27, // ASCII ESC

        dash = '-', equal = '=', 
        lbracket = '[', rbracket = ']',
        backslash = '\\',
        semicolon = ';', apostrophe = '\'',
        grave = '`',
        comma = ',', dot = '.', slash = '/',

        // Special (non-ASCII) keys start at a safe offset
        UpArrow = 0x100,
        DownArrow,
        LeftArrow,
        RightArrow,

        // Extendable: F1–F12, Ctrl, Alt, Shift, etc.
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        LeftShift, RightShift,
        LeftCtrl, RightCtrl,
        LeftAlt, RightAlt
    };
    static constexpr std::size_t s_maxSize = 256;

    KeyboardBuffer() = default;

    /**
     * @brief Add a character to the buffer.
     * 
     * If the buffer is full, the oldest character will be overwritten.
     */
    void put(Key c) {
        m_buffer.enqueue(c);
    }

    /**
     * @brief Get a character from the buffer.
     * 
     * If the buffer is empty, this will return 0.
     */
    Key get() {
        return m_buffer.dequeue();
    }

    /**
     * @brief Check if there is data in the buffer.
     */
    bool hasData() const {
        return !m_buffer.isEmpty();
    }
private:
    kpp::CircularQueue<Key, s_maxSize> m_buffer;
};

namespace kernel {
inline KeyboardBuffer keyboardBuffer;
};
