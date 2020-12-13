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

#include "Arguments.h"
#include "Convert.h"
#include "File.h"
#include "Semaphore.h"
#include "Preset.h"

std::unordered_map<std::string, std::string> Preset
{
	{"anima,avc,720p,comp,colorspace=bt709", Combine(InputCopy, Size720p, AvcAudioComp, AnimaAvc720pComp, ColorSpaceBt709, Output)},

	{"anima,avc,comp", Combine(InputCopy, AnimaAvcComp, Output)},
	{"anima,avc,comp,colorspace=bt709", Combine(InputCopy, AnimaAvcComp, ColorSpaceBt709, Output)},
	{"anima,avc,comp,bt709", Combine(InputCopy, AnimaAvcComp, Bt709, Output)},

	{"anima,hevc,comp", Combine(InputCopy, AnimaHevcComp, Output)},
	{"anima,hevc,comp,colorspace=bt709", Combine(InputCopy, AnimaHevcComp, ColorSpaceBt709, Output)},
	{"anima,hevc,comp,bt709", Combine(InputCopy, AnimaHevcComp, Bt709, Output)},

	{"anima,upto60fps,avc,ll", Combine(InputCopy, ScaleUp60Fps, AvcLossLessP10, Output)},
	{"anima,upto60fps,avc,ll,colorspace=bt709", Combine(InputCopy, ScaleUp60Fps, AvcLossLessP10, ColorSpaceBt709, Output)},
	{"anima,upto60fps,avc,ll,bt709", Combine(InputCopy, ScaleUp60Fps, AvcLossLessP10, Bt709, Output)},

	{"anima,upto60fps,avc,comp", Combine(InputCopy, ScaleUp60Fps, AnimaAvcComp, Output)},
	{"anima,upto60fps,avc,comp,colorspace=bt709", Combine(InputCopy, ScaleUp60Fps, AnimaAvcComp, ColorSpaceBt709, Output)},
	{"anima,upto60fps,avc,comp,bt709", Combine(InputCopy, ScaleUp60Fps, AnimaAvcComp, Bt709, Output)},

	{"anima,upto60fps,avc,comp,444p10,colorspace=bt709", Combine(InputCopy, ScaleUp60Fps, AnimaAvcCompYuv444, ColorSpaceBt709, Output)},

	{"avc,ll", Combine(InputCopy, AvcLossLess, Output)},
	{"avc,vll", Combine(InputCopy, AvcVisuallyLossLess, Output)},
	{"avc,vll,p10", Combine(InputCopy, AvcVisuallyLossLessP10, Output)},
	{"avc,vll,p10,bt709", Combine(InputCopy, AvcVisuallyLossLessP10, Bt709, Output)},
	{"avc,vll,p10,smpte170m", Combine(InputCopy, AvcVisuallyLossLessP10, Smpte170m, Output)},

	{"avc", Combine(Input, X264, Output)},
	{"avc,bt709", Combine(Input, X264, Bt709, Output)},
	{"avc,placebo", Combine(Input, X264, PresetPlacebo, Output)},

	{"avc,720p,nvenc,colorspace=bt709", Combine(InputCopy, Size720p, AvcAudioComp, X264Nvenc, Yuv420p, ColorSpaceBt709, Output)},

	{"nv,avc", Combine(NvInput, X264Nvenc, Output)},
	{"nv,avc,ll", Combine(NvInput, NvencAvcLossLess, Output)},

	{"rp,avc", Combine(X264Mmal, Input, X264Omx, Output)},

	{"pic,jpg", Combine(Input, OutputJpg)},
	{"pic,png", Combine(Input, OutputPng)},

	{"pic,png,resize100x100", Combine(Input, Size100X100, OutputPng)},

	{"vid,mp4", Combine(Input, OutputMp4)},

	{"vid2png%d", Combine(Input, Image2, OutputPng_d)},

	{"png%d2mp4,29.97fps,p10le", Combine(FrameRate29_97, InputPng_d, Yuv420p10le, Output)},

	{"loudnorm", Combine(InputCopy, LoudNorm, Output)},

	{"i", Combine(Input)},

	{"easydecode", Combine(Vsync0, HwCuvid, X264Cuvid, Resize720p, Input, X264Nvenc, Ac2, OutputMp4)}
};

[[nodiscard]] std::string PresetDesc()
{
	std::ostringstream ss;
	for (auto& preset : Preset)
	{
		ss << SuffixString<'\n'>(Combine(std::string(8, ' '), preset.first, " => ", preset.second));
	}
	return ss.str();
}

