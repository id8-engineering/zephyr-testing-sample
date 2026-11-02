/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>

#include <zephyr/ztest.h>

#include <app/lib/environment.h>

ZTEST_SUITE(environment_fake, NULL, NULL, NULL, NULL, NULL);

ZTEST(environment_fake, test_read_returns_configured_values)
{
	struct environment_reading reading;

	zassert_ok(environment_read(&reading));

	zassert_equal(reading.temperature_mc, CONFIG_ENVIRONMENT_FAKE_TEMPERATURE_MC);
	zassert_equal(reading.humidity_mpct, CONFIG_ENVIRONMENT_FAKE_HUMIDITY_MPCT);
}

ZTEST(environment_fake, test_read_null_returns_einval)
{
	zassert_equal(environment_read(NULL), -EINVAL);
}
