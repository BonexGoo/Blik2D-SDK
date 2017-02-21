#include <blik.h>
#if BLIK_NEED_ADDON_AAC

// 도구준비
#include <addon/blik_fakewin.h>
#include "blik_integration_fdk-aac-0.1.4.h"

#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/aacenc.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/aacenc_hcr.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/aacenc_lib.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/aacenc_pns.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/aacEnc_ram.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/aacEnc_rom.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/aacenc_tns.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/adj_thr.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/bandwidth.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/band_nrg.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/bitenc.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/bit_cnt.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/block_switch.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/channel_map.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/chaosmeasure.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/dyn_bits.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/grp_data.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/intensity.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/line_pe.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/metadata_compressor.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/metadata_main.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/ms_stereo.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/noisedet.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/pnsparam.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/pre_echo_control.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/psy_configuration.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/psy_main.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/qc_main.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/quantize.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/sf_estim.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/spreading.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/tonality.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libAACenc/src/transform.cpp>

#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/autocorr2nd.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/dct.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/FDK_bitbuffer.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/FDK_core.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/FDK_crc.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/FDK_hybrid.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/FDK_tools_rom.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/FDK_trigFcts.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/fft.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/fft_rad2.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/fixpoint_math.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/mdct.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/qmf.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libFDK/src/scale.cpp>

#include <addon/fdk-aac-0.1.4_for_blik/libSYS/src/cmdl_parser.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSYS/src/conv_string.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSYS/src/genericStds.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSYS/src/wav_file.cpp>

#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/bit_sbr.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/code_env.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/env_bit.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/env_est.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/fram_gen.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/invf_est.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/mh_det.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/nf_est.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/ps_bitenc.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/ps_encode.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/ps_main.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/resampler.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/sbrenc_freq_sca.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/sbr_encoder.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/sbr_misc.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/sbr_ram.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/sbr_rom.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/ton_corr.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libSBRenc/src/tran_det.cpp>

#include <addon/fdk-aac-0.1.4_for_blik/libMpegTPEnc/src/tpenc_adif.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libMpegTPEnc/src/tpenc_adts.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libMpegTPEnc/src/tpenc_asc.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libMpegTPEnc/src/tpenc_latm.cpp>
#include <addon/fdk-aac-0.1.4_for_blik/libMpegTPEnc/src/tpenc_lib.cpp>

#endif
