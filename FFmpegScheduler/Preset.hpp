#pragma once

#include <filesystem>
#include <unordered_map>

#include "Utility/Utility.hpp"

namespace Preset
{
#define MakeConstVar(name, str) constexpr auto name = CuUtil::String::ToBuffer(str)

	constexpr std::array EmptyString = { '\0' };

	MakeConstVar(ThreadId, "$$$thread_id$$$");

	MakeConstVar(Vsync0, "-vsync 0");

	MakeConstVar(HwCuvid, "-hwaccel cuvid");

	MakeConstVar(X264Cuvid, "-c:v h264_cuvid");
	MakeConstVar(X264Mmal, "-c:v h264_mmal");

	MakeConstVar(FrameRate29_97, "-framerate 24000/1001");

	MakeConstVar(Input, "$$$input$$$");
	MakeConstVar(InputPng_d, "$$$input$$$/%d.png");

	MakeConstVar(NvInput, CuUtil::String::Join(" ", HwCuvid, X264Cuvid, Input));

	MakeConstVar(copy, "-c copy");

	MakeConstVar(X264, "-c:v libx264");
	MakeConstVar(X265, "-c:v libx265");
	MakeConstVar(X264Nvenc, "-c:v h264_nvenc");
	MakeConstVar(X264Omx, "-c:v h264_omx");
	MakeConstVar(SvtAv1, "-c:v libsvtav1");
	MakeConstVar(VvEnc, "-c:v libvvenc");

	MakeConstVar(Aac, "-c:a aac");
	MakeConstVar(Opus, "-c:a libopus");

	MakeConstVar(InputCopy, CuUtil::String::Join(" ", Input, copy));

	MakeConstVar(LoudNorm, "-filter:a loudnorm");

	MakeConstVar(ScaleUp60Fps,
		R"(-filter:v "minterpolate='fps=60:mi_mode=mci:mc_mode=aobmc:me_mode=bidir:me=epzs:vsbmc=1:scd=fdiff'")");

	MakeConstVar(Image2, "-f image2");

	MakeConstVar(Size100X100, "-s 100x100");
	MakeConstVar(Size720p, "-s 1280x720");
	MakeConstVar(Resize720p, "-resize 1280x720");

	MakeConstVar(Ac2, "-ac 2");

	MakeConstVar(PresetLossLessHp, "-preset losslesshp");
	MakeConstVar(PresetUltraFast, "-preset ultrafast");
	MakeConstVar(PresetSlower, "-preset slower");
	MakeConstVar(PresetVerySlow, "-preset veryslow");
	MakeConstVar(PresetPlacebo, "-preset placebo");

	MakeConstVar(TuneFilm, "-tune film");

	MakeConstVar(AudioBitrate128k, "-b:a 128k");

	MakeConstVar(AudioS16, "-sample_fmt s16");

	MakeConstVar(Qp0, "-qp 0");

	MakeConstVar(Crf14, "-crf 14");
	MakeConstVar(Crf15, "-crf 15");
	MakeConstVar(Crf17, "-crf 17");
	MakeConstVar(Crf19, "-crf 17");

	MakeConstVar(Yuv420p, "-pix_fmt yuv420p");
	MakeConstVar(Yuv420p10le, "-pix_fmt yuv420p10le");
	MakeConstVar(Yuv444p10le, "-pix_fmt yuv444p10le");

	MakeConstVar(ColorSpaceBt709, "-colorspace 1");
	MakeConstVar(Bt709, CuUtil::String::Join(" ", ColorSpaceBt709, "-color_primaries 1 -color_trc 1"));
	MakeConstVar(Smpte170m, "-colorspace 6 -color_trc 6 -color_primaries 6");

