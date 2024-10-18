#pragma once

namespace dav {

template<typename T>
T abs(T value)
{
	return value < 0 ? -value : value;
}

} // namespace dav
