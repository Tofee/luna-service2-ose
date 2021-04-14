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


#include <stdlib.h>
#include <glib.h>
#include <stdio.h>
#include <clock.h>

/* Test cases *****************************************************************/

/* Several tests are heavily data-driven and could be refactored to have
 * a loop that utilizes a data table or similar structure. */

/* TODO only ClockGetMs() checks for null arguments.
 * We should test it on most tests, but it'll blow everything else if the code isn't fixed. */

/* Copied from clock.c */
#define NSEC_PER_SEC 1000000000LL
#define NSEC_PER_MSEC 1000000LL

/* void ClockGetTime(struct timespec *time) */
static void
test_ClockGetTime(void)
{
    struct timespec time;

    ClockGetTime(&time);

    /* Check if the values are sane. */

    g_assert_cmpint(time.tv_sec, >, 0); /* seconds - TYPE_T (long int) */
    g_assert_true(time.tv_nsec > 0); /* nanoseconds - long int */

    /* The value should always be less than one second. */
    int nsecToSec = time.tv_sec / NSEC_PER_SEC;
    g_assert_cmpint(nsecToSec, <, 1);
}

static void
test_ClockTimeIsGreater_validate_values(long a_sec, long long a_nsec, long b_sec, long long b_nsec, bool expected_a_b, bool expected_b_a)
{
    struct timespec a;
    a.tv_sec = a_sec;
    a.tv_nsec = a_nsec;
    struct timespec b;
    b.tv_sec = b_sec;
    b.tv_nsec = b_nsec;

    g_assert_true(ClockTimeIsGreater(&a, &b) == expected_a_b);
    g_assert_true(ClockTimeIsGreater(&b, &a) == expected_b_a);
}

/* bool ClockTimeIsGreater(struct timespec *a, struct timespec *b) */
static void
test_ClockTimeIsGreater(void)
{
    test_ClockTimeIsGreater_validate_values(0, 0, 0, 0, FALSE, FALSE);
    test_ClockTimeIsGreater_validate_values(4444, 0, 3333, 0, TRUE, FALSE);
    test_ClockTimeIsGreater_validate_values(4444, 987654321, 3333, 123456789, TRUE, FALSE);
    test_ClockTimeIsGreater_validate_values(0, 987654321, 0, 123456789, TRUE, FALSE);
    test_ClockTimeIsGreater_validate_values(LONG_MAX, 0, LONG_MIN, 0, TRUE, FALSE);
    test_ClockTimeIsGreater_validate_values(LONG_MAX, 0, 0, 0, TRUE, FALSE);
    test_ClockTimeIsGreater_validate_values(LONG_MIN, 0, 0, 0, FALSE, TRUE);
    test_ClockTimeIsGreater_validate_values(LONG_MAX, LONG_MAX, LONG_MAX, LONG_MAX, FALSE, FALSE);
    test_ClockTimeIsGreater_validate_values(LONG_MAX, LONG_MIN, LONG_MAX, LONG_MAX, FALSE, TRUE);
    test_ClockTimeIsGreater_validate_values(0, LONG_MIN, 0, LONG_MAX, FALSE, TRUE);
    test_ClockTimeIsGreater_validate_values(0, 0, 0, LONG_MAX, FALSE, TRUE);
    test_ClockTimeIsGreater_validate_values(0, 0, 0, LONG_MIN, TRUE, FALSE);
}

static void
test_ClockStr_validate_values(long sec, long nsec, gchar* prefix)
{
    GString *teststring = g_string_new(prefix);
    struct timespec time;
    time.tv_sec = sec;
    time.tv_nsec = nsec;
    GString *referencestring = g_string_new(prefix);
    g_string_append_printf(referencestring, "%lds.%lldms ", time.tv_sec, time.tv_nsec / NSEC_PER_MSEC);

    ClockStr(teststring, &time);
    g_assert_cmpstr(teststring->str, ==, referencestring->str);

    g_string_free(teststring, TRUE);
    g_string_free(referencestring, TRUE);
}

/* void ClockStr(GString *str, struct timespec *time)
 * Appends "nnnns.nnnnms" (formatted seconds and milliseconds) after a string. */
static void
test_ClockStr(void)
{
    test_ClockStr_validate_values(0, 0, "");
    test_ClockStr_validate_values(0, 0, "a");
    test_ClockStr_validate_values(0, 0, "qwertyuiopåpöoikujyhtgfrdesasdfghjklöäölkjmhnbgvfcdxsz<zxcvbnm,.-.");
    test_ClockStr_validate_values(0, 0, "\n\t\t\t\t\n\\n\\\\\"");
    test_ClockStr_validate_values(-1, 0, "");
    test_ClockStr_validate_values(-1, -1, "\xe2\x98\xa0\xe2\x98\xa0\xe2\x98\xa0\xe2\x98\xa0");
    test_ClockStr_validate_values(LONG_MIN, LONG_MIN, "");
    test_ClockStr_validate_values(LONG_MAX, LONG_MIN, "");
    test_ClockStr_validate_values(LONG_MAX, LONG_MAX, "ÄÄÄÄÄÄÄÄ\xe2\x98\xa0ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ\xe2\x98\xa0\xe2\x98\xa0\xe2\x98\xa0ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅÅ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÖÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄmorethan1024chars");
}