	MakeConstVar(Anima60FpsAvc720pParams,
		R"(-x264-params "deblock='0:0':keyint=600:min-keyint=1:ref=9:qcomp=0.7:rc-lookahead=180:aq-strength=0.9:merange=16:me=tesa:psy-rd='0:0.20':no-fast-pskip=1")");
	MakeConstVar(Anima60FpsAvcParams,
		R"(-x264-params "mbtree=1:aq-mode=3:psy-rd='0.6:0.15':aq-strength=0.8:rc-lookahead=180:qcomp=0.75:deblock='-1:-1':keyint=600:min-keyint=1:bframes=8:ref=13:me=tesa:no-fast-pskip=1")");
	MakeConstVar(Anima60FpsHevcParams,
		R"(-x265-params "deblock='-1:-1':ctu=32:qg-size=8:pbratio=1.2:cbqpoffs=-2:crqpoffs=-2:no-sao=1:me=3:subme=5:merange=38:b-intra=1:limit-tu=4:no-amp=1:ref=4:weightb=1:keyint=600:min-keyint=1:bframes=6:aq-mode=1:aq-strength=0.8:rd=5:psy-rd=2.0:psy-rdoq=1.0:rdoq-level=2:no-open-gop=1:rc-lookahead=180:scenecut=40:qcomp=0.65:no-strong-intra-smoothing=1:")");

	MakeConstVar(AvcAudioComp, CuUtil::String::Join(" ", Aac, AudioBitrate128k /*, AudioS16*/));

	MakeConstVar(AvcLossLess, CuUtil::String::Join(" ", X264, PresetUltraFast, Qp0));
	MakeConstVar(AvcLossLessP10, CuUtil::String::Join(" ", AvcLossLess, Yuv420p10le));

	MakeConstVar(AvcVisuallyLossLess, CuUtil::String::Join(" ", X264, Crf17));
	MakeConstVar(AvcVisuallyLossLessP10, CuUtil::String::Join(" ", AvcVisuallyLossLess, Yuv420p10le));

	MakeConstVar(NvencAvcLossLess, CuUtil::String::Join(" ", X264Nvenc, PresetLossLessHp, Qp0));

	MakeConstVar(NvencAvc720pComp, CuUtil::String::Join(" ", X264Nvenc, PresetLossLessHp, Qp0));
	MakeConstVar(AnimaAvc720pComp,
		CuUtil::String::Join(" ", X264, PresetVerySlow, Crf19, Yuv420p, TuneFilm, Anima60FpsAvc720pParams));

	MakeConstVar(AnimaAvcComp, CuUtil::String::Join(" ", X264, PresetVerySlow, Crf15, Yuv420p10le, Anima60FpsAvcParams));
	MakeConstVar(AnimaAvcCompYuv444,
		CuUtil::String::Join(" ", X264, PresetVerySlow, Crf15, Yuv444p10le, Anima60FpsAvcParams));
	MakeConstVar(AnimaHevcComp, CuUtil::String::Join(" ", X265, PresetSlower, Crf14, Yuv420p10le, Anima60FpsHevcParams));

	MakeConstVar(FormatNull, "-f null");

	MakeConstVar(Output, "$$$output$$$");
	MakeConstVar(OutputJpg, "$$$output$$$.jpg");
	MakeConstVar(OutputPng, "$$$output$$$.png");
	MakeConstVar(OutputWebp, "$$$output$$$.webp");
	MakeConstVar(OutputPng_d, "$$$output$$$/%d.png");
	MakeConstVar(OutputAvif, "$$$output$$$.avif");
	MakeConstVar(OutputMp4, "$$$output$$$.mp4");
	MakeConstVar(OutputMkv, "$$$output$$$.mkv");
	MakeConstVar(OutputOpus, "$$$output$$$.opus");
#ifdef CuUtil_Platform_Windows
	MakeConstVar(OutputNull, "NUL");
#else
	MakeConstVar(OutputNull, "/dev/null");
#endif

