#pragma once

#include <filesystem>
#include <unordered_map>

#include "Utility/Utility.hpp"

namespace Preset
{
#define MakeConstVar(name, str) constexpr auto name = CuUtil::String::ToBuffer(str)

	constexpr auto AndCh = '\n';
	constexpr std::array And{AndCh, '\0'};

	MakeConstVar(ThreadId, "$$$thread_id$$$");

	MakeConstVar(Vsync0, "-vsync 0");

	MakeConstVar(HwCuvid, "-hwaccel cuvid");

	MakeConstVar(X264Cuvid, "-c:v h264_cuvid");
	MakeConstVar(X264Mmal, "-c:v h264_mmal");

	MakeConstVar(FrameRate29_97, "-framerate 29.97");

	MakeConstVar(Input, CuUtil::String::Join(" ", "-i", R"("$$$input$$$")"));
	MakeConstVar(InputPng_d, CuUtil::String::Join(" ", "-i", R"("$$$input$$$/%d.png")"));

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

	MakeConstVar(Output, R"("$$$output$$$")");
	MakeConstVar(OutputJpg, R"("$$$output$$$.jpg")");
	MakeConstVar(OutputPng, R"("$$$output$$$.png")");
	MakeConstVar(OutputPng_d, R"("$$$output$$$/%d.png")");
	MakeConstVar(OutputAvif, R"("$$$output$$$.avif")");
	MakeConstVar(OutputMp4, R"("$$$output$$$.mp4")");
	MakeConstVar(OutputMkv, R"("$$$output$$$.mkv")");
	MakeConstVar(OutputOpus, R"("$$$output$$$.opus")");
#ifdef CuUtil_Platform_Windows
	MakeConstVar(OutputNull, CuUtil::String::Join(" ", FormatNull, "NUL"));
#else
	MakeConstVar(OutputNull, CuUtil::String::Join(" ", FormatNull, "/dev/null"));
#endif

	MakeConstVar(Vvc2Pass1, CuUtil::String::Combine(R"(-an -c:v libvvenc -vvenc-params passes=2:pass=1:rcstatsfile=)", ThreadId, ".json"));
	MakeConstVar(Vvc2Pass2, CuUtil::String::Combine(R"(-c:v libvvenc -vvenc-params passes=2:pass=2:rcstatsfile=)", ThreadId, ".json"));
	MakeConstVar(VidArchVvc_2pass, CuUtil::String::Join(" ", Input, Vvc2Pass1, OutputNull, And, Input, Opus, Vvc2Pass2, OutputMp4));
#undef MakeConstVar

