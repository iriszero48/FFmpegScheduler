#pragma once

#include <string>
#include <sstream>

template <typename ...Args>
std::string Combine(Args&&... args)
{
	std::ostringstream ss;
	(ss << ... << args);
	return ss.str();
}
