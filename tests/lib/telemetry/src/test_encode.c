/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>

#include <zephyr/ztest.h>
#include <app/lib/telemetry.h>

ZTEST_SUITE(encode_test, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief Test a typical record.
 *
 * Test a typical record with normal values and checks the JSON string.
 */
ZTEST(encode_test, test_typical_record)
{
	const struct telemetry_record record = {
		.seq = 3,
		.uptime_ms = 15000,
		.temperature_mc = 21500,
		.humidity_mpct = 45000,
	};
	char buffer[TELEMETRY_JSON_MAX_LEN];

	zassert_ok(telemetry_encode_record(&record, buffer, sizeof(buffer)));

	zassert_str_equal(buffer, "{\"seq\":3,\"uptime_ms\":15000,"
				  "\"temperature_mc\":21500,\"humidity_mpct\":45000}");
}

/**
 * @brief Test a negative temperature.
 *
 */
ZTEST(encode_test, test_negative_temperature)
{
	const struct telemetry_record record = {
		.seq = 0,
		.uptime_ms = 0,
		.temperature_mc = -12345,
		.humidity_mpct = 0,
	};
	char buffer[TELEMETRY_JSON_MAX_LEN];

	zassert_ok(telemetry_encode_record(&record, buffer, sizeof(buffer)));
	zassert_str_equal(buffer, "{\"seq\":0,\"uptime_ms\":0,"
				  "\"temperature_mc\":-12345,\"humidity_mpct\":0}");
}

/**
 * @brief Test NULL record.
 */
ZTEST(encode_test, test_null_record)
{
	char buffer[TELEMETRY_JSON_MAX_LEN];

	zassert_equal(telemetry_encode_record(NULL, buffer, sizeof(buffer)), -EINVAL);
}

/**
 * @brief Test NULL buffer.
 */
ZTEST(encode_test, test_null_buffer)
{
	const struct telemetry_record record = {0};

	zassert_equal(telemetry_encode_record(&record, NULL, TELEMETRY_JSON_MAX_LEN), -EINVAL);
}

/**
 * @brief Test a buffer size of 0.
 *
 * A buffer size of 0 is invalid and should return -EINVAL.
 */
ZTEST(encode_test, test_zero_buffer_size)
{
	const struct telemetry_record record = {0};
	char buffer[TELEMETRY_JSON_MAX_LEN];

	zassert_equal(telemetry_encode_record(&record, buffer, 0), -EINVAL);
}

/**
 * @brief Test a buffer that is too small.
 *
 * A 1 byte buffer cannot fit any record.
 */
ZTEST(encode_test, test_buffer_too_small)
{
	const struct telemetry_record record = {0};
	char buffer[1];

	zassert_equal(telemetry_encode_record(&record, buffer, sizeof(buffer)), -ENOMEM);
}
