/***************************************************************************
*                                                                          *
* Project64-video - A Nintendo 64 gfx plugin.                              *
* http://www.pj64-emu.com/                                                 *
* Copyright (C) 2017 Project64. All rights reserved.                       *
* Copyright (C) 2003-2009  Sergey 'Gonetz' Lipski                          *
* Copyright (C) 2002 Dave2001                                              *
*                                                                          *
* License:                                                                 *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                       *
* version 2 of the License, or (at your option) any later version.         *
*                                                                          *
****************************************************************************/
#pragma once
#include <Project64-video/Renderer/types.h>
#include <glide.h>
#include "../GlideExtensions.h"

void gfxClipWindow(FxU32 minx, FxU32 miny, FxU32 maxx, FxU32 maxy);
void gfxColorMask(FxBool rgb, FxBool a);
FxU32 gfxTexMinAddress(GrChipID_t tmu);
FxBool gfxSstWinClose(GrContext_t context);
void gfxTextureBufferExt(GrChipID_t tmu, FxU32 startAddress, GrLOD_t lodmin, GrLOD_t lodmax, GrAspectRatio_t aspect, GrTextureFormat_t fmt, FxU32 evenOdd);
FxU32 gfxTexMaxAddress(GrChipID_t tmu);
FxU32 gfxTexTextureMemRequired(FxU32 evenOdd, GrTexInfo *info);
void gfxConstantColorValue(GrColor_t value);
void gfxColorCombine(GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert);
void gfxAlphaCombine(GrCombineFunction_t function, GrCombineFactor_t factor, GrCombineLocal_t local, GrCombineOther_t other, FxBool invert);
void gfxTexCombine(GrChipID_t tmu, GrCombineFunction_t rgb_function, GrCombineFactor_t rgb_factor, GrCombineFunction_t alpha_function, GrCombineFactor_t alpha_factor, FxBool rgb_invert, FxBool alpha_invert);
void gfxAlphaBlendFunction(GrAlphaBlendFnc_t rgb_sf, GrAlphaBlendFnc_t rgb_df, GrAlphaBlendFnc_t alpha_sf, GrAlphaBlendFnc_t alpha_df);
void gfxAlphaTestReferenceValue(gfxAlpha_t value);
void gfxAlphaTestFunction(GrCmpFnc_t function);
void gfxFogMode(GrFogMode_t mode);
void gfxFogGenerateLinear(float nearZ, float farZ);
void gfxFogColorValue(GrColor_t fogcolor);
void gfxChromakeyMode(GrChromakeyMode_t mode);
void gfxChromakeyValue(GrColor_t value);
void gfxStippleMode(GrStippleMode_t mode);
void gfxColorCombineExt(GrCCUColor_t a, GrCombineMode_t a_mode, GrCCUColor_t b, GrCombineMode_t b_mode, GrCCUColor_t c, FxBool c_invert, GrCCUColor_t d, FxBool d_invert, FxU32 shift, FxBool invert);
void gfxAlphaCombineExt(GrACUColor_t a, GrCombineMode_t a_mode, GrACUColor_t b, GrCombineMode_t b_mode, GrACUColor_t c, FxBool c_invert, GrACUColor_t d, FxBool d_invert, FxU32 shift, FxBool invert);
void gfxTexColorCombineExt(GrChipID_t tmu, GrTCCUColor_t a, GrCombineMode_t a_mode, GrTCCUColor_t b, GrCombineMode_t b_mode, GrTCCUColor_t c, FxBool c_invert, GrTCCUColor_t d, FxBool d_invert, FxU32 shift, FxBool invert);
void gfxTexAlphaCombineExt(GrChipID_t tmu, GrTACUColor_t a, GrCombineMode_t a_mode, GrTACUColor_t b, GrCombineMode_t b_mode, GrTACUColor_t c, FxBool c_invert, GrTACUColor_t d, FxBool d_invert, FxU32 shift, FxBool invert);
void gfxConstantColorValueExt(GrChipID_t tmu, GrColor_t value);
void gfxVertexLayout(FxU32 param, FxI32 offset, FxU32 mode);
void gfxCullMode(GrCullMode_t mode);
void gfxDepthBufferMode(GrDepthBufferMode_t mode);
void gfxDepthBufferFunction(GrCmpFnc_t function);
void gfxDepthMask(FxBool mask);
void gfxDrawTriangle(const void *a, const void *b, const void *c);
void gfxDepthBiasLevel(FxI32 level);
void gfxDrawLine(const void *a, const void *b);
void gfxDrawVertexArray(FxU32 mode, FxU32 Count, void *pointers2);
void gfxDrawVertexArrayContiguous(FxU32 mode, FxU32 Count, void *pointers, FxU32 stride);

GrContext_t gfxSstWinOpen(GrColorFormat_t color_format, GrOriginLocation_t origin_location, int nColBuffers, int nAuxBuffers);
void gfxAuxBufferExt(GrBuffer_t buffer);
FxU32 gfxGet(FxU32 pname, FxU32 plength, FxI32 *params);
void gfxFramebufferCopyExt(int x, int y, int w, int h, int from, int to, int mode);
void gfxRenderBuffer(GrBuffer_t buffer);
void gfxBufferClear(GrColor_t color, gfxAlpha_t alpha, FxU32 depth);
void gfxBufferSwap(FxU32 swap_interval);
FxBool gfxLfbLock(GrLock_t type, GrBuffer_t buffer, GrLfbWriteMode_t writeMode, GrOriginLocation_t origin, FxBool pixelPipeline, GrLfbInfo_t *info);
FxBool gfxLfbUnlock(GrLock_t type, GrBuffer_t buffer);
FxBool gfxLfbReadRegion(GrBuffer_t src_buffer, FxU32 src_x, FxU32 src_y, FxU32 src_width, FxU32 src_height, FxU32 dst_stride, void *dst_data);

extern uint32_t nbTextureUnits;
extern uint32_t g_scr_res_x, g_scr_res_y, g_res_x, g_res_y;