	MakeConstVar(Vvc2Pass1, CuUtil::String::Combine(R"(-an -c:v libvvenc -vvenc-params passes=2:pass=1:rcstatsfile=)", ThreadId, ".json"));
	MakeConstVar(Vvc2Pass2, CuUtil::String::Combine(R"(-c:v libvvenc -vvenc-params passes=2:pass=2:rcstatsfile=)", ThreadId, ".json"));
#undef MakeConstVar

	struct Params
	{
		std::filesystem::path Input;
		std::string InputExt;
		std::filesystem::path Output;
		std::string OutputExt;
		std::thread::id Id;
		std::optional<bool> Overwrite;
		std::string InputOptionExt;
		std::string OutputOptionExt;
		std::string GlobalExt;
	};

	using CmdStringType = std::filesystem::path::string_type;

	template <typename Str>
	CmdStringType ToCmdString(const Str& str)
	{
		return CuStr::ToStringAs<CmdStringType>(str);
	}

	CmdStringType operator""_cmd(const char* str, std::size_t)
	{
		return std::filesystem::path(str).string<CmdStringType::value_type>();
	}

	struct IPreset
	{
		IPreset() = default;
		IPreset(const IPreset&) = default;
		IPreset(IPreset&&) = default;
		virtual ~IPreset() = default;

		virtual CmdStringType ToCommand(const Params& params) = 0;

		virtual std::string ToString() = 0;
	};

	template <size_t GlobalS, size_t InputOptS, size_t InputS, size_t OutputOptS, size_t OutputS>
	struct Preset final : IPreset
	{
		std::array<char, GlobalS> Global;
		std::array<char, InputOptS> InputOption;
		std::array<char, InputS> Input;
		std::array<char, OutputOptS> OutputOption;
		std::array<char, OutputS> Output;

		Preset(std::array<char, GlobalS> global,
		       std::array<char, InputOptS> inputOption,
		       std::array<char, InputS> input,
		       std::array<char, OutputOptS> outputOption,
		       std::array<char, OutputS> output) : Global(global), InputOption(inputOption), Input(input),
		                                           OutputOption(outputOption), Output(output)
		{
		}

		CmdStringType ToCommand(const Params& params) override
		{
			std::basic_stringstream<CmdStringType::value_type> ss;

			ss << ToCmdString(Global.data()) << " ";
			ss << ToCmdString(params.GlobalExt) << " ";

			ss << ToCmdString(InputOption.data()) << " ";
			ss << ToCmdString(params.InputOptionExt) << " ";

			const auto inputExp = ToCmdString(
				std::regex_replace(Input.data(), std::regex(R"(\${3}input\${3})"), params.InputExt));
			const auto input = std::regex_replace(inputExp, std::basic_regex(R"(\${3}input\${3})"_cmd),
			                                      params.Input.native());
			ss << "-i " << std::quoted(input) << " ";

			ss << ToCmdString(OutputOption.data()) << " ";
			ss << ToCmdString(params.OutputOptionExt) << " ";
			if (params.Overwrite) ss << ToCmdString(*params.Overwrite ? "-y" : "-n") << " ";

			const auto outputExp = ToCmdString(
				std::regex_replace(Output.data(), std::regex(R"(\${3}output\${3})"), params.OutputExt));
			const auto output = std::regex_replace(outputExp, std::basic_regex(R"(\${3}output\${3})"_cmd),
			                                       params.Output.native());
			ss << std::quoted(output);

			const auto cmd = ss.str();

			return std::regex_replace(cmd, std::basic_regex(R"(\${3}thread_id\${3})"_cmd),
			                          CuStr::CombineAs<CmdStringType>(params.Id));
		}

		std::string ToString() override
		{
			return CuStr::Appends("ffmpeg ", Global.data(), " ", InputOption.data(), " ", Input.data(), " ",
			                      OutputOption.data(), " ", Output.data());
		}
	};

