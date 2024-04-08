#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "vg_lite.h"
#include "vg_lite_util.h"

#define DEFAULT_SIZE   400.0f;
#define __func__ __FUNCTION__
char *error_type[] = 
{
    "VG_LITE_SUCCESS",
    "VG_LITE_INVALID_ARGUMENT",
    "VG_LITE_OUT_OF_MEMORY",
    "VG_LITE_NO_CONTEXT",      
    "VG_LITE_TIMEOUT",
    "VG_LITE_OUT_OF_RESOURCES",
    "VG_LITE_GENERIC_IO",
    "VG_LITE_NOT_SUPPORT",
};
#define IS_ERROR(status)         (status > 0)
#define CHECK_ERROR(Function) \
    error = Function; \
    if (IS_ERROR(error)) \
    { \
        printf("[%s: %d] failed.error type is %s\n", __func__, __LINE__,error_type[error]);\
        goto ErrorHandler; \
    }
static int   fb_width = 400, fb_height = 400;
static float fb_scale = 1.0f;

static vg_lite_buffer_t buffer;     //offscreen framebuffer object for rendering.
static vg_lite_buffer_t * sys_fb;   //system framebuffer object to show the rendering result.
static vg_lite_buffer_t * fb;

vg_lite_matrix_t matrix;

static vg_lite_buffer_t offscreenBuf;

static int test_raster_stride;

#define ALIGMENT(value,base)   ((value + base - 1) & ~(base-1))

#define TEST_RASTER_WIDTH               100 /*pixels*/
#define TEST_RASTER_HEIGHT              100 /*pixels*/
#define TEST_RASTER_ALIGMENT            16 /*pixels*/
#define TEST_RASTER_COLOUR_CHANNELS     1

#define TEST_RASTER_OFFSET              25
#define TEST_RASTER_BUF_SIZE            (TEST_RASTER_WIDTH * \
                                         TEST_RASTER_HEIGHT * \
                                         TEST_RASTER_COLOUR_CHANNELS)
#define TEST_RASTER_BUF_SIZEW           (TEST_RASTER_BUF_SIZE >> 2)

