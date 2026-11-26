/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdint.h>

#include <zephyr/ztest.h>

#include <app/lib/publisher.h>
#include <app/lib/telemetry_queue.h>

#include "publishing.h"

/** @brief What the fake publisher returns. 0 = success, negative = failure. */
static int publish_result;

/** @brief How many times the fake publisher has been called. */
static int publish_count;

/** @brief Seq number of the last record the fake publisher got. */
static uint32_t last_published_seq;

/**
 * @brief Fake publisher, used instead of the real telemetry_publish().
 *
 * Counts the calls, saves the seq number of the record and returns
 * publish_result, so a test can make publishing fail by setting
 * publish_result to an error code.
 *
 * @param record The record to publish.
 *
 * @return publish_result
 */
int telemetry_publish(const struct telemetry_record *record)
{
	publish_count++;
	last_published_seq = record->seq;

	return publish_result;
}

/**
 * @brief Runs before every test to start from a clean state.
 *
 * The queue and the fake variables are shared by all tests, so whatever
 * the test before leaves behind will show up in the next test. This
 * function makes the publisher succeed and clears the queue.
 *
 * @param fixture Test data from setup. Not used here since setup is NULL.
 */
static void reset_before_each_test(void *fixture)
{
	ARG_UNUSED(fixture);

	publish_result = 0;
	publish_count = 0;
	last_published_seq = 0;

	telemetry_queue_reset();
}

/**
 * @brief ZTEST_SUITE(name, predicate, setup, before, after, teardown)
 *
 * @param name      Suite name, used by every ZTEST() below.
 * @param predicate Decides if the suite runs, NULL = always.
 * @param setup     Runs once before all tests.
 * @param before    Runs before each test, resets the fakes and the queue.
 * @param after     Runs after each test.
 * @param teardown  Runs once after all tests.
 */
ZTEST_SUITE(telemetry_app_test, NULL, NULL, reset_before_each_test, NULL, NULL);

/**
 * @brief Add a record with the given seq number directly to the queue.
 *
 * The publish tests use this instead of a sample, so they do not depend on
 * the sensor.
 *
 * @param seq Seq number of the record.
 */
static void add_record(uint32_t seq)
{
	const struct telemetry_record record = {.seq = seq};

	zassert_ok(telemetry_queue_add(&record));
}

/**
 * @brief A published record should be removed from the queue.
 *
 * Adds one record and publishes the queue. The publisher should be
 * called once and the queue should be empty afterwards.
 */
ZTEST(telemetry_app_test, test_publish_queued_removes_published_record)
{
	add_record(0);

	zassert_ok(telemetry_publish_queued());

	zassert_equal(publish_count, 1);
	zassert_equal(telemetry_queue_count(), 0U);
}

/**
 * @brief The oldest record should be published first.
 *
 * Adds two records, seq 0 and seq 1, and lets the publisher fail. Then
 * only the first record is tried, and that should be seq 0 since it is the
 * oldest.
 */
ZTEST(telemetry_app_test, test_publish_queued_sends_oldest_first)
{
	add_record(0);
	add_record(1);
	publish_result = -EIO;

	zassert_equal(telemetry_publish_queued(), -EIO);

	zassert_equal(publish_count, 1);
	zassert_equal(last_published_seq, 0U);
}

/**
 * @brief Publishing with a failing publisher should return the error.
 *
 * The fake publisher returns -EIO, so publish should pass that error on.
 */
ZTEST(telemetry_app_test, test_publish_queued_failure_returns_error)
{
	add_record(0);
	publish_result = -EIO;

	int ret = telemetry_publish_queued();

	zassert_equal(ret, -EIO);
}

/**
 * @brief A failed publish should keep the record in the queue.
 *
 * Adds one record and lets the publisher fail. The record was not sent,
 * so it should still be in the queue for a later attempt.
 */
ZTEST(telemetry_app_test, test_publish_queued_failure_keeps_record)
{
	add_record(0);
	publish_result = -EIO;

	zassert_equal(telemetry_publish_queued(), -EIO);

	zassert_equal(telemetry_queue_count(), 1U);
}

/**
 * @brief A kept record should be published on the next attempt.
 *
 * The first publish fails and the record stays in the queue. The second
 * publish succeeds, so the same record, seq 0, should be sent and removed.
 */
ZTEST(telemetry_app_test, test_publish_queued_retries_kept_record)
{
	add_record(0);
	publish_result = -EIO;
	zassert_equal(telemetry_publish_queued(), -EIO);

	publish_result = 0;
	zassert_ok(telemetry_publish_queued());

	zassert_equal(last_published_seq, 0U);
	zassert_equal(telemetry_queue_count(), 0U);
}
