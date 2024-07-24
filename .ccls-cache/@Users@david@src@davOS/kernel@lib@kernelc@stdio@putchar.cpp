#include <stdio.h>

#include <kernel/Terminal.hpp>

int putchar(int ic)
{
    char c = (char)ic;
	globalTerminal.write(dav::StringView<char>(&c, 1));
    return ic;
}
