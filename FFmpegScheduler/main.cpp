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
#include <mutex>
#include <barrier>
#include <iostream>
#include <execution>
#include <sstream>

#include "Convert/Convert.hpp"
#include "Arguments/Arguments.hpp"
#include "File/File.hpp"
#include "Enum/Enum.hpp"
#include "String/String.hpp"
#include "File/Directory.hpp"
#include "Thread/Thread.hpp"
#include "Assert/Assert.hpp"
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
            const std::thread::id& id, const bool printOnly, const bool createNewWindow, CuThread::Semaphore* semaphore)
{
	const auto idStr = CuStr::Combine(id);

	if (semaphore) semaphore->WaitOne();

	if (*cmd.rbegin() != Preset::AndCh) cmd.push_back(Preset::AndCh);
	std::string cmdLine;
	std::stringstream ss(cmd);
	while (std::getline(ss, cmdLine))
	{
		const auto execCmd = ReplaceCommandLine(cmdLine, input, output, id);

		CuConsole::WriteLine(CuStr::ToDirtyUtf8StringView(
			CuStr::FormatU8("[{}] [{}] -> {} {}", CuStr::Combine(std::chrono::system_clock::now()), idStr, ffmpeg, execCmd)));

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

	if (semaphore) semaphore->Release();
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

		args.Add(helpArg, inputArg, outputArg, logArg, threadArg, customArg, modeArg, execArg, inputExtensionArg,
		         outputExtensionArg, presetArg, printOnlyArg);

		args.Parse(argc, argv);

		if (args.Value(helpArg))
		{
			CuConsole::WriteLine(args.GetDesc());
			return EXIT_SUCCESS;
		}

		CuConsole::WriteLine(args.GetValuesDesc());

		const auto printOnly = args.Value(printOnlyArg);
		const auto ffmpegExec = args.Value(execArg);
		const auto rawSubCmd = args.Get(customArg) ? args.Value(customArg) : Preset::Presets.at(args.Value(presetArg));
		const auto threadNum = args.Value(threadArg);
		const bool createNewWindow = CuUtil::ToUnderlying(args.Get(createNewWindowArg).value_or(threadNum != 1 ? CreateNewWindow::On : CreateNewWindow::Off));

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
		std::ranges::stable_sort(files);

		const auto process = [&](const std::filesystem::path& file, CuThread::Semaphore* semaphore)
		{
			CuConsole::WriteLine(CuStr::ToDirtyUtf8StringView(CuStr::FormatU8("-> {}", file)));

			const auto output = outputPath ? *outputPath / file.filename() : file.parent_path() / file.filename();
			FFmpeg(rawCmd, ffmpegExec, file, output, std::this_thread::get_id(), printOnly, createNewWindow, semaphore);
		};

		if (threadNum == 1)
		{
			for (const auto& file : files)
			{
				process(file, nullptr);
			}
		}
		else
		{
			CuThread::Semaphore semaphore(threadNum);
			std::vector<std::thread> workers;
			for (const auto& file : files)
			{
				workers.emplace_back([&]
				{
					const auto logFile = logPath / CuStr::Combine(std::this_thread::get_id(), ".log");
					process(file, &semaphore);
				});
			}
		}
	}
	catch (const std::exception& e)
	{
		CuConsole::Error::Write(e.what());
		return EXIT_FAILURE;
	}
}
