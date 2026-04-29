#ifndef _UAPI__ASM_GENERIC_BPF_PERF_EVENT_H__
#define _UAPI__ASM_GENERIC_BPF_PERF_EVENT_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "../../linux/mydefs.h"
#include "../../linux/ptrace.h"

/* Export kernel pt_regs structure */
typedef struct pt_regs bpf_user_pt_regs_t;

#ifdef __cplusplus
}
#endif
#endif /* _UAPI__ASM_GENERIC_BPF_PERF_EVENT_H__ */