	inline std::unordered_map<std::string, std::string> Presets{
		{"anima,avc,720p,comp,colorspace=bt709", std::string(CuUtil::String::Join(" ", InputCopy, Size720p, AvcAudioComp, AnimaAvc720pComp, ColorSpaceBt709, Output).data())},

		{"anima,avc,comp", std::string(CuUtil::String::Join(" ", InputCopy, AnimaAvcComp, Output).data())},
		{"anima,avc,comp,colorspace=bt709",
		 std::string(CuUtil::String::Join(" ", InputCopy, AnimaAvcComp, ColorSpaceBt709, Output).data())},
		{"anima,avc,comp,bt709", std::string(CuUtil::String::Join(" ", InputCopy, AnimaAvcComp, Bt709, Output).data())},

		{"anima,hevc,comp", std::string(CuUtil::String::Join(" ", InputCopy, AnimaHevcComp, Output).data())},
		{"anima,hevc,comp,colorspace=bt709",
		 std::string(CuUtil::String::Join(" ", InputCopy, AnimaHevcComp, ColorSpaceBt709, Output).data())},
		{"anima,hevc,comp,bt709", std::string(CuUtil::String::Join(" ", InputCopy, AnimaHevcComp, Bt709, Output).data())},

		{"anima,upto60fps,avc,ll",
		 std::string(CuUtil::String::Join(" ", InputCopy, ScaleUp60Fps, AvcLossLessP10, Output).data())},
		{"anima,upto60fps,avc,ll,colorspace=bt709",
		 std::string(
			 CuUtil::String::Join(" ", InputCopy, ScaleUp60Fps, AvcLossLessP10, ColorSpaceBt709, Output).data())},
		{"anima,upto60fps,avc,ll,bt709",
		 std::string(CuUtil::String::Join(" ", InputCopy, ScaleUp60Fps, AvcLossLessP10, Bt709, Output).data())},

		{"anima,upto60fps,avc,comp",
		 std::string(CuUtil::String::Join(" ", InputCopy, ScaleUp60Fps, AnimaAvcComp, Output).data())},
		{"anima,upto60fps,avc,comp,colorspace=bt709",
		 std::string(CuUtil::String::Join(" ", InputCopy, ScaleUp60Fps, AnimaAvcComp, ColorSpaceBt709, Output).data())},
		{"anima,upto60fps,avc,comp,bt709",
		 std::string(CuUtil::String::Join(" ", InputCopy, ScaleUp60Fps, AnimaAvcComp, Bt709, Output).data())},

		{"anima,upto60fps,avc,comp,444p10,colorspace=bt709",
		 std::string(
			 CuUtil::String::Join(" ", InputCopy, ScaleUp60Fps, AnimaAvcCompYuv444, ColorSpaceBt709, Output).data())},

		{"avc,ll", std::string(CuUtil::String::Join(" ", InputCopy, AvcLossLess, Output).data())},
		{"avc,vll", std::string(CuUtil::String::Join(" ", InputCopy, AvcVisuallyLossLess, Output).data())},
		{"avc,vll,p10", std::string(CuUtil::String::Join(" ", InputCopy, AvcVisuallyLossLessP10, Output).data())},
		{"avc,vll,p10,bt709",
		 std::string(CuUtil::String::Join(" ", InputCopy, AvcVisuallyLossLessP10, Bt709, Output).data())},
		{"avc,vll,p10,smpte170m",
		 std::string(CuUtil::String::Join(" ", InputCopy, AvcVisuallyLossLessP10, Smpte170m, Output).data())},

		{"avc", std::string(CuUtil::String::Join(" ", Input, X264, Output).data())},
		{"avc,bt709", std::string(CuUtil::String::Join(" ", Input, X264, Bt709, Output).data())},
		{"avc,placebo", std::string(CuUtil::String::Join(" ", Input, X264, PresetPlacebo, Output).data())},

		{"avc,720p,nvenc,colorspace=bt709",
		 std::string(CuUtil::String::Join(" ", InputCopy, Size720p, AvcAudioComp, X264Nvenc, Yuv420p, ColorSpaceBt709,
										  Output)
						 .data())},

		{"nv,avc", std::string(CuUtil::String::Join(" ", NvInput, X264Nvenc, Output).data())},
		{"nv,avc,ll", std::string(CuUtil::String::Join(" ", NvInput, NvencAvcLossLess, Output).data())},

		{"rp,avc", std::string(CuUtil::String::Join(" ", X264Mmal, Input, X264Omx, Output).data())},

		{"jpg", std::string(CuUtil::String::Join(" ", Input, OutputJpg).data())},
		{"png", std::string(CuUtil::String::Join(" ", Input, OutputPng).data())},

		{"png,resize100x100", std::string(CuUtil::String::Join(" ", Input, Size100X100, OutputPng).data())},

		{"vid,mp4", std::string(CuUtil::String::Join(" ", Input, OutputMp4).data())},

		{"vid2png%d", std::string(CuUtil::String::Join(" ", Input, Image2, OutputPng_d).data())},

		{"png%d2mp4,29.97fps,p10le",
		 std::string(CuUtil::String::Join(" ", FrameRate29_97, InputPng_d, Yuv420p10le, Output).data())},

		{"loudnorm", std::string(CuUtil::String::Join(" ", InputCopy, LoudNorm, Output).data())},

		{"i", std::string(CuUtil::String::Join(" ", Input).data())},

		{"easydecode",
		 std::string(CuUtil::String::Join(" ", Vsync0, HwCuvid, X264Cuvid, Resize720p, Input, X264Nvenc, Ac2,
										  OutputMp4)
						 .data())},

		{"arch,svtp10", std::string(CuUtil::String::Join(" ", Input, Opus, SvtAv1, Yuv420p10le, OutputMkv).data())},
		{"arch,vvc", std::string(CuUtil::String::Join(" ", Input, Opus, VvEnc, OutputMp4).data())},
		{"arch,vvc2", std::string(VidArchVvc_2pass.data())},
		{"avif", std::string(CuUtil::String::Join(" ", Input, OutputAvif).data())},
		{"opus", std::string(CuUtil::String::Join(" ", Input, Opus, OutputOpus).data())}
	};
}
