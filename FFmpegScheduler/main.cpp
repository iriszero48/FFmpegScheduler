#include <cstdio>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <filesystem>
#include <regex>
#include <functional>
#include <optional>
#include <thread>
#include <chrono>
#include <exception>
#include <execution>
#include <iostream>
#include <sstream>

#include "Arguments/Arguments.hpp"
#include "Enum/Enum.hpp"
#include "String/String.hpp"
#include "File/Directory.hpp"
#include "Thread/Thread.hpp"
#include "StdIO/StdIO.hpp"

#include "Preset.hpp"

#ifdef CuUtil_Platform_Windows
#include <Windows.h>
#include <WinUser.h>
#endif

[[nodiscard]] std::string PresetDesc()
{
	std::ostringstream ss;
	for (const auto& [fst, snd] : Preset::Presets)
	{
		ss << CuStr::Combine(std::string(4, ' '), fst, " => ");

		for (size_t i = 0; i < snd.size(); ++i)
		{
			if (i != 0) ss << " && ";
			ss << snd[i]->ToString();
		}
		ss << "\n";
	}
	return ss.str();
}

CuEnum_MakeEnum(InputMode, File, Dir);
CuEnum_MakeEnum(CreateNewWindow, Off = 0, On = 1);
CuEnum_MakeEnum(FilterPreset, p, v, a);

using CmdStringType = std::filesystem::path::string_type;

CmdStringType operator""_cmd(const char* str, std::size_t)
{
	return std::filesystem::path(str).string<CmdStringType::value_type>();
}

template <typename T>
T ApplyReplace(T str, const std::unordered_map<T, T>& replaceList)
{
	for (const auto& rep : replaceList)
	{
		str = std::regex_replace(str, std::basic_regex(rep.first), rep.second);
	}
	return str;
}

CmdStringType ReplaceCommandLine(const std::string& cmd,
                                 const std::filesystem::path& input, const std::filesystem::path& output,
                                 const std::thread::id& id)
{
	return ApplyReplace(CuStr::ToStringAs<CmdStringType>(cmd), {
		                    {R"(\${3}input\${3})"_cmd, input.native()},
		                    {R"(\${3}output\${3})"_cmd, output.native()},
		                    {R"(\${3}thread_id\${3})"_cmd, CuStr::CombineAs<CmdStringType>(id)}
	                    });
}


void FFmpeg(const decltype(Preset::Presets)::value_type::second_type& cmds, const std::filesystem::path& ffmpeg, Preset::Params params, const bool printOnly, const bool createNewWindow)
{
	for (const auto& cmd : cmds)
	{
		const auto cmdStr = cmd->ToCommand(params);

		CuConsole::WriteLine(CuStr::ToDirtyUtf8StringView(
			CuStr::FormatU8("[{}] -> {} {}", CuStr::Combine(params.Id), ffmpeg, cmdStr)));

		if (!printOnly)
		{
#ifdef CuUtil_Platform_Windows
			STARTUPINFOW si;
			PROCESS_INFORMATION pi;

			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));

			// Start the child process. 
			auto args = CuStr::CombineW(ffmpeg, " ", cmdStr);
			if (!CreateProcess(ffmpeg.native().c_str(),   // No module name (use command line)
				args.data(),        // Command line
				nullptr,           // Process handle not inheritable
				nullptr,           // Thread handle not inheritable
				FALSE,          // Set handle inheritance to FALSE
				createNewWindow ? CREATE_NEW_CONSOLE : 0,              // No creation flags
				nullptr,           // Use parent's environment block
				nullptr,           // Use parent's starting directory 
				&si,            // Pointer to STARTUPINFO structure
				&pi)           // Pointer to PROCESS_INFORMATION structure
				)
			{
				throw CuExcept::U8Exception(CuStr::FormatU8("CreateProcess failed ({})", static_cast<uint64_t>(GetLastError())));
			}

			// Wait until child process exits.
			WaitForSingleObject(pi.hProcess, INFINITE);

			// Close process and thread handles. 
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
#else
			std::string args{};
			if (createNewWindow)
			{
				args = CuStr::Combine("gnome-terminal -e ", std::quoted(cmdStr, '\''));
			}
			else
			{
				args = cmdStr;
			}
			system(args.c_str());
#endif
		}
	}
}

