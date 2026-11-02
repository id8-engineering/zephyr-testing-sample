/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>

#include <zephyr/ztest.h>

#include <app/lib/publisher.h>

ZTEST_SUITE(logger_publisher, NULL, NULL, NULL, NULL, NULL);

ZTEST(logger_publisher, test_publish_known_record)
{
	const struct telemetry_record record = {
		.seq = 56,
		.uptime_ms = 1000,
		.temperature_mc = 180000,
		.humidity_mpct = 75000,
	};

	zassert_ok(telemetry_publish(&record));
}

ZTEST(logger_publisher, test_publish_null_returns_einval)
{
	zassert_equal(telemetry_publish(NULL), -EINVAL);
}
