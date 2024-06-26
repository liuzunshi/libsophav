//-----------------------------------------------------------------------------
// COPYRIGHT (C) 2020   CHIPS&MEDIA INC. ALL RIGHTS RESERVED
//
// This file is distributed under BSD 3 clause and LGPL2.1 (dual license)
// SPDX License Identifier: BSD-3-Clause
// SPDX License Identifier: LGPL-2.1-only
//
// The entire notice above must be reproduced on all authorized copies.
//
// Description  :
//-----------------------------------------------------------------------------

#ifndef __VPU_DRV_H__
#define __VPU_DRV_H__

#include <linux/fs.h>
#include <linux/types.h>
#include "../../../vpuapi/vpuconfig.h"


#define VDI_IOCTL_MAGIC  'V'
#define VDI_IOCTL_ALLOCATE_PHYSICAL_MEMORY	_IO(VDI_IOCTL_MAGIC, 0)
#define VDI_IOCTL_FREE_PHYSICALMEMORY		_IO(VDI_IOCTL_MAGIC, 1)
#define VDI_IOCTL_WAIT_INTERRUPT			_IO(VDI_IOCTL_MAGIC, 2)
#define VDI_IOCTL_SET_CLOCK_GATE			_IO(VDI_IOCTL_MAGIC, 3)
#define VDI_IOCTL_RESET						_IO(VDI_IOCTL_MAGIC, 4)
#define VDI_IOCTL_GET_INSTANCE_POOL			_IO(VDI_IOCTL_MAGIC, 5)
#define VDI_IOCTL_GET_COMMON_MEMORY			_IO(VDI_IOCTL_MAGIC, 6)
#define VDI_IOCTL_GET_RESERVED_VIDEO_MEMORY_INFO _IO(VDI_IOCTL_MAGIC, 8)
#define VDI_IOCTL_OPEN_INSTANCE				_IO(VDI_IOCTL_MAGIC, 9)
#define VDI_IOCTL_CLOSE_INSTANCE			_IO(VDI_IOCTL_MAGIC, 10)
#define VDI_IOCTL_GET_INSTANCE_NUM			_IO(VDI_IOCTL_MAGIC, 11)
#define VDI_IOCTL_GET_REGISTER_INFO			_IO(VDI_IOCTL_MAGIC, 12)
#define VDI_IOCTL_GET_FREE_MEM_SIZE			_IO(VDI_IOCTL_MAGIC, 13)
#define VDI_IOCTL_VPU_RESET				_IO(VDI_IOCTL_MAGIC, 21)

#define VDI_IOCTL_FLUSH_DCACHE                          _IO(VDI_IOCTL_MAGIC, 24)
#define VDI_IOCTL_INVALIDATE_DCACHE                     _IO(VDI_IOCTL_MAGIC, 25)

typedef struct vpudrv_buffer_t {
    unsigned long size;
    unsigned long phys_addr;
    unsigned long base;         /* kernel logical address in use kernel */
    unsigned long virt_addr;    /* virtual user space address */

    int    ion_fd;
    struct dma_buf_attachment *attach;
    struct sg_table *table;
    struct dma_buf *dma_buf;

} vpudrv_buffer_t;

typedef struct vpu_bit_firmware_info_t {
	unsigned int size;						/* size of this structure*/
	unsigned int core_idx;
	unsigned long reg_base_offset;
	unsigned short bit_code[512];
} vpu_bit_firmware_info_t;

typedef struct vpudrv_inst_info_t {
	unsigned int core_idx;
	unsigned int inst_idx;
    int support_cq;
	int inst_open_count;	/* for output only*/
} vpudrv_inst_info_t;

typedef struct vpudrv_intr_info_t {
	unsigned int timeout;
	int			intr_reason;
	int			intr_inst_index;
	int         core_idx;
} vpudrv_intr_info_t;

typedef enum {
	VPUDRV_MUTEX_VPU,
	VPUDRV_MUTEX_DISP_FALG,
	VPUDRV_MUTEX_RESET,
	VPUDRV_MUTEX_VMEM,
	VPUDRV_MUTEX_REV1,
	VPUDRV_MUTEX_REV2,
	VPUDRV_MUTEX_MAX
} vpudrv_mutex_type;

#define VDI_NUM_LOCK_HANDLES                6

#endif

