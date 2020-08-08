#pragma once

#include <string>

template <char Suffix = ' '>
class SuffixString : public std::string
{
public:
	template <typename ...T>
	SuffixString(T&&... arg) : std::string(std::forward<T>(arg)...) { }

	friend std::ostream& operator<<(std::ostream& ss, const SuffixString<Suffix>& cs)
	{
		return ss << static_cast<std::string>(cs) << Suffix;
	}
};

template <char Prefix = ' '>
class PrefixString : public std::string
{
public:
	template <typename ...T>
	PrefixString(T&&... arg) : std::string(std::forward<T>(arg)...) { }

	friend std::ostream& operator<<(std::ostream& ss, const PrefixString<Prefix>& cs)
	{
		return ss << Prefix << static_cast<std::string>(cs);
	}
};

template <char Prefix = ' ', char Suffix = ' '>
class PrefixSuffixString : public std::string
{
public:
	template <typename ...T>
	PrefixSuffixString(T&&... arg) : std::string(std::forward<T>(arg)...) { }

	friend std::ostream& operator<<(std::ostream& ss, const PrefixSuffixString<Prefix, Suffix>& cs)
	{
		return ss << Prefix << static_cast<std::string>(cs) << Suffix;
	}
};

using DoubleQuotes = PrefixSuffixString<'\"', '\"'>;