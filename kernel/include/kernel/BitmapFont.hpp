#pragma once

#include <cstdint>
#include <kpp/Array.hpp>
#include <kpp/Span.hpp>

template<std::size_t bytesPerCharacter>
struct BitmapFont {
	size_t characterWidth = 8;
	const size_t characterHeight = bytesPerCharacter;
	size_t characterLeftPadding = 1;
	size_t characterTopPadding = 1;
    size_t characterRightPadding = 1;
    size_t characterBottomPadding = 1;
	kpp::Array<uint8_t, 256 * bytesPerCharacter> data;

	inline std::size_t characterTotalWidth() const { return characterWidth + characterLeftPadding + characterRightPadding; }
	inline std::size_t characterTotalHeight() const { return characterHeight + characterTopPadding + characterBottomPadding; }
	inline kpp::Span<uint8_t> characterBitmap(char character) const
	{
		return kpp::Span<uint8_t>(data.data() + character * characterHeight, characterHeight);
	}
};

using VGAFont = BitmapFont<16>;

namespace DefaultFonts {
constexpr VGAFont terminalFont = {
	.characterWidth = 8,
	.characterLeftPadding = 0,
	.characterTopPadding = 0,
    .characterRightPadding = 0,
    .characterBottomPadding = 0,
	.data = {
		#include "fonts/vga_font.raw"
	}
};
} // namespace DefaultFonts