	template <size_t GlobalS, size_t InputOptS, size_t InputS, size_t OutputOptS, size_t OutputS>
	auto Create(std::array<char, GlobalS> global, std::array<char, InputOptS> inputOpt, std::array<char, InputS> input,
	            std::array<char, OutputOptS> outputOpt, std::array<char, OutputS> output)
	{
		return std::make_shared<Preset<GlobalS, InputOptS, InputS, OutputOptS, OutputS>>(
			global, inputOpt, input, outputOpt, output);
	}

	template <size_t InputOptS, size_t InputS, size_t OutputOptS, size_t OutputS>
	auto Create(std::array<char, InputOptS> inputOpt, std::array<char, InputS> input,
	            std::array<char, OutputOptS> outputOpt, std::array<char, OutputS> output)
	{
		return Create(EmptyString, inputOpt, input, outputOpt, output);
	}

	template <size_t InputOptS, size_t OutputOptS, size_t OutputS>
	auto Create(std::array<char, InputOptS> inputOpt, std::array<char, OutputOptS> outputOpt,
	            std::array<char, OutputS> output)
	{
		return Create(inputOpt, Input, outputOpt, output);
	}

	template <size_t OutputOptS, size_t OutputS>
	auto Create(std::array<char, OutputOptS> outputOpt, std::array<char, OutputS> output)
	{
		return Create(EmptyString, outputOpt, output);
	}

	template <size_t OutputS>
	auto Create(std::array<char, OutputS> output)
	{
		return Create(EmptyString, output);
	}

	inline auto Create()
	{
		return Create(Output);
	}

	template <typename... Args>
	consteval auto CombineOption(Args&&... args)
	{
		return CuUtil::String::Join(" ", std::forward<Args>(args)...);
	}

	inline std::unordered_map<std::string, std::vector<std::shared_ptr<IPreset>>> Presets{
		{"avif", {Create(OutputAvif)}},
		{"opus", {Create(OutputOpus)}},
		{"vvc,opus,mp4", {Create(CuUtil::String::Join(" ", VvEnc, Opus), OutputMp4)}},
		{"vvc,2pass,opus,mp4", {Create(CombineOption(Vvc2Pass1, FormatNull), OutputNull), Create(CombineOption(Vvc2Pass2, Opus), OutputMp4)}},
		{"svtav1,p10le,opus,mkv", {Create(CombineOption(SvtAv1, Yuv420p10le, Opus), OutputMkv)}},
		{
			"easydecode",
			{
				Create(CombineOption(Vsync0, HwCuvid), X264Cuvid, Input, CombineOption(Resize720p, X264Nvenc, Ac2),
				       OutputMp4)
			}
		},
		{"pass", {Create()}},
		{"anima,avc,720p", {Create(CombineOption(copy, Size720p, AnimaAvc720pComp), OutputMkv)}},
		{"anima,avc", {Create(CombineOption(copy, AnimaAvcComp), OutputMkv)}},
		{"anima,hevc", {Create(CombineOption(copy, AnimaHevcComp), OutputMkv)}},
		{"anima,upto60fps,ll", {Create(CombineOption(copy, ScaleUp60Fps, AvcLossLessP10), OutputMkv)}},
		{"anima,upto60fps,avc", {Create(CombineOption(copy, ScaleUp60Fps, AnimaAvcComp), OutputMkv)}},
		{"avc,ll", {Create(CombineOption(copy, AvcLossLess), OutputMkv)}},
		{"avc,vll", {Create(CombineOption(copy, AvcVisuallyLossLess), OutputMkv)}},
		{"raspberrypi", {Create(X264Mmal, Input, X264Omx, OutputMkv)}},
		{"jpg", {Create(OutputJpg)}},
		{"webp", {Create(OutputWebp)}},
		{"png", {Create(OutputPng)}},
		{"vid2png%d", {Create(Image2, OutputPng_d)}},
		{"png%d2mp4", {Create(FrameRate29_97, InputPng_d, CombineOption(AvcLossLess, Yuv420p10le), OutputMkv)}},
		{"loudnorm", {Create(CombineOption(copy, LoudNorm), OutputMkv)}},
	};
}
