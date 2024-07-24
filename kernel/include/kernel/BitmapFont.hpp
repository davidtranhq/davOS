#pragma once

#include <cstdint>
#include <dav/Array.hpp>
#include <dav/Span.hpp>

template<std::size_t bytesPerCharacter>
struct BitmapFont {
	size_t characterWidth = 8;
	const size_t characterHeight = bytesPerCharacter;
	size_t characterLeftPadding = 1;
	size_t characterTopPadding = 1;
	dav::Array<uint8_t, 256 * bytesPerCharacter> data;

	inline std::size_t characterTotalWidth() const { return characterWidth + characterLeftPadding; }
	inline std::size_t characterTotalHeight() const { return characterHeight + characterTopPadding; }
	inline dav::Span<uint8_t> characterBitmap(char character) const
	{
		return dav::Span<uint8_t>(data.data() + character * characterHeight, characterHeight);
	}
};

using VGAFont = BitmapFont<16>;

namespace DefaultFonts {
constexpr VGAFont terminalFont = {
	.characterWidth = 8,
	.characterLeftPadding = 0,
	.characterTopPadding = 0,
	.data = {
		#include "fonts/vga_font.raw"
	}
};
} // namespace DefaultFonts
