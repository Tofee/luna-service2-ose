// Copyright (c) 2008-2019 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0


#ifndef _BASE_H_
#define _BASE_H_


#include <glib.h>
#include <pthread.h>
#include <stdbool.h>
#include <pbnjson.h>

#include "error.h"
#include "signal.h"
#include "subscription.h"
#include "transport.h"

/**
 * @cond INTERNAL
 * @addtogroup LunaServiceInternals
 * @{
 */

extern bool _ls_enable_utf8_validation;
__attribute__((visibility("hidden")))
extern volatile gboolean activity;
__attribute__((visibility("hidden")))
extern volatile int activity_num;

#define WAKEUP() g_atomic_int_set(&activity, 1)
#define ACTIVITY_INC() g_atomic_int_add(&activity_num,  1)
#define ACTIVITY_DEC() g_atomic_int_add(&activity_num, -1)

void LSDebugLogIncoming(const char *where, _LSTransportMessage *message);

typedef struct _CallMap _CallMap;

bool _CallMapInit(LSHandle *sh, _CallMap **ret_map, LSError *lserror);
void _CallMapDeinit(LSHandle *sh, _CallMap *map);

void _LSGlobalLock();
void _LSGlobalUnlock();

bool _LSUnregisterCommon(LSHandle *sh, bool flush_and_send_shutdown, void *call_ret_addr, LSError *lserror);

#ifdef LSHANDLE_CHECK
/**
 *******************************************************************************
 * @brief Fields for detecting invalid handles and where they were created and destroyed
 *******************************************************************************
 */
typedef struct {
    unsigned long magic_state_num; /**< marks state as valid or destroyed */
    void *creator_ret_addr;        /**< ret addr of caller to outermost functions like LSRegister, etc */
    void *destroyer_ret_addr;      /**< ret addr of caller to outermost functions like LSUnregister, etc */
} _LSHandleHistory;

#define LSHANDLE_MAGIC_STATE_VALID     0xdecaf9a5  /* The handle has been initialized and not destroyed */
#define LSHANDLE_MAGIC_STATE_DESTROYED 0xd09faced  /* The handle has been destroyed */

#define LSHANDLE_SET_VALID(sh, ret)                                 \
do {                                                                \
    (sh)->history.magic_state_num = LSHANDLE_MAGIC_STATE_VALID;     \
    (sh)->history.creator_ret_addr = ret;                           \
    (sh)->history.destroyer_ret_addr = NULL;                        \
} while (0)

#define LSHANDLE_SET_DESTROYED(sh, ret)                             \
do {                                                                \
    (sh)->history.magic_state_num = LSHANDLE_MAGIC_STATE_DESTROYED; \
    (sh)->history.destroyer_ret_addr = ret;                         \
} while (0)

#define LSHANDLE_VALIDATE(sh) _lshandle_validate(sh)

void _lshandle_validate(LSHandle *sh);

#define LSHANDLE_GET_RETURN_ADDR() (__builtin_return_address(0))
#define LSHANDLE_POISON(sh) memset(sh, 0xFF, offsetof(LSHandle, history))

#else

#define LSHANDLE_SET_VALID(sh, ret) do {} while(0)
#define LSHANDLE_SET_DESTROYED(sh, ret) do {} while(0)
#define LSHANDLE_VALIDATE(sh) do {} while(0)
#define LSHANDLE_GET_RETURN_ADDR() (NULL)
#define LSHANDLE_POISON(sh) memset(sh, 0xFF, sizeof(LSHandle))

#endif

/**
 *******************************************************************************
 * @brief An Object representing a Luna Service.
 *
 * You may create a new Luna Service via LSRegister()
 *******************************************************************************
 */
struct LSHandle {
    char           *name;

    _LSTransport    *transport;     /**< underlying transport */

    GMainContext   *context;       /**< context associated with this handle */

    _CallMap       *callmap;       /**< contains outbound calls -> cbs */
    _Catalog       *catalog;       /**< contains subscriptions */

    GHashTable     *tableHandlers; /**< contains method tables */

    LSDisconnectHandler disconnect_handler;
    void           *disconnect_handler_data;

#ifdef SECURITY_COMPATIBILITY
    bool is_public_bus;            /**< for compatibility with old public/private connections */
#endif //SECURITY_COMPATIBILITY

#ifdef LSHANDLE_CHECK
    /* This  M U S T  be that last thing in the struct for LSHANDLE_POISON to work */
    _LSHandleHistory history;      /**< fields for detecting invalid handles and where they were created and destroyed */
#endif
};

/** Category for lunabus watch category signal */
#define LUNABUS_WATCH_CATEGORY_CATEGORY "/com/palm/bus/watch/category"

/**
 * @} END OF LunaServiceInternals
 * @endcond
 */

#endif // _BASE_H_
