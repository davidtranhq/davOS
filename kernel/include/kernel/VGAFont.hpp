#pragma once

#include <cstdint>

struct BitFont {
	std::size_t characterWidth = 8;
	std::size_t characterHeight = 16;
	std::size_t characterLeftPadding = 1;
	std::size_t characterTopPadding = 1;
};
