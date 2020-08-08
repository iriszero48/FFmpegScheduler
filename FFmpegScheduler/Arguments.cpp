#include "Arguments.h"

std::string Arguments::GetDesc()
{
	std::ostringstream ss;
	for (auto& arg : args)
	{
		ss << SuffixString<'\n'>(Combine(SuffixString<'-'>(std::string(4, ' ')),
			SuffixString(arg.first),
			arg.second->GetDesc()));
	}
	return ss.str();
}

void Arguments::Parse(const int argc, char** argv)
{
	if (argc < 3 || (argc & 1) == 0)
	{
		ThrowEx(SuffixString(argv[0]), " [options]\n", GetDesc());
	}
	for (auto i = 1; i < argc; i += 2)
	{
		if (argv[i][0] != '-' || args.find(argv[i] + 1) == args.end())
		{
			ThrowEx(argv[i], ": Option not found");
		}
		args.at(argv[i] + 1)->Set(argv[i + 1]);
	}
}

IArgument* Arguments::operator[](const std::string& arg)
{
	return args.at(arg);
}
