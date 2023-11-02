#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static bool print(const char *data, size_t length)
{
    const unsigned char *bytes = (const unsigned char *)data;
    for (size_t i = 0; i < length; i++)
        if (putchar(bytes[i]) == EOF)
            return false;
    return true;
}

static char hex[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

int vprintf(const char *format, va_list args)
{
    size_t written = 0;
    while (*format != '\0')
    {
        // print regular characters and escaped %'s
        if (format[0] != '%' || format[1] == '%')
        {
            // consume the escaping %
            if (format[0] == '%')
                format += 1;
            // print until the next format specifier or the end of the string
            size_t chars_to_print = 1;
            while (format[chars_to_print] && format[chars_to_print] != '%')
                chars_to_print += 1;
            if (!print(format, chars_to_print))
                return -1;
            format += chars_to_print;
            written += chars_to_print;
        }
        // handle format specifiers
        else
        {
            // consume the %
            format += 1;
            if (format[0] == 'c')
            {
                // consume the specifier
                format += 1;
                // standard mandates that variable arguments are promoted to at least int,
                // so we accept an int and convert it to a char
                char c = (char)va_arg(args, int);
                if (!print(&c, sizeof(c)))
                    return -1;
                written += 1;
            }
            else if (format[0] == 's')
            {
                format += 1;
                const char *str = va_arg(args, const char *);
                size_t len = strlen(str);
                if (!print(str, len))
                    return -1;
                written += len;
            }
            else if (format[0] == 'd')
            {
                format += 1;
                int num = va_arg(args, int);
                const int max_num_len = 128;
                char str[max_num_len] = {0};
                int str_start = max_num_len - 1;
                if (num == 0)
                {
                    str[str_start] = '0';
                    str_start -= 1;
                }
                else
                {
                    while (num > 0)
                    {
                        int digit = num % 10;
                        str[str_start] = digit + '0';
                        str_start -= 1;
                        num /= 10;
                    }
                }
                str_start += 1;
                size_t str_len = max_num_len - str_start;
                if (!print(str + str_start, str_len))
                    return -1;
                written += str_len;
            }
            else if (format[0] == 'x' || format[0] == 'p')
            {
                format += 1;
                unsigned long long num = va_arg(args, unsigned long long);
                const int max_num_len = 128;
                char str[max_num_len] = {0};
                int str_start = max_num_len - 1;
                str[str_start] = max_num_len - 1;
                if (num == 0)
                {
                    str[str_start] = '0';
                    str_start -= 1;
                }
                else
                {
                    while (num > 0)
                    {
                        int digit = hex[num % 16];
                        str[str_start] = digit;
                        str_start -= 1;
                        num /= 16;
                    }
                }
                str_start += 1;
                size_t str_len = max_num_len - str_start;
                if (!print(str + str_start, str_len))
                    return -1;
                written += str_len;
            }
        }
    }
    
    return written > INT_MAX ? -1 : written;
}

int printf(const char *__restrict format, ...)
{
    va_list args;
    va_start(args, format);
    int written = vprintf(format, args);
    va_end(args);
    return written;
}
