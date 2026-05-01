/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright © 2016 Intel Corporation
 */

#ifndef __MOCK_DMABUF_H__
#define __MOCK_DMABUF_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../../../../include/linux/mydefs.h"
#include "../../../../../../include/linux/dma-buf.h"

struct mock_dmabuf {
	int npages;
	struct page *pages[];
};

static inline struct mock_dmabuf *to_mock(struct dma_buf *buf)
{
	return buf->priv;
}

#ifdef __cplusplus
}
#endif
#endif /* !__MOCK_DMABUF_H__ */