static uint32_t test_a8_raster[TEST_RASTER_BUF_SIZEW] = {
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000, /* 1 */
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000, /* 10 */

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x825B340D,0xDFD2C5A9,0xECF9F9EC,0xA9C5D2DF,0x0D345B82,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0xCF8E4C0E,
  0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFEFFFFFF,
  0x0E4C8ECF,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000, /* 20 */

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0xC1640F00,0xFFFFFFFC,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0xFCFFFFFF,0x000F64C1,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x4A010000,0xFFFFF7A9,0xFFFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xA9F7FFFF,0x0000014A,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000, /* 30 */

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFD45A03,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0x035AD4FF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x63030000,0xFFFFFFDB,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xDBFFFFFF,0x00000363,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000, /* 40 */

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0xFFD03A00,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0x8DB4DFFF,0x202D4066,0x13060613,0x66402D20,0xFFDFB48D,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x003AD0FF,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x14000000,0xFFFFFE9D,0xFFFFFFFF,0xFFFFFFFF,0x60A2E5FF,
  0x0000001E,0x00000000,0x00000000,0x00000000,0x1E000000,
  0xFFE5A260,0xFFFFFFFF,0xFFFFFFFF,0x9DFEFFFF,0x00000014,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000, /* 50 */

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0xED4F0000,0xFFFFFFFF,0xFFFFFFFF,0x9FF2FFFF,0x00000142,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x42010000,0xFFFFF29F,0xFFFFFFFF,0xFFFFFFFF,0x00004FEC,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0xFFFF8E05,0xFFFFFFFF,0xFFFFFFFF,0x000A61D8,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0xD8610A00,0xFFFFFFFF,0xFFFFFFFF,0x058EFFFF,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000, /* 60 */

  0x00000000,0x00000000,0x00000000,0x00000000,0x20000000,
  0xFFFFFFC9,0xFFFFFFFF,0x56D0FFFF,0x00000002,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x02000000,0xFFFFD056,0xFFFFFFFF,0xC9FFFFFF,
  0x00000020,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0xEB350000,
  0xFFFFFFFF,0xFFFFFFFF,0x00005CE8,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0xE85C0100,0xFFFFFFFF,0xFFFFFFFF,
  0x000035EB,0x00000000,0x00000000,0x00000000,0x00000000, /* 70 */

  0x00000000,0x00000000,0x00000000,0x00000000,0xFFF34600,
  0xFFFFFFFF,0x93FCFFFF,0x0000000F,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0F000000,0xFFFFFC93,0xFFFFFFFF,
  0x0046F3FF,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0xFFFFFA59,
  0xFFFFFFFF,0x0036DEFF,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0xFFDE3600,0xFFFFFFFF,
  0x59FAFFFF,0x00000000,0x00000000,0x00000000,0x00000000, /* 80 */

  0x00000000,0x00000000,0x00000000,0x59000000,0xFFFFFFFE,
  0xFFFFFFFF,0x000010AD,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0xAD100000,0xFFFFFFFF,
  0xFEFFFFFF,0x00000059,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0xFA460000,0xFFFFFFFF,
  0x6FFCFFFF,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0xFFFFFC6F,
  0xFFFFFFFF,0x000046FA,0x00000000,0x00000000,0x00000000, /* 90 */

  0x00000000,0x00000000,0x00000000,0xFFF33500,0xFFFFFFFF,
  0x0050F7FF,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0xFFF75000,
  0xFFFFFFFF,0x0035F3FF,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0xFFFFEB20,0xFFFFFFFF,
  0x00003EF0,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0xF03E0000,
  0xFFFFFFFF,0x20EBFFFF,0x00000000,0x00000000,0x00000000, /* 100 */

  0x00000000,0x00000000,0x05000000,0xFFFFFFC9,0xF0FFFFFF,
  0x00000030,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x30000000,
  0xFFFFFFF0,0xC9FFFFFF,0x00000005,0x00000000,0x00000000,

  0x00000000,0x00000000,0x8F000000,0xFFFFFFFF,0x3EF7FFFF,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0xFFFFF73E,0xFFFFFFFF,0x0000008F,0x00000000,0x00000000, /* 110 */

  0x00000000,0x00000000,0xFF4F0000,0xFFFFFFFF,0x0050FCFF,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0xFFFC5000,0xFFFFFFFF,0x00004FFF,0x00000000,0x00000000,

  0x00000000,0x00000000,0xFFED1400,0xFFFFFFFF,0x00006FFF,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0xFF6F0000,0xFFFFFFFF,0x0014ECFF,0x00000000,0x00000000, /* 120 */

  0x00000000,0x00000000,0xFFFF9D00,0xFFFFFFFF,0x000000AD,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0xAD000000,0xFFFFFFFF,0x009DFFFF,0x00000000,0x00000000,

  0x00000000,0x00000000,0xFFFFFE3A,0xDEFFFFFF,0x00000010,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x10000000,0xFFFFFFDE,0x3AFEFFFF,0x00000000,0x00000000, /* 130 */

  0x00000000,0x03000000,0xFFFFFFD0,0x36FCFFFF,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0xFFFFFC36,0xD0FFFFFF,0x00000003,0x00000000,

  0x00000000,0x63000000,0xFFFFFFFF,0x0093FFFF,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0xFFFF9300,0xFFFFFFFF,0x00000063,0x00000000, /* 140 */

  0x00000000,0xDB030000,0xFFFFFFFF,0x000FE8FF,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0xFFE80F00,0xFFFFFFFF,0x000003DB,0x00000000,

  0x00000000,0xFF5A0000,0xFFFFFFFF,0x00005CFF,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0xFF5C0000,0xFFFFFFFF,0x00005AFF,0x00000000, /* 150 */

  0x00000000,0xFFD40100,0xFFFFFFFF,0x000000D0,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0xD0000000,0xFFFFFFFF,0x0001D4FF,0x00000000,

  0x00000000,0xFFFF4A00,0xFFFFFFFF,0x00000056,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x56000000,0xFFFFFFFF,0x004AFFFF,0x00000000, /* 160 */

  0x00000000,0xFFFFA900,0xD8FFFFFF,0x00000002,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x02000000,0xFFFFFFD8,0x00A9FFFF,0x00000000,

  0x00000000,0xFFFFF70F,0x61FFFFFF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFFFFF61,0x0FF7FFFF,0x00000000, /* 170 */

  0x00000000,0xFFFFFF64,0x0AF2FFFF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFFFF20A,0x64FFFFFF,0x00000000,

  0x00000000,0xFFFFFFC1,0x009FFFFF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFFF9F00,0xC1FFFFFF,0x00000000, /* 180 */

  0x0E000000,0xFFFFFFFC,0x0042FFFF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFFF4200,0xFCFFFFFF,0x0000000E,

  0x4C000000,0xFFFFFFFF,0x0001E5FF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFE50200,0xFFFFFFFF,0x0000004C, /* 190 */

  0x8E000000,0xFFFFFFFF,0x0000A2FF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFA20000,0xFFFFFFFF,0x0000008E,

  0xD0000000,0xFFFFFFFF,0x000060FF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFF600000,0xFFFFFFFF,0x000000CF, /* 200 */

  0xFE0D0000,0xFFFFFFFF,0x00001EFF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFF1E0000,0xFFFFFFFF,0x00000DFE,

  0xFF340000,0xFFFFFFFF,0x000000DF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xDF000000,0xFFFFFFFF,0x000034FF, /* 210 */

  0xFF5B0000,0xFFFFFFFF,0x000000B4,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xB4000000,0xFFFFFFFF,0x00005BFF,

  0xFF820000,0xFFFFFFFF,0x0000008D,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x8D000000,0xFFFFFFFF,0x000082FF, /* 220 */

  0xFFA90000,0xFFFFFFFF,0x00000066,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x66000000,0xFFFFFFFF,0x0000A9FF,

  0xFFC60000,0xFFFFFFFF,0x00000040,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x40000000,0xFFFFFFFF,0x0000C5FF, /* 230 */

  0xFFD20000,0xFFFFFFFF,0x0000002D,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x2D000000,0xFFFFFFFF,0x0000D2FF,

  0xFFDF0000,0xFFFFFFFF,0x00000020,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x20000000,0xFFFFFFFF,0x0000DFFF, /* 240 */

  0xFFEC0000,0xFFFFFFFF,0x00000013,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x13000000,0xFFFFFFFF,0x0000ECFF,

  0xFFF90000,0xFFFFFFFF,0x00000006,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x06000000,0xFFFFFFFF,0x0000F9FF, /* 250 */

  0xFFF90000,0xFFFFFFFF,0x00000006,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x06000000,0xFFFFFFFF,0x0000F9FF,

  0xFFEC0000,0xFFFFFFFF,0x00000013,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x13000000,0xFFFFFFFF,0x0000ECFF, /* 260 */

  0xFFDF0000,0xFFFFFFFF,0x00000020,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x20000000,0xFFFFFFFF,0x0000DFFF,

  0xFFD20000,0xFFFFFFFF,0x0000002D,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x2D000000,0xFFFFFFFF,0x0000D2FF, /* 270 */

  0xFFC50000,0xFFFFFFFF,0x00000040,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x40000000,0xFFFFFFFF,0x0000C5FF,

  0xFFA90000,0xFFFFFFFF,0x00000066,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x66000000,0xFFFFFFFF,0x0000A9FF, /* 280 */

  0xFF820000,0xFFFFFFFF,0x0000008D,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x8D000000,0xFFFFFFFF,0x000082FF,

  0xFF5B0000,0xFFFFFFFF,0x000000B4,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xB4000000,0xFFFFFFFF,0x00005BFF, /* 290 */

  0xFF340000,0xFFFFFFFF,0x000000DF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xDF000000,0xFFFFFFFF,0x000034FF,

  0xFE0D0000,0xFFFFFFFF,0x00001EFF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFF1E0000,0xFFFFFFFF,0x00000DFE, /* 300 */

  0xCF000000,0xFFFFFFFF,0x000060FF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFF600000,0xFFFFFFFF,0x000000CF,

  0x8E000000,0xFFFFFFFF,0x0000A2FF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFA20000,0xFFFFFFFF,0x0000008E, /* 310 */

  0x4C000000,0xFFFFFFFF,0x0001E5FF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFE50100,0xFFFFFFFF,0x0000004C,

  0x0E000000,0xFFFFFFFC,0x0042FFFF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFFF4200,0xFCFFFFFF,0x0000000E, /* 320 */

  0x00000000,0xFFFFFFC1,0x009FFFFF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFFF9F00,0xC1FFFFFF,0x00000000,

  0x00000000,0xFFFFFF64,0x0AF2FFFF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFFFF20A,0x64FFFFFF,0x00000000, /* 330 */

  0x00000000,0xFFFFF70F,0x61FFFFFF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFFFFF61,0x0FF7FFFF,0x00000000,

  0x00000000,0xFFFFA900,0xD8FFFFFF,0x00000002,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x02000000,0xFFFFFFD8,0x00A9FFFF,0x00000000, /* 340 */

  0x00000000,0xFFFF4A00,0xFFFFFFFF,0x00000056,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x56000000,0xFFFFFFFF,0x004AFFFF,0x00000000,

  0x00000000,0xFFD40100,0xFFFFFFFF,0x000000D0,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0xD0000000,0xFFFFFFFF,0x0001D4FF,0x00000000, /* 350 */

  0x00000000,0xFF5A0000,0xFFFFFFFF,0x00005CFF,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0xFF5C0000,0xFFFFFFFF,0x00005AFF,0x00000000,

  0x00000000,0xDB030000,0xFFFFFFFF,0x000FE8FF,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0xFFE80F00,0xFFFFFFFF,0x000003DB,0x00000000, /* 360 */

  0x00000000,0x63000000,0xFFFFFFFF,0x0093FFFF,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0xFFFF9300,0xFFFFFFFF,0x00000063,0x00000000,

  0x00000000,0x03000000,0xFFFFFFD0,0x36FCFFFF,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0xFFFFFC36,0xD0FFFFFF,0x00000003,0x00000000, /* 370 */

  0x00000000,0x00000000,0xFFFFFE3A,0xDEFFFFFF,0x00000010,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x10000000,0xFFFFFFDE,0x3AFEFFFF,0x00000000,0x00000000,

  0x00000000,0x00000000,0xFFFF9D00,0xFFFFFFFF,0x000000AD,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0xAE000000,0xFFFFFFFF,0x009DFFFF,0x00000000,0x00000000, /* 380 */

  0x00000000,0x00000000,0xFFEC1400,0xFFFFFFFF,0x00006FFF,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0xFF6F0000,0xFFFFFFFF,0x0014ECFF,0x00000000,0x00000000,

  0x00000000,0x00000000,0xFF4F0000,0xFFFFFFFF,0x0050FCFF,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0xFFFC5000,0xFFFFFFFF,0x00004FFF,0x00000000,0x00000000, /* 390 */

  0x00000000,0x00000000,0x8F000000,0xFFFFFFFF,0x3EF7FFFF,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0xFFFFF73E,0xFFFFFFFF,0x0000008E,0x00000000,0x00000000,

  0x00000000,0x00000000,0x05000000,0xFFFFFFC9,0xF0FFFFFF,
  0x00000030,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x30000000,
  0xFFFFFFF0,0xC9FFFFFF,0x00000005,0x00000000,0x00000000, /* 400 */

  0x00000000,0x00000000,0x00000000,0xFFFFEB20,0xFFFFFFFF,
  0x00003EF0,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0xF03E0000,
  0xFFFFFFFF,0x20EBFFFF,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0xFFF33500,0xFFFFFFFF,
  0x0050F7FF,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0000FF00,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0xFFF75000,
  0xFFFFFFFF,0x0035F3FF,0x00000000,0x00000000,0x00000000, /* 410 */

  0x00000000,0x00000000,0x00000000,0xFA460000,0xFFFFFFFF,
  0x6FFCFFFF,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0xFFFFFC6F,
  0xFFFFFFFF,0x000046FA,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x59000000,0xFFFFFFFE,
  0xFFFFFFFF,0x000010AD,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0xAD100000,0xFFFFFFFF,
  0xFEFFFFFF,0x00000059,0x00000000,0x00000000,0x00000000, /* 420 */

  0x00000000,0x00000000,0x00000000,0x00000000,0xFFFFFA59,
  0xFFFFFFFF,0x0036DEFF,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0xFFDE3600,0xFFFFFFFF,
  0x59FAFFFF,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0xFFF34600,
  0xFFFFFFFF,0x93FCFFFF,0x0000000F,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x0F000000,0xFFFFFC93,0xFFFFFFFF,
  0x0046F3FF,0x00000000,0x00000000,0x00000000,0x00000000, /* 430 */

  0x00000000,0x00000000,0x00000000,0x00000000,0xEB350000,
  0xFFFFFFFF,0xFFFFFFFF,0x00005CE8,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xE85C0000,0xFFFFFFFF,0xFFFFFFFF,
  0x000035EB,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0x20000000,
  0xFFFFFFC9,0xFFFFFFFF,0x56D0FFFF,0x00000002,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x02000000,0xFFFFD056,0xFFFFFFFF,0xC9FFFFFF,
  0x00000020,0x00000000,0x00000000,0x00000000,0x00000000, /* 440 */

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0xFFFF8F05,0xFFFFFFFF,0xFFFFFFFF,0x000A61D8,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0xD8610A00,0xFFFFFFFF,0xFFFFFFFF,0x058FFFFF,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0xED4F0000,0xFFFFFFFF,0xFFFFFFFF,0x9FF2FFFF,0x00000142,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x42010000,0xFFFFF29F,0xFFFFFFFF,0xFFFFFFFF,0x00004FEC,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000, /* 450 */

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x14000000,0xFFFFFE9D,0xFFFFFFFF,0xFFFFFFFF,0x60A2E5FF,
  0x0000001E,0x00000000,0x00000000,0x00000000,0x1E000000,
  0xFFE5A260,0xFFFFFFFF,0xFFFFFFFF,0x9DFEFFFF,0x00000014,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0xFFD03A00,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0x8DB4DFFF,0x202D4066,0x13060613,0x66402D20,0xFFDFB48D,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x003AD0FF,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000, /* 460 */

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x63030000,0xFFFFFFDB,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xDBFFFFFF,0x00000363,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0xFFD45A03,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0x035AD4FF,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000, /* 470 */

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x4A010000,0xFFFFF7A9,0xFFFFFFFF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xA9F7FFFF,0x0000014A,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0xC1640F00,0xFFFFFFFC,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0xFCFFFFFF,0x000F64C1,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000, /* 480 */

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0xD08E4C0E,
  0xFFFFFFFE,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFEFFFFFF,
  0x0E4C8ED0,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x825B340D,0xDFD2C5A9,0xECF9F9EC,0xA9C5D2DF,0x0D345B82,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000, /* 490 */

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,

  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,
  0x00000000,0x00000000,0x00000000,0x00000000,0x00000000 /* 500 */
};

