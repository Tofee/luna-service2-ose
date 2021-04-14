// Copyright (c) 2008-2021 LG Electronics, Inc.
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

#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <stdbool.h>
#include <time.h>
#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @cond INTERNAL */

void ClockGetTime(struct timespec *time);

bool ClockTimeIsGreater(struct timespec *a, struct timespec *b);

void ClockStr(GString *str, struct timespec *time);

void ClockPrintTime(struct timespec *time);
void ClockPrint(void);

bool ClockDiff(struct timespec *diff, const struct timespec *a, const struct timespec *b);
void ClockAccum(struct timespec *sum, const struct timespec *b);

void ClockAccumMs (struct timespec *sum, int duration_ms);

long long ClockGetMs(struct timespec *ts);

void ClockClear(struct timespec *a);

#ifdef __cplusplus
}
#endif

/** @endcond */

#endif //_CLOCK_H_
