#pragma once

#include <string>
#include <charconv>

template<typename In = std::string, typename Out = int>
[[nodiscard]] Out Convert(const In& value)
{
	Out res;
	std::from_chars(value.data(), value.data() + value.size(), res);
	return res;
}
