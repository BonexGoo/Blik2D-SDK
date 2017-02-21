#include <blik.h>
#if BLIK_NEED_ADDON_JPG

#include "blik_integration_opencv-3.1.0_3rdparty_libjpeg.h"

#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jaricom.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jcapimin.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jcapistd.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jcarith.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jccoefct.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jccolor.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jcdctmgr.c>
#define encode_mcu_DC_first encode_mcu_DC_first_jchuff_BLIK
#define encode_mcu_AC_first encode_mcu_AC_first_jchuff_BLIK
#define encode_mcu_DC_refine encode_mcu_DC_refine_jchuff_BLIK
#define encode_mcu_AC_refine encode_mcu_AC_refine_jchuff_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jchuff.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jcinit.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jcmainct.c>
#define emit_byte emit_byte_jcmarker_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jcmarker.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jcmaster.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jcomapi.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jcparam.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jcprepct.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jcsample.c>
#define my_coef_controller my_coef_controller_jctrans_BLIK
#define start_iMCU_row start_iMCU_row_jctrans_BLIK
#define start_pass_coef start_pass_coef_jctrans_BLIK
#define compress_output compress_output_jctrans_BLIK
#define my_coef_ptr my_coef_ptr_jctrans_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jctrans.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdapimin.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdapistd.c>
#define arith_entropy_ptr arith_entropy_ptr_jdarith_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdarith.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdatadst.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdatasrc.c>
#define my_coef_controller my_coef_controller_jdcoefct_BLIK
#define start_iMCU_row start_iMCU_row_jdcoefct_BLIK
#define my_coef_ptr my_coef_ptr_jdcoefct_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdcoefct.c>
#define my_cconvert_ptr my_cconvert_ptr_jdcolor_BLIK
#define rgb_gray_convert rgb_gray_convert_jdcolor_BLIK
#define rgb_convert rgb_convert_jdcolor_BLIK
#define null_convert null_convert_jdcolor_BLIK
#define grayscale_convert grayscale_convert_jdcolor_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdcolor.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jddctmgr.c>
#define savable_state savable_state_jdhuff_BLIK
#define huff_entropy_ptr huff_entropy_ptr_jdhuff_BLIK
#define insufficient_data insufficient_data_jdhuff_BLIK
#define process_restart process_restart_jdhuff_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdhuff.c>
#define initial_setup initial_setup_jdinput_BLIK
#define per_scan_setup per_scan_setup_jdinput_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdinput.c>
#define my_main_controller my_main_controller_jdmainct_BLIK
#define my_main_ptr my_main_ptr_jdmainct_BLIK
#define start_pass_main start_pass_main_jdmainct_BLIK
#define process_data_simple_main process_data_simple_main_jdmainct_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdmainct.c>
#define my_marker_ptr my_marker_ptr_jdmarker_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdmarker.c>
#define my_master_ptr my_master_ptr_jdmaster_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdmaster.c>
#define build_ycc_rgb_table build_ycc_rgb_table_jdmerge_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdmerge.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdpostct.c>
#define my_upsampler my_upsampler_jdsample_BLIK
#define my_upsample_ptr my_upsample_ptr_jdsample_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdsample.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jdtrans.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jerror.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jfdctflt.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jfdctfst.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jfdctint.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jidctflt.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jidctfst.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jidctint.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jmemansi.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jmemmgr.c>
#define jpeg_get_small jpeg_get_small_jmemnobs_BLIK
#define jpeg_free_small jpeg_free_small_jmemnobs_BLIK
#define jpeg_get_large jpeg_get_large_jmemnobs_BLIK
#define jpeg_free_large jpeg_free_large_jmemnobs_BLIK
#define jpeg_mem_available jpeg_mem_available_jmemnobs_BLIK
#define jpeg_open_backing_store jpeg_open_backing_store_jmemnobs_BLIK
#define jpeg_mem_init jpeg_mem_init_jmemnobs_BLIK
#define jpeg_mem_term jpeg_mem_term_jmemnobs_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jmemnobs.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jquant1.c>
#define my_cquantizer my_cquantizer_jquant2_BLIK
#define my_cquantize_ptr my_cquantize_ptr_jquant2_BLIK
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jquant2.c>
#include <addon/opencv-3.1.0_for_blik/3rdparty/libjpeg/jutils.c>

#endif