/* void ClockPrint() */
static void
test_ClockPrint(void)
{
#if GLIB_CHECK_VERSION(2, 38, 0)
    /* ClockPrint uses internally ClockPrintTime() so we'll stress test that one
       and check here only the print procedure. */
    /* Fork this test to capture stdout. */
    if (g_test_subprocess())
    {
        ClockPrint();
        exit(0);
    }
    g_test_trap_subprocess(NULL, 1000000, 0);
    g_test_trap_assert_passed();
    /* Consider regular expression to check for more specific "%lds.%ldms". */
    g_test_trap_assert_stderr("*s.*ms*");
#endif
}

struct timespec to_timespec(int sec, int nsec)
{
    struct timespec ret = {.tv_sec = sec, .tv_nsec = nsec};
    return ret;
}

bool timespec_equal(struct timespec* a, struct timespec* b)
{
    return (a->tv_nsec == b->tv_nsec) && (a->tv_sec == b->tv_sec);
}

static void
test_ClockDiff_validate(struct timespec diff, struct timespec a, struct timespec b, bool negative_result)
{
    struct timespec resulttime;
    g_assert_true(ClockDiff(&resulttime, &a, &b) == negative_result);
    g_assert_true(timespec_equal(&resulttime, &diff));
}

static void
test_ClockDiff(void)
{
    test_ClockDiff_validate(to_timespec(0, 0), to_timespec(0, 0), to_timespec(0, 0), false);
    test_ClockDiff_validate(to_timespec(0, 0), to_timespec(1, 0), to_timespec(1, 0), false);
    test_ClockDiff_validate(to_timespec(1, 0), to_timespec(2, 0), to_timespec(1, 0), false);
    test_ClockDiff_validate(to_timespec(-1, 0), to_timespec(1, 0), to_timespec(2, 0), true);
    test_ClockDiff_validate(to_timespec(0, 0), to_timespec(0, 1), to_timespec(0, 1), false);
    test_ClockDiff_validate(to_timespec(0, 1), to_timespec(0, 2), to_timespec(0, 1), false);
    test_ClockDiff_validate(to_timespec(-1, NSEC_PER_SEC-1), to_timespec(0, 1), to_timespec(0, 2), true);
    test_ClockDiff_validate(to_timespec(-2, NSEC_PER_SEC-1), to_timespec(1, 1), to_timespec(2, 2), true);
}

static void
test_ClockAccum_validate(struct timespec diff, struct timespec a, struct timespec b)
{
    /* a will contain the total sum a+b */
    ClockAccum(&a, &b);

    g_assert_true(timespec_equal(&diff, &a));
}

/* void test_ClockAccum(struct timespec *sum, struct timespec *b) */
static void
test_ClockAccum(void)
{
    /* INT_MIN/INT_MAX aren't corner cases, but are typical in writing errors. */
    test_ClockAccum_validate(to_timespec(0, 0), to_timespec(0, 0), to_timespec(0, 0));
    test_ClockAccum_validate(to_timespec(1, 0), to_timespec(1, 0), to_timespec(0, 0));
    test_ClockAccum_validate(to_timespec(2, 0), to_timespec(1, 0), to_timespec(1, 0));
    test_ClockAccum_validate(to_timespec(0, 1), to_timespec(0, 1), to_timespec(0, 0));
    test_ClockAccum_validate(to_timespec(0, 1), to_timespec(0, 0), to_timespec(0, 1));
    test_ClockAccum_validate(to_timespec(0, 2), to_timespec(0, 1), to_timespec(0, 1));
    test_ClockAccum_validate(to_timespec(1, 2*(NSEC_PER_SEC/2 + 1) - NSEC_PER_SEC) , to_timespec(0, NSEC_PER_SEC/2 + 1), to_timespec(0, NSEC_PER_SEC/2 + 1));
}

static void
test_ClockAccumMs_validate(long sec, long nsec, int ms)
{
    struct timespec ts;
    ts.tv_sec = sec;
    ts.tv_nsec = nsec;

    long long reference_nsec = (sec * NSEC_PER_SEC) + nsec + (ms * NSEC_PER_MSEC);

    /* ts will contain ts+ms */
    ClockAccumMs(&ts, ms);

    long long total_result_nsec = (ts.tv_sec * NSEC_PER_SEC) + ts.tv_nsec;

    g_assert_true(total_result_nsec == reference_nsec);
}

