#pragma once

#include <vector>
#include <functional>
#include <filesystem>
#include <functional>

template<typename Element = std::filesystem::directory_entry, typename Container = std::vector<Element>>
[[nodiscard]] Container GetFiles(
	const std::filesystem::path& path,
	std::function<void(Container&, Element)> insertFunc = [](auto& files, const auto& file) { files.push_back(file); })
{
	Container files{};
	for (const auto& file :
		std::filesystem::directory_iterator(path, std::filesystem::directory_options::skip_permission_denied))
	{
		if (file.is_regular_file())
		{
			insertFunc(files, file);
		}
	}
	return files;
}
