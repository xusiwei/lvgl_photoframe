#ifndef APPLICATIONS_COMMON_MACROS_H_
#define APPLICATIONS_COMMON_MACROS_H_

#include <rtdbg.h>
#include <rtdef.h>

#define RT_CHECK(ipc_call)                                                   \
    do {                                                                     \
        rt_err_t err = (ipc_call);                                           \
        if (err != RT_EOK) {                                                 \
            LOG_E("%s: RT_CHECK(%s): %d failed!", __func__, #ipc_call, err); \
        }                                                                    \
    } while (0)

#define RT_CHECK_RETURN(ipc_call, retval)                                    \
    do {                                                                     \
        rt_err_t err = (ipc_call);                                           \
        if (err != RT_EOK) {                                                 \
            LOG_E("%s: RT_CHECK(%s): %d failed!", __func__, #ipc_call, err); \
            return retval;                                                   \
        }                                                                    \
    } while (0)

#endif // APPLICATIONS_COMMON_MACROS_H_