int main(int argc, const char* argv[])
{
	try
	{
		CuArgs::Arguments args{};

		CuArgs::BoolArgument helpArg("-h", "help");
		CuArgs::Argument inputArg("-i", "input");
		CuArgs::Argument outputArg("-o", "output");
		CuArgs::Argument logArg("-l", "log path");
		CuArgs::Argument threadArg("-t", "thread", 1u);
		CuArgs::EnumArgument modeArg("-m", "[(File)|Dir] input file/directory mode", InputMode::File);
		CuArgs::Argument<std::string> execArg("-e", "ffmpeg exec path", "ffmpeg");
		CuArgs::Argument<std::string> inputExtensionArg("-ie", "input extension", "$&");
		CuArgs::Argument<std::string> outputExtensionArg("-oe", "output extension", "$&");
		CuArgs::Argument presetArg("-p", CuStr::Combine("preset\n", PresetDesc()));
		CuArgs::BoolArgument printOnlyArg("--print-only", "print only");
		CuArgs::EnumArgument<CreateNewWindow> createNewWindowArg("--create-new-window", "create new window [(Auto)|On|Off]");
		CuArgs::Argument filterReArg("-f", "filter (JavaScript regex)");
		filterReArg.Validate = [](const std::string& val)
		{
			if (val.length() < 3) return false;
			if (val[0] != '/') return false;
			if (const auto endPos = val.find_last_of('/'); endPos == std::string::npos || endPos == 0) return false;

			return true;
		};
		CuArgs::EnumArgument<FilterPreset> filterPresetArg("-fp", "filter preset [p|v|a]");
		CuArgs::Argument<std::string> inputOptionArg("-io", "input option", "");
		CuArgs::Argument<std::string> outputOptionArg("-oo", "output option", "");
		CuArgs::Argument<std::string> globalOptionArg("-go", "global option", "");
		CuArgs::Argument<bool> overwriteArg("--overwrite", "overwrite [y|n]",
			[](const auto& value)
			{
				const auto val = CuStr::ToLower(value);
				if (val == "y") return true;
				if (val == "n") return false;
				throw CuExcept::Exception("invalid value");
			});

		args.Add(helpArg, inputArg, outputArg, logArg, threadArg, modeArg, execArg, inputExtensionArg,
		         outputExtensionArg, presetArg, printOnlyArg, createNewWindowArg, filterReArg, filterPresetArg, inputOptionArg, outputOptionArg, globalOptionArg, overwriteArg);

		args.Parse(argc, argv);

		if (args.Value(helpArg))
		{
			CuConsole::WriteLine(args.GetDesc());
			return EXIT_SUCCESS;
		}

		CuConsole::WriteLine(args.GetValuesDesc());

		const auto printOnly = args.Value(printOnlyArg);
		const auto ffmpegExec = args.Value(execArg);
		const auto ffPreset = Preset::Presets.at(args.Value(presetArg));
		const auto threadNum = args.Value(threadArg);
		const bool createNewWindow = CuUtil::ToUnderlying(args.Get(createNewWindowArg).value_or(threadNum != 1 ? CreateNewWindow::On : CreateNewWindow::Off));
		auto filterRe = args.Get(filterReArg);
		const auto filterPreset = args.Get(filterPresetArg);
		const auto inputOption = args.Value(inputOptionArg);
		const auto outputOption = args.Value(outputOptionArg);
		const auto globalOption = args.Value(globalOptionArg);
		const auto overwrite = args.Get(overwriteArg);
		const auto inputExt = args.Value(inputExtensionArg);
		const auto outputExt = args.Value(outputExtensionArg);

		const std::filesystem::path inputPath = args.Value(inputArg);
		const auto outputPath = args.Get(outputArg);

		if (outputPath) std::filesystem::create_directories(*outputPath);

		const std::filesystem::path logPath = args.Get(logArg).value_or("log");

		if (threadNum != 1) create_directories(logPath);

		std::vector<std::filesystem::path> files;
		if (args.Value(modeArg) == InputMode::File)
			files = GetFiles(
				inputPath, CuDirectory::IteratorOptions_RecurseSubdirectories);
		else files.push_back(inputPath);
		CuRanges::StableSort(files);

		if (!filterRe && filterPreset)
		{
			static std::unordered_map<FilterPreset, std::string> presets
			{
				{FilterPreset::p, R"(/.+\.(jpg|jpeg|png|psd|bmp|gif)/i)"},
				{FilterPreset::v, R"(/.+\.(mp4|mkv|avi|rm|rmvb|wmv|flv|mpg|ts|webm|m2ts|m4v)/i)"},
				{FilterPreset::a, R"(/.+\.(mp3|wav|mka|aac|flac)/i)"}
			};

			filterRe = presets.at(*filterPreset);
		}

		if (filterRe)
		{
			constexpr auto being = 1;
			const auto end = filterRe->find_last_of('/');
			const auto mode = filterRe->substr(end + 1);
			filterRe = filterRe->substr(being, end - being);
			auto flags = std::regex_constants::ECMAScript | std::regex_constants::optimize;
			if (mode == "i")
			{
				flags |= std::regex_constants::icase;
			}
			else if (!mode.empty())
			{
				throw CuExcept::Exception("invalid regex flags");
			}

			CuConsole::WriteLine(CuStr::Combine("using filter: `", *filterRe, "` 0x", std::hex, flags));
			auto re = std::regex(CuStr::ToDirtyUtf8String(CuStr::ToU8String(*filterRe)), flags);
			std::erase_if(files, [&re](const auto& file)
			{
				return !std::regex_match(CuStr::ToDirtyUtf8String(file.u8string()).c_str(), re);
			});
		}

		const auto process = [&](const std::filesystem::path& file, const size_t count)
		{
			static std::atomic_size_t index = 0;
			CuConsole::WriteLine(CuStr::ToDirtyUtf8StringView(CuStr::FormatU8("-> ({}/{}) {}", ++index, count, file)));

			const auto output = outputPath ? *outputPath / file.filename() : file.parent_path() / file.filename();
			FFmpeg(ffPreset, ffmpegExec, {
				file,
				inputExt,
				output,
				outputExt,
				std::this_thread::get_id(),
				overwrite,
				inputOption,
				outputOption,
				globalOption
			}, printOnly, createNewWindow);
		};

		if (threadNum == 1)
		{
			for (const auto& file : files)
			{
				process(file, files.size());
			}
		}
		else
		{
			CuThread::Semaphore semaphore(threadNum);
			CuThread::Channel<std::filesystem::path> channel{};
			std::vector<std::thread> workers(threadNum);
			CuRanges::Generate(workers, [&]()
			{
				return std::thread([&]()
				{
					while (true)
					{
						const auto file = channel.Read();
						if (file.empty()) break;

						const auto logFile = logPath / CuStr::Combine(std::this_thread::get_id(), ".log");
						process(file, files.size());
					}
				});
			});
			
			for (const auto& file : files)
			{
				channel.Write(std::filesystem::path(file));
			}

			for (int64_t i = 0; i < threadNum; ++i)
			{
				channel.Write(std::filesystem::path{});
			}

			for (auto& worker : workers)
			{
				worker.join();
			}
		}
	}
	catch (const std::exception& e)
	{
		CuConsole::Error::WriteLine(e.what());
		return EXIT_FAILURE;
	}
}
