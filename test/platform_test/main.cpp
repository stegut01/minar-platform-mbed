/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed/test_env.h"
#include "us_ticker_api.h"
#include "minar-platform/minar_platform.h"

using namespace minar::platform;

static tick_t ms_to_ticks(uint32_t milliseconds)
{
    uint64_t ticks = (((uint64_t)milliseconds) * ((uint64_t)Time_Base)) / 1000;
    return (tick_t)ticks;
}

void app_start(int, char*[])
{
    MBED_HOSTTEST_TIMEOUT(10);
    MBED_HOSTTEST_SELECT(default);
    MBED_HOSTTEST_DESCRIPTION(minar mbed platform);
    MBED_HOSTTEST_START("minar mbed platform");

    const char *current_test = "none";
    bool tests_pass = false;

    do {
        current_test = "platform init";
        init();

        current_test = "overflow check";
        if (getTimeOverflows()) {
            break;
        }

        // Test lp ticker read with 5ms period
        {
            current_test = "get time 5 ms test";
            Timer timer;
            tick_t old_ticks = getTime();
            timer.start();
            int start = timer.read_ms();

            // wait (=block) for 5 ms
            while ((timer.read_ms() - start) < 5);
            tick_t diff = getTime() - old_ticks;
            if ((diff < ms_to_ticks(5)) && (diff > ms_to_ticks(6))) {
                break;
            }
        }

        {
            // Go to sleep and wake up
            current_test = "sleep until test for 10ms";
            tick_t old_ticks = getTime();
            sleepFromUntil(old_ticks, old_ticks + ms_to_ticks(10));
            tick_t diff = getTime() - old_ticks;
            if (diff <  ms_to_ticks(10) && diff > ms_to_ticks(11)) {
                break;
            }
        }

        {
            current_test = "IRQ state push and pop";
            irqstate_t irq_state= pushDisableIRQState();
            popDisableIRQState(irq_state);
            if (irq_state != __get_PRIMASK()) {
                break;
            }
        }

        tests_pass = true;
    } while (0);

    if (!tests_pass) {
        printf("First failing test: %s \r\n", current_test);
    }

    MBED_HOSTTEST_RESULT(tests_pass);
}