/* void ClockAccumMs (struct timespec *sum, int duration_ms) */
static void
test_ClockAccumMs(void)
{
    test_ClockAccumMs_validate(0, 0, 0);
    test_ClockAccumMs_validate(-1, 0, 0);
    test_ClockAccumMs_validate(0, -1, 0);
    test_ClockAccumMs_validate(0, 0, -1);
    test_ClockAccumMs_validate(LONG_MAX, 0, 0);
    test_ClockAccumMs_validate(0, LONG_MAX, 0);
    test_ClockAccumMs_validate(0, 0, INT_MAX);
    test_ClockAccumMs_validate(LONG_MIN, 0, 0);
    test_ClockAccumMs_validate(0, LONG_MIN, 0);
    test_ClockAccumMs_validate(0, 0, INT_MIN);
    test_ClockAccumMs_validate(LONG_MAX, NSEC_PER_SEC, 0);
    if(sizeof(long) > sizeof(int)) //disable following test cases on 32bit arch
    {
        test_ClockAccumMs_validate(LONG_MAX, NSEC_PER_SEC, INT_MAX);
        test_ClockAccumMs_validate(LONG_MAX, NSEC_PER_SEC, INT_MIN);
        test_ClockAccumMs_validate(LONG_MIN, 0, INT_MIN);
        test_ClockAccumMs_validate(LONG_MIN, -NSEC_PER_SEC, INT_MIN);
    }
}

static void
test_ClockGetMs_validate(long sec, long nsec)
{
    struct timespec ts;
    ts.tv_sec = sec;
    ts.tv_nsec = nsec;
    long long reference = ((sec * NSEC_PER_SEC) + nsec) / NSEC_PER_MSEC;

    long long milliseconds = ClockGetMs(&ts);

    g_assert_true(milliseconds == reference);
}

/* long long ClockGetMs(struct timespec *ts) */
static void
test_ClockGetMs(void)
{
    test_ClockGetMs_validate(0, 0);
    test_ClockGetMs_validate(1, 0);
    test_ClockGetMs_validate(0, 1);
    test_ClockGetMs_validate(-1, 0);
    test_ClockGetMs_validate(0, -1);
    test_ClockGetMs_validate(INT_MAX, 0);
    test_ClockGetMs_validate(0, INT_MAX);
    test_ClockGetMs_validate(INT_MAX, INT_MAX);
    test_ClockGetMs_validate(LONG_MAX, 0);
    test_ClockGetMs_validate(0, NSEC_PER_SEC);
    test_ClockGetMs_validate(LONG_MAX, NSEC_PER_SEC);
    test_ClockGetMs_validate(LONG_MIN, -NSEC_PER_SEC);
}

static void
test_ClockClear_validate(long sec, long nsec)
{
    struct timespec a;
    a.tv_sec = sec;
    a.tv_nsec = nsec;

    ClockClear(&a);

    g_assert_true(a.tv_sec == 0);
    g_assert_true(a.tv_nsec == 0);
}

/* void ClockClear(struct timespec *a) */
static void
test_ClockClear(void)
{
    test_ClockClear_validate(0, 0);
    test_ClockClear_validate(-1, 0);
    test_ClockClear_validate(0, -1);
    test_ClockClear_validate(-1, -1);
    test_ClockClear_validate(LONG_MAX, 0);
    test_ClockClear_validate(LONG_MAX, -1);
    test_ClockClear_validate(LONG_MAX, LONG_MAX);
    test_ClockClear_validate(LONG_MIN, 0);
    test_ClockClear_validate(LONG_MIN, -1);
    test_ClockClear_validate(LONG_MIN, LONG_MAX);
    test_ClockClear_validate(LONG_MAX, LONG_MIN);
    test_ClockClear_validate(0, LONG_MAX);
}

/* Test suite *****************************************************************/

int
main(int argc, char *argv[])
{
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/luna-service2/ClockGetTime", test_ClockGetTime);
    g_test_add_func("/luna-service2/ClockTimeIsGreater", test_ClockTimeIsGreater);
    g_test_add_func("/luna-service2/ClockStr", test_ClockStr);
    g_test_add_func("/luna-service2/ClockPrint", test_ClockPrint);
    g_test_add_func("/luna-service2/ClockDiff", test_ClockDiff);
    g_test_add_func("/luna-service2/ClockAccum", test_ClockAccum);
    g_test_add_func("/luna-service2/ClockAccumMs", test_ClockAccumMs);
    g_test_add_func("/luna-service2/ClockGetMs", test_ClockGetMs);
    g_test_add_func("/luna-service2/ClockClear", test_ClockClear);

    return g_test_run();
}