void cleanup(void)
{
    if (buffer.handle != NULL) {
        // Free the buffer memory.
        vg_lite_free(&buffer);
    }

    vg_lite_close();
}

static vg_lite_error_t init_offscreenBuf()
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_buffer_t *vg_buffer  = &offscreenBuf;
    vg_buffer->width     = test_raster_stride;
    vg_buffer->height    = TEST_RASTER_HEIGHT;
    vg_buffer->format    = VG_LITE_A8;
    vg_buffer->image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
    vg_buffer->transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
    CHECK_ERROR(vg_lite_allocate(vg_buffer));

ErrorHandler:
    return error;
}

static vg_lite_error_t init_vg_lite(void)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    // Initialize the draw.
    CHECK_ERROR(vg_lite_init(128, 128));

    test_raster_stride = ALIGMENT(TEST_RASTER_WIDTH, TEST_RASTER_ALIGMENT);
    init_offscreenBuf();

ErrorHandler:
    return error;
}

static vg_lite_error_t render()
{
    uint32_t j;
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t feature_check = 1;
    vg_lite_blend_t blend[4] = {VG_LITE_BLEND_SUBTRACT_LVGL, VG_LITE_BLEND_NORMAL_LVGL,
                                    VG_LITE_BLEND_ADDITIVE_LVGL, VG_LITE_BLEND_MULTIPLY_LVGL};

    const char *names[4] = {"BLEND_SUBTRACT_LVGL.png", "BLEND_NORMAL_LVGL.png", 
                            "BLEND_ADDITIVE_LVGL.png","BLEND_MULTIPLY_LVGL.png"};

    feature_check = vg_lite_query_feature(gcFEATURE_BIT_VG_LVGL_SUPPORT);
    if (!feature_check) {
        printf("lvgl is not supported.\n");
        cleanup();
        return error;
    }

    for (j = 0; j < 4; j++)
    {
        unsigned char * ptr;
        int i;

        buffer.format = VG_LITE_RGB565; 
        buffer.width = ALIGMENT(fb_width,64);
        buffer.height = fb_height;
        ptr = offscreenBuf.memory;

        //Setup A8 surface
        memset(offscreenBuf.memory,0,test_raster_stride*TEST_RASTER_HEIGHT);
        for(i = 0; i < TEST_RASTER_HEIGHT; i++)
        {
            memcpy(ptr,&test_a8_raster[i*TEST_RASTER_OFFSET], TEST_RASTER_HEIGHT);
            ptr += test_raster_stride;
        }
        CHECK_ERROR(vg_lite_allocate(&buffer));
        //Clear surface with red
        CHECK_ERROR(vg_lite_clear( &buffer, NULL, 0xFF0000FF));
        CHECK_ERROR(vg_lite_finish());
        vg_lite_identity(&matrix);
        vg_lite_scale(4, 4, &matrix);
        vg_lite_translate(0, 0, &matrix);

        CHECK_ERROR(vg_lite_blit( &buffer, &offscreenBuf, &matrix, blend[j], 0xFF00FF00, VG_LITE_FILTER_POINT));
        CHECK_ERROR(vg_lite_finish());

        // Save PNG file.
        vg_lite_save_png(names[j], &buffer);
    }
    
ErrorHandler:
    return error;
}

int main(int argc, const char * argv[])
{
    vg_lite_error_t error;

    CHECK_ERROR(init_vg_lite());

    CHECK_ERROR(render());

ErrorHandler:
    cleanup();
    return 0;
}