int main(int argc, char* argv[])
{
#define InvalidArgument(v) Argument<>::ConstraintFuncMsg{ Combine(v, ": Invalid argument") }
#define InvalidArgumentFunc(func) [](const auto& v) { return (func) ? std::nullopt : InvalidArgument(v); }

	try
	{
		Arguments args{};

		Argument input("i", "input");
		Argument output("o", "output");
		Argument log("l", "log path");
		Argument<int> thread(
			"t",
			"thread",
			1,
			{ InvalidArgumentFunc(v > 0) },
			Convert<std::string, int>);
		Argument custom("custom", "custom");
		Argument mode(
			"mode",
			"[(f)|d] file/directory",
			{ "f" },
			{ InvalidArgumentFunc(v == "f" || v == "d") });
		Argument<bool> move(
			"move",
			"[(y)|n] move when done",
			{ true },
			{ InvalidArgumentFunc(true) },
			{ [](const auto& v) { return !(v == "n"); } });
		Argument call(
			"call",
			"(ffmpeg) call ffmpeg",
			{ "ffmpeg" });
		Argument inputExtension(
			"ie",
			"input extension",
			{ "" });
		Argument outputExtension(
			"oe",
			"output extension",
			{ "" });
		Argument preset(
			"p",
			Combine("preset\n", PresetDesc()),
			{},
			{ InvalidArgumentFunc(Preset.find(v) != Preset.end()) });

		args.Add(input);
		args.Add(output);
		args.Add(log);
		args.Add(thread);
		args.Add(custom);
		args.Add(mode);
		args.Add(move);
		args.Add(inputExtension);
		args.Add(outputExtension);
		args.Add(call);
		args.Add(preset);

		args.Parse(argc, argv);

		const std::regex inputRe(R"(\${3}input.?\${3})");
		const std::regex outputRe(R"(\${3}output\${3})");
		const std::regex inputExtensionRe(R"("\${3}output\${3}")");
		const std::regex outputExtensionRe(R"("\${3}output\${3}.*?")");

		const auto extendPresetCmd = std::regex_replace(
			std::regex_replace(
				Combine(SuffixString(args.Value(call)), args.Get(custom) ? args.Value(custom) : Preset[args.Value(preset)]),
				outputExtensionRe,
				Combine(SuffixString(args.Value(outputExtension)), "$&")),
			inputExtensionRe,
			Combine(SuffixString(args.Value(inputExtension)), "$&"));

		const auto inputPath = std::filesystem::path(args.Value(input));
		const auto outputPath = args.Get(output) ? std::filesystem::path(args.Value(output)) : inputPath / "done";

		create_directory(outputPath);

		if (args.Value(mode) == "f")
		{
			const auto moveWhenDone = args.Value<decltype(move)::ValueType>(move);
			const auto threadNum = args.Value<decltype(thread)::ValueType>(thread);

			const auto rawPath = inputPath / "raw";
			const auto logPath = args.Get(log) ? std::filesystem::path(args.Value(log)) : inputPath / "log";

			if (moveWhenDone) create_directory(rawPath);
			if (threadNum != 1) create_directory(logPath);

			auto files = GetFiles(inputPath);

			std::stable_sort(std::execution::par_unseq, files.begin(), files.end());

			std::mutex idMtx{};
			Semaphore cs(threadNum);

			auto ffmpeg = [&, count = 0](const auto& file) mutable
			{
				cs.WaitOne();
				idMtx.lock();
				auto id = count++;
				idMtx.unlock();

				const auto currFile = inputPath / file.path().filename();
				const auto cmd =
					Combine(
						std::regex_replace(
							std::regex_replace(
								extendPresetCmd,
								inputRe,
								currFile.string()),
							outputRe,
							(outputPath / file.path().filename()).string()),
						threadNum == 1 ? "" : Combine(" >", DoubleQuotes((logPath / Combine("log.", id)).string()), " 2>&1"));
				printf("\n>>> %s\n\n", cmd.c_str());
				system(cmd.c_str());

				if (moveWhenDone)
				{
					try
					{
						rename(currFile, rawPath / file.path().filename());
					}
					catch (...)
					{
						using namespace std::chrono_literals;
						std::this_thread::sleep_for(+1s);
						rename(currFile, rawPath / file.path().filename());
					}
				}
				cs.Release();
			};

			if (threadNum == 1)
			{
				if (args.Value<decltype(move)::ValueType>(move))
				{
					for (; !files.empty();
						files = GetFiles(inputPath),
						std::stable_sort(std::execution::par_unseq, files.begin(), files.end()))
					{
						ffmpeg(files[0]);
					}
				}
				else
				{
					std::for_each(files.begin(), files.end(), ffmpeg);
				}
			}
			else
			{
				std::for_each(std::execution::par, files.begin(), files.end(), ffmpeg);
			}
		}
		else
		{
			const auto cmd =
				std::regex_replace(
					std::regex_replace(
						extendPresetCmd,
						inputRe,
						inputPath.string()),
					outputRe,
					outputPath.string());
			printf("\n>>> %s\n\n", cmd.c_str());
			system(cmd.c_str());
		}
	}
	catch (const std::exception& e)
	{
		fputs(e.what(), stderr), exit(EXIT_FAILURE);
	}
}
// g++<10 may need tbb
// g++ fs.cpp -o fs -std=c++17 -ltbb -O2