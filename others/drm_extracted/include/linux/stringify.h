#ifndef __LINUX_STRINGIFY_H
#define __LINUX_STRINGIFY_H

#ifdef __cplusplus
extern "C" {
#endif
#include "mydefs.h"

/* Indirect stringification.  Doing two levels allows the parameter to be a
 * macro itself.  For example, compile with -DFOO=bar, __stringify(FOO)
 * converts to "bar".
 */

#define __stringify_1(x...)	#x
#define __stringify(x...)	__stringify_1(x)

#define FILE_LINE	__FILE__ ":" __stringify(__LINE__)

#ifdef __cplusplus
}
#endif
#endif	/* !__LINUX_STRINGIFY_H */
