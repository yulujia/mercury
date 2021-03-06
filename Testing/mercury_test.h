/*
 * Copyright (C) 2013-2019 Argonne National Laboratory, Department of Energy,
 *                    UChicago Argonne, LLC and The HDF Group.
 * All rights reserved.
 *
 * The full copyright notice, including terms governing use, modification,
 * and redistribution, is contained in the COPYING file that can be
 * found at the root of the source code distribution tree.
 */

#ifndef MERCURY_TEST_H
#define MERCURY_TEST_H

#include "na_test.h"

#include "mercury.h"
#include "mercury_request.h"
#ifdef HG_TEST_HAS_THREAD_POOL
# include "mercury_thread_pool.h"
# include "mercury_thread_mutex.h"
#endif
#include "mercury_atomic.h"

#include "test_rpc.h"
#include "test_bulk.h"
#include "test_overflow.h"

#ifdef HG_TEST_HAS_CRAY_DRC
# include <rdmacred.h>
#endif

/*************************************/
/* Public Type and Struct Definition */
/*************************************/

struct hg_test_info {
    struct na_test_info na_test_info;
#ifdef HG_TEST_HAS_THREAD_POOL
    hg_thread_mutex_t bulk_handle_mutex;
    hg_thread_pool_t *thread_pool;
#endif
    hg_class_t *hg_class;
    hg_context_t *context;
    hg_request_class_t *request_class;
    hg_addr_t target_addr;
    hg_bulk_t bulk_handle;
    hg_bool_t auth;
#ifdef HG_TEST_HAS_CRAY_DRC
    uint32_t credential;
    uint32_t wlm_id;
    drc_info_handle_t credential_info;
    uint32_t cookie;
#endif
    unsigned int thread_count;
    hg_bool_t auto_sm;
};

struct hg_test_context_info {
    hg_atomic_int32_t finalizing;
};

/*****************/
/* Public Macros */
/*****************/

/* Default error macro */
#ifdef HG_HAS_VERBOSE_ERROR
# include <mercury_log.h>
# define HG_TEST_LOG_MASK hg_test_log_mask
/* Log mask will be initialized in init routine */
extern unsigned int HG_TEST_LOG_MASK;
# define HG_TEST_LOG_MODULE_NAME "HG Test"
# define HG_TEST_LOG_ERROR(...) do {                                \
    if (HG_TEST_LOG_MASK & HG_LOG_TYPE_ERROR)                       \
        HG_LOG_WRITE_ERROR(HG_TEST_LOG_MODULE_NAME, __VA_ARGS__);   \
} while (0)
# define HG_TEST_LOG_DEBUG(...) do {                                \
    if (HG_TEST_LOG_MASK & HG_LOG_TYPE_DEBUG)                       \
        HG_LOG_WRITE_DEBUG(HG_TEST_LOG_MODULE_NAME, __VA_ARGS__);   \
} while (0)
# define HG_TEST_LOG_WARNING(...) do {                              \
    if (HG_TEST_LOG_MASK & HG_LOG_TYPE_WARNING)                     \
        HG_LOG_WRITE_WARNING(HG_TEST_LOG_MODULE_NAME, __VA_ARGS__); \
} while (0)
#else
# define HG_TEST_LOG_ERROR(...)     (void)0
# define HG_TEST_LOG_DEBUG(...)     (void)0
# define HG_TEST_LOG_WARNING(...)   (void)0
#endif

/* Branch predictor hints */
#ifndef _WIN32
# define likely(x)       __builtin_expect(!!(x), 1)
# define unlikely(x)     __builtin_expect(!!(x), 0)
#else
# define likely(x)       (x)
# define unlikely(x)     (x)
#endif

/* Error macros */
#define HG_TEST_GOTO_DONE(label, ret, ret_val) do {             \
    ret = ret_val;                                              \
    goto label;                                                 \
} while (0)

#define HG_TEST_GOTO_ERROR(label, ret, err_val, ...) do {       \
    HG_LOG_ERROR(__VA_ARGS__);                                  \
    ret = err_val;                                              \
    goto label;                                                 \
} while (0)

/* Check for hg_ret value and goto label */
#define HG_TEST_CHECK_HG_ERROR(label, hg_ret, ...) do {         \
    if (unlikely(hg_ret != HG_SUCCESS)) {                       \
        HG_TEST_LOG_ERROR(__VA_ARGS__);                         \
        goto label;                                             \
    }                                                           \
} while (0)

/* Check for cond, set ret to err_val and goto label */
#define HG_TEST_CHECK_ERROR(cond, label, ret, err_val, ...) do {\
    if (unlikely(cond)) {                                       \
        HG_TEST_LOG_ERROR(__VA_ARGS__);                         \
        ret = err_val;                                          \
        goto label;                                             \
    }                                                           \
} while (0)

#define HG_TEST_CHECK_ERROR_NORET(cond, label, ...) do {        \
    if (unlikely(cond)) {                                       \
        HG_TEST_LOG_ERROR(__VA_ARGS__);                         \
        goto label;                                             \
    }                                                           \
} while (0)

#define HG_TEST_CHECK_ERROR_DONE(cond, ...) do {                \
    if (unlikely(cond)) {                                       \
        HG_TEST_LOG_ERROR(__VA_ARGS__);                         \
    }                                                           \
} while (0)

/* Check for cond and print warning */
#define HG_TEST_CHECK_WARNING(cond, ...) do {                   \
    if (unlikely(cond)) {                                       \
        HG_TEST_LOG_WARNING(__VA_ARGS__);                       \
    }                                                           \
} while (0)

#define HG_TEST(x) do {                                         \
    printf("Testing %-62s", x);                                 \
    fflush(stdout);                                             \
} while (0)

#define HG_PASSED() do {                                        \
    puts(" PASSED");                                            \
    fflush(stdout);                                             \
} while (0)

#define HG_FAILED() do {                                        \
    puts("*FAILED*");                                           \
    fflush(stdout);                                             \
} while (0)

/*********************/
/* Public Prototypes */
/*********************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize client/server
 */
hg_return_t
HG_Test_init(int argc, char *argv[], struct hg_test_info *hg_test_info);

/**
 * Finalize client/server
 */
hg_return_t
HG_Test_finalize(struct hg_test_info *hg_test_info);

#ifdef __cplusplus
}
#endif

#endif /* MERCURY_TEST_H */
