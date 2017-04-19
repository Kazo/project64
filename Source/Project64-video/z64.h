/****************************************************************************
*                                                                           *
* Project64 - A Nintendo 64 emulator.                                       *
* http://www.pj64-emu.com/                                                  *
* Copyright (C) 2017 Project64. All rights reserved.                        *
*                                                                           *
* License:                                                                  *
* GNU/GPLv2 http://www.gnu.org/licenses/gpl-2.0.html                        *
*                                                                           *
****************************************************************************/
#pragma once

enum
{
    DPC_CLR_XBUS_DMEM_DMA = 0x0001,	/* Bit 0: clear xbus_dmem_dma */
    DPC_SET_XBUS_DMEM_DMA = 0x0002,	/* Bit 1: set xbus_dmem_dma */
    DPC_CLR_FREEZE = 0x0004,	/* Bit 2: clear freeze */
    DPC_SET_FREEZE = 0x0008,	/* Bit 3: set freeze */
    DPC_CLR_FLUSH = 0x0010,	/* Bit 4: clear flush */
    DPC_SET_FLUSH = 0x0020,	/* Bit 5: set flush */
    DPC_CLR_TMEM_CTR = 0x0040,	/* Bit 6: clear tmem ctr */
    DPC_CLR_PIPE_CTR = 0x0080,	/* Bit 7: clear pipe ctr */
    DPC_CLR_CMD_CTR = 0x0100,	/* Bit 8: clear cmd ctr */
    DPC_CLR_CLOCK_CTR = 0x0200,	/* Bit 9: clear clock ctr */

    DPC_STATUS_XBUS_DMEM_DMA = 0x001,	/* Bit  0: xbus_dmem_dma */
    DPC_STATUS_FREEZE = 0x002,	/* Bit  1: freeze */
    DPC_STATUS_FLUSH = 0x004,	/* Bit  2: flush */
    DPC_STATUS_START_GCLK = 0x008,	/* Bit  3: start gclk */
    DPC_STATUS_TMEM_BUSY = 0x010,	/* Bit  4: tmem busy */
    DPC_STATUS_PIPE_BUSY = 0x020,	/* Bit  5: pipe busy */
    DPC_STATUS_CMD_BUSY = 0x040,	/* Bit  6: cmd busy */
    DPC_STATUS_CBUF_READY = 0x080,	/* Bit  7: cbuf ready */
    DPC_STATUS_DMA_BUSY = 0x100,	/* Bit  8: dma busy */
    DPC_STATUS_END_VALID = 0x200,	/* Bit  9: end valid */
    DPC_STATUS_START_VALID = 0x400,	/* Bit 10: start valid */
};
