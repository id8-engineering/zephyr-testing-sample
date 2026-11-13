/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>

#include <zephyr/kernel.h>
#include <zephyr/ztest.h>

#include <app/lib/environment.h>
#include <app/lib/telemetry.h>

ZTEST_SUITE(telemetry_test, NULL, NULL, NULL, NULL, NULL);

/**
 * @brief Test return value.
 *
 * Creates one record with a valid pointer and checks that the
 * function returns 0.
 */
ZTEST(telemetry_test, test_check_return_equals_to_0)
{
	struct telemetry_record record;

	zassert_ok(telemetry_record_create(&record));
}

/**
 * @brief Test that a NULL record returns -EINVAL.
 *
 * I put in NULL instead of a record and checks if the function catches that.
 *
 */
ZTEST(telemetry_test, test_read_null_returns_einval)
{
	zassert_equal(telemetry_record_create(NULL), -EINVAL);
}

/**
 * @brief Test that reset sets the sequence number back to 0.
 *
 * Creates two records so the sequence number is no longer 0 then
 * calls the reset function and check if the next record gets sequence number 0.
 */
ZTEST(telemetry_test, test_reset_sets_seq_to_0)
{
	struct telemetry_record record;

	zassert_ok(telemetry_record_create(&record));
	zassert_ok(telemetry_record_create(&record));
	zassert_not_equal(record.seq, 0U);

	telemetry_reset();

	zassert_ok(telemetry_record_create(&record));
	zassert_equal(record.seq, 0U);
}

/**
 * @brief Test that the sequence number is increasing by 1 for every record.
 *
 * Resets the sequence number and create 10 records with an for loop. Each record is
 * compared with an value i know is correct so they should match if succeded.
 */
ZTEST(telemetry_test, test_seq_increasing_check)
{
	struct telemetry_record record;
	uint32_t expected_seq = 0U;

	telemetry_reset();

	for (int i = 0; i < 10; i++) {
		zassert_ok(telemetry_record_create(&record));
		zassert_equal(record.seq, expected_seq);
		expected_seq++;
	}
}

/**
 * @brief Test that the record gets the values from the sensor.
 *
 * Creates one record and checks that temperature and humidity are the
 * same as the values configured for the fake sensor in prj.conf.
 */
ZTEST(telemetry_test, test_record_has_sensor_values)
{
	struct telemetry_record record;

	zassert_ok(telemetry_record_create(&record));

	zassert_equal(record.temperature_mc, CONFIG_ENVIRONMENT_FAKE_TEMPERATURE_MC);
	zassert_equal(record.humidity_mpct, CONFIG_ENVIRONMENT_FAKE_HUMIDITY_MPCT);
}

/**
 * @brief Test that every record gets the same sensor values.
 *
 * Creates 10 records and checks temperature and humidity in every
 * record not only in the first one.
 */
ZTEST(telemetry_test, test_sensor_values_same_in_every_record)
{
	struct telemetry_record record;

	for (int i = 0; i < 10; i++) {
		zassert_ok(telemetry_record_create(&record));
		zassert_equal(record.temperature_mc, CONFIG_ENVIRONMENT_FAKE_TEMPERATURE_MC);
		zassert_equal(record.humidity_mpct, CONFIG_ENVIRONMENT_FAKE_HUMIDITY_MPCT);
	}
}

/**
 * @brief Test that reset in the middle of a series starts over from 0.
 *
 * Creates 5 records with sequence number 0 to 4 calls reset and
 * creates 5 more. The second series should also get 0 to 4.
 */
ZTEST(telemetry_test, test_reset_in_middle_of_series)
{
	struct telemetry_record record;
	uint32_t expected_seq = 0U;

	telemetry_reset();

	for (int i = 0; i < 5; i++) {
		zassert_ok(telemetry_record_create(&record));
		zassert_equal(record.seq, expected_seq);
		expected_seq++;
	}

	telemetry_reset();
	expected_seq = 0U;

	for (int i = 0; i < 5; i++) {
		zassert_ok(telemetry_record_create(&record));
		zassert_equal(record.seq, expected_seq);
		expected_seq++;
	}
}

/**
 * @brief Test that a NULL record does not use up a sequence number.
 *
 * Resets the sequence number and calls the function with NULL. The
 * next valid record should still get sequence number 0.
 */
ZTEST(telemetry_test, test_null_does_not_consume_seq)
{
	struct telemetry_record record;

	telemetry_reset();

	zassert_equal(telemetry_record_create(NULL), -EINVAL);

	zassert_ok(telemetry_record_create(&record));
	zassert_equal(record.seq, 0U);
}

/**
 * @brief Test that the uptime increases between two records.
 *
 * Creates a record, sleeps for 100 ms and creates another one. The
 * second uptime should be at least 100 ms later than the first.
 */
ZTEST(telemetry_test, test_uptime_increases_between_records)
{
	struct telemetry_record first;
	struct telemetry_record second;

	zassert_ok(telemetry_record_create(&first));
	k_sleep(K_MSEC(100));
	zassert_ok(telemetry_record_create(&second));

	zassert_true(second.uptime_ms >= first.uptime_ms + 100);
}
