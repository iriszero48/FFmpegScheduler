#pragma once

#include "String.h"
#include "Combine.h"
#include "Unified.h"

const SuffixString Vsync0 = "-vsync 0";

const SuffixString HwCuvid = "-hwaccel cuvid";

const SuffixString X264Cuvid = "-c:v h264_cuvid";
const SuffixString X264Mmal = "-c:v h264_mmal";

const SuffixString FrameRate29_97 = "-framerate 29.97";

const SuffixString Input = Combine("-i ", DoubleQuotes("$$$input$$$"));
const SuffixString InputPng_d = Combine("-i ", DoubleQuotes("$$$input$$$" PathSeparator "%d.png"));

const SuffixString NvInput = Combine(HwCuvid, X264Cuvid, Input);

const SuffixString copy = "-c copy";
const SuffixString X264 = "-c:v libx264";
const SuffixString X265 = "-c:v libx265";
const SuffixString X264Nvenc = "-c:v h264_nvenc";
const SuffixString X264Omx = "-c:v h264_omx";
const SuffixString Aac = "-c:a aac";

const SuffixString InputCopy = Combine(Input, copy);

const SuffixString LoudNorm = "-filter:a loudnorm";
const SuffixString ScaleUp60Fps = R"(-filter:v "minterpolate='fps=60:mi_mode=mci:mc_mode=aobmc:me_mode=bidir:me=epzs:vsbmc=1:scd=fdiff'")";
const SuffixString Image2 = "-f image2";

const SuffixString Size100X100 = "-s 100x100";
const SuffixString Size720p = "-s 1280x720";

const SuffixString Resize720p = "-resize 1280x720";

const SuffixString Ac2 = "-ac 2";

const SuffixString PresetLossLessHp = "-preset losslesshp";
const SuffixString PresetUltraFast = "-preset ultrafast";
const SuffixString PresetSlower = "-preset slower";
const SuffixString PresetVerySlow = "-preset veryslow";
const SuffixString PresetPlacebo = "-preset placebo";

const SuffixString TuneFilm = "-tune film";

const SuffixString AudioBitrate128k = "-b:a 128k";

const SuffixString AudioS16 = "-sample_fmt s16";

const SuffixString Qp0 = "-qp 0";

const SuffixString Crf14 = "-crf 14";
const SuffixString Crf15 = "-crf 15";
const SuffixString Crf17 = "-crf 17";
const SuffixString Crf19 = "-crf 17";

const SuffixString Yuv420p = "-pix_fmt yuv420p";
const SuffixString Yuv420p10le = "-pix_fmt yuv420p10le";
const SuffixString Yuv444p10le = "-pix_fmt yuv444p10le";

const SuffixString ColorSpaceBt709 = "-colorspace 1";
const SuffixString Bt709 = Combine(ColorSpaceBt709, "-color_primaries 1 -color_trc 1");
const SuffixString Smpte170m = "-colorspace 6 -color_trc 6 -color_primaries 6";

const SuffixString Anima60FpsAvc720pParams = R"(-x264-params "deblock='0:0':keyint=600:min-keyint=1:ref=9:qcomp=0.7:rc-lookahead=180:aq-strength=0.9:merange=16:me=tesa:psy-rd='0:0.20':no-fast-pskip=1")";
const SuffixString Anima60FpsAvcParams = R"(-x264-params "mbtree=1:aq-mode=3:psy-rd='0.6:0.15':aq-strength=0.8:rc-lookahead=180:qcomp=0.75:deblock='-1:-1':keyint=600:min-keyint=1:bframes=8:ref=13:me=tesa:no-fast-pskip=1")";
const SuffixString Anima60FpsHevcParams = R"(-x265-params "deblock='-1:-1':ctu=32:qg-size=8:pbratio=1.2:cbqpoffs=-2:crqpoffs=-2:no-sao=1:me=3:subme=5:merange=38:b-intra=1:limit-tu=4:no-amp=1:ref=4:weightb=1:keyint=600:min-keyint=1:bframes=6:aq-mode=1:aq-strength=0.8:rd=5:psy-rd=2.0:psy-rdoq=1.0:rdoq-level=2:no-open-gop=1:rc-lookahead=180:scenecut=40:qcomp=0.65:no-strong-intra-smoothing=1:")";

const SuffixString AvcAudioComp = Combine(Aac, AudioBitrate128k/*, AudioS16*/);

const SuffixString AvcLossLess = Combine(X264, PresetUltraFast, Qp0);
const SuffixString AvcLossLessP10 = Combine(AvcLossLess, Yuv420p10le);
const SuffixString AvcVisuallyLossLess = Combine(X264, Crf17);
const SuffixString AvcVisuallyLossLessP10 = Combine(AvcVisuallyLossLess, Yuv420p10le);
const SuffixString NvencAvcLossLess = Combine(X264Nvenc, PresetLossLessHp, Qp0);
const SuffixString NvencAvc720pComp = Combine(X264Nvenc, PresetLossLessHp, Qp0);
const SuffixString AnimaAvc720pComp = Combine(X264, PresetVerySlow, Crf19, Yuv420p, TuneFilm, Anima60FpsAvc720pParams);
const SuffixString AnimaAvcComp = Combine(X264, PresetVerySlow, Crf15, Yuv420p10le, Anima60FpsAvcParams);
const SuffixString AnimaAvcCompYuv444 = Combine(X264, PresetVerySlow, Crf15, Yuv444p10le, Anima60FpsAvcParams);
const SuffixString AnimaHevcComp = Combine(X265, PresetSlower, Crf14, Yuv420p10le, Anima60FpsHevcParams);

const auto Output = DoubleQuotes("$$$output$$$");
const auto OutputJpg = DoubleQuotes("$$$output$$$.jpg");
const auto OutputPng = DoubleQuotes("$$$output$$$.png");
const auto OutputPng_d = DoubleQuotes("$$$input$$$" PathSeparator "%d.png");
const auto OutputMp4 = DoubleQuotes("$$$output$$$.mp4");