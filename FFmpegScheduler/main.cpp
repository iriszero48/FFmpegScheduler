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
		ss << CuStr::Combine(std::string(4, ' '), fst, " => ", snd) << "\n";
	}
	return ss.str();
}

CuEnum_MakeEnum(InputMode, File, Dir);
CuEnum_MakeEnum(CreateNewWindow, Off = 0, On = 1);

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


void FFmpeg(std::string cmd, const std::filesystem::path& ffmpeg, const std::filesystem::path& input,
            const std::filesystem::path& output,
            const std::thread::id& id, const bool printOnly, const bool createNewWindow)
{
	const auto idStr = CuStr::Combine(id);

	if (*cmd.rbegin() != Preset::AndCh) cmd.push_back(Preset::AndCh);
	std::string cmdLine;
	std::stringstream ss(cmd);
	while (std::getline(ss, cmdLine))
	{
		const auto execCmd = ReplaceCommandLine(cmdLine, input, output, id);

		CuConsole::WriteLine(CuStr::ToDirtyUtf8StringView(
			CuStr::FormatU8("[{}] -> {} {}", idStr, ffmpeg, execCmd)));

		if (!printOnly)
		{
#ifdef CuUtil_Platform_Windows
			STARTUPINFOW si;
			PROCESS_INFORMATION pi;

			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));

			// Start the child process. 
			auto args = CuStr::CombineW(ffmpeg, " ", execCmd);
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
			if (createNewWindow) args.append("gnome-terminal -e '");
			CuStr::AppendsTo(args, ffmpeg, " ", execCmd);
			if (createNewWindow) args.append("'");
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
		CuArgs::Argument customArg("-c", "custom command");
		CuArgs::EnumArgument modeArg("-m", "[(File)|Dir] input file/directory mode", InputMode::File);
		CuArgs::Argument<std::string> execArg("-e", "ffmpeg exec path", "ffmpeg");
		CuArgs::Argument<std::string> inputExtensionArg("-ie", "input extension", "$&");
		CuArgs::Argument<std::string> outputExtensionArg("-oe", "output extension", "$&");
		CuArgs::Argument presetArg("-p", CuStr::Combine("preset\n", PresetDesc()));
		CuArgs::BoolArgument printOnlyArg("--print-only", "print only");
		CuArgs::EnumArgument<CreateNewWindow> createNewWindowArg("--create-new-window", "create new window [(Auto)|On|Off]");
		CuArgs::Argument filterReArg("-f", "filter");

		args.Add(helpArg, inputArg, outputArg, logArg, threadArg, customArg, modeArg, execArg, inputExtensionArg,
		         outputExtensionArg, presetArg, printOnlyArg, createNewWindowArg, filterReArg);

		args.Parse(argc, argv);

		if (args.Value(helpArg))
		{
			CuConsole::WriteLine(args.GetDesc());
			return EXIT_SUCCESS;
		}

		CuConsole::WriteLine(args.GetValuesDesc());

		const auto printOnly = args.Value(printOnlyArg);
		const auto ffmpegExec = args.Value(execArg);
		const auto rawSubCmd = args.Get(customArg).has_value() ? args.Value(customArg) : Preset::Presets.at(args.Value(presetArg));
		const auto threadNum = args.Value(threadArg);
		const bool createNewWindow = CuUtil::ToUnderlying(args.Get(createNewWindowArg).value_or(threadNum != 1 ? CreateNewWindow::On : CreateNewWindow::Off));
		const auto filterRe = args.Get(filterReArg);

		const auto rawCmd = ApplyReplace(rawSubCmd, {
			                                 {R"("\${3}input\${3}")", args.Value(inputExtensionArg)},
			                                 {R"("\${3}output\${3}.*?")", args.Value(outputExtensionArg)}
		                                 });

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
		if (filterRe)
		{
			auto re = std::regex(CuStr::ToDirtyUtf8String(CuStr::ToU8String(*filterRe)));
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
			FFmpeg(rawCmd, ffmpegExec, file, output, std::this_thread::get_id(), printOnly, createNewWindow);
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
