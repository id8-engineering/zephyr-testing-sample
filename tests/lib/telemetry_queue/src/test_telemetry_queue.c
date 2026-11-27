/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdint.h>

#include <zephyr/ztest.h>

#include <app/lib/telemetry.h>
#include <app/lib/telemetry_queue.h>

/**
 * @brief Runs before every test to clear the queue.
 *
 * The queue is shared by all tests so whatever the
 * test before leaves in memory will show up in the next test. Therefore
 * this function should be used before every test.
 *
 * This is solved in ZTEST_SUITE.
 *
 * @param fixture Test data from setup. Not used here since setup is NULL.
 */
static void reset_queue_before_each_test(void *fixture)
{
	ARG_UNUSED(fixture);

	telemetry_queue_reset();
}

/**
 * @brief ZTEST_SUITE(name, predicate, setup, before, after, teardown)
 *
 * @param name      Suite name, used by every ZTEST() below.
 * @param predicate Decides if the suite runs, NULL = always.
 * @param setup     Runs once before all tests.
 * @param before    Runs before each test, clears the queue.
 * @param after     Runs after each test.
 * @param teardown  Runs once after all tests.
 */
ZTEST_SUITE(telemetry_queue_test, NULL, NULL, reset_queue_before_each_test, NULL, NULL);

/**
 * @brief Helper function that adds a number of records to the queue.
 *
 * The first record gets first_seq as its seq number and every record after
 * that gets one more than the previous one so it is easy to see which
 * record is which. Every add has to return 0 so only use this when the
 * queue has room for all the records.
 *
 * @param first_seq Seq number of the first record.
 * @param n Number of records to add.
 */
static void add_records(uint8_t first_seq, uint8_t n)
{
	struct telemetry_record record = {0};

	for (uint8_t i = 0; i < n; i++) {
		record.seq = first_seq + i;
		zassert_ok(telemetry_queue_add(&record));
	}
}

/**
 * @brief Helper function that fills the queue.
 *
 * Adds TELEMETRY_MAX_RECORDS records to the queue with seq 0, 1, 2 and so on.
 * After this the queue is full and the next add will remove seq 0 (the oldest).
 */
static void fill_queue(void)
{
	add_records(0, TELEMETRY_MAX_RECORDS);
}

/**
 * @brief Helper function that clears the queue and checks every record.
 *
 * First it checks that the queue holds exactly n records. Then the function takes
 * the records out one by one starting with the oldest first and checks that the seq numbers
 * go first_seq, first_seq + 1 and so on without any gaps between the numbers. When it is done the
 * queue should be empty.
 *
 * Each record has to be removed after it is checked. Peek only shows the
 * oldest record so the only way to see the next one is to remove the one
 * in front of it.
 *
 * @param first_seq Seq number the oldest record should have.
 * @param n         Number of records the queue should hold.
 */
static void remove_and_check_records(uint8_t first_seq, uint8_t n)
{
	struct telemetry_record oldest;

	zassert_equal(telemetry_queue_count(), n);

	for (uint8_t i = 0; i < n; i++) {
		zassert_ok(telemetry_queue_peek(&oldest));
		zassert_equal(oldest.seq, first_seq + i);
		zassert_ok(telemetry_queue_remove(oldest.seq));
	}

	zassert_equal(telemetry_queue_count(), 0U);
}

/**
 * @brief Adding a record to an empty queue should return 0.
 *
 */
ZTEST(telemetry_queue_test, test_add_valid_record_returns_0)
{
	const struct telemetry_record record = {.seq = 1};

	int ret = telemetry_queue_add(&record);

	zassert_ok(ret);
}

/**
 * @brief Adding a record should make the count go up by one.
 *
 * Starts with an empty queue, adds one record and expects the count to be 1.
 */
ZTEST(telemetry_queue_test, test_add_valid_record_increments_count)
{
	const struct telemetry_record record = {.seq = 1};

	zassert_ok(telemetry_queue_add(&record));

	zassert_equal(telemetry_queue_count(), 1U);
}

/**
 * @brief Adding a NULL record should fail with -EINVAL.
 *
 * There is nothing to copy into the queue so add should refuse and return
 * -EINVAL.
 */
ZTEST(telemetry_queue_test, test_add_null_returns_einval)
{
	int ret = telemetry_queue_add(NULL);

	zassert_equal(ret, -EINVAL);
}

/**
 * @brief Records should come out in the same order they went in (FIFO).
 *
 * Adds three records with seq 1, 2 and 3 and then clears the queue. They
 * should come out as 1, 2, 3.
 */
ZTEST(telemetry_queue_test, test_add_keeps_fifo_order)
{
	add_records(1, 3);

	remove_and_check_records(1, 3);
}

/**
 * @brief Adding when the queue is full should return 1.
 *
 * Fills the queue and adds one more record. The record still gets added but
 * the oldest one should be discarded and add tells us that by returning
 * 1 instead of 0.
 */
ZTEST(telemetry_queue_test, test_add_to_full_queue_returns_1)
{
	const struct telemetry_record record = {.seq = TELEMETRY_MAX_RECORDS};

	fill_queue();

	int ret = telemetry_queue_add(&record);

	zassert_equal(ret, 1);
}

/**
 * @brief The count should never go above TELEMETRY_MAX_RECORDS (16).
 *
 * Fills the queue and adds one more record. Since one record goes out and
 * one comes in the count should still be TELEMETRY_MAX_RECORDS (16).
 */
ZTEST(telemetry_queue_test, test_add_to_full_queue_keeps_count_at_max)
{
	const struct telemetry_record record = {.seq = TELEMETRY_MAX_RECORDS};

	fill_queue();

	zassert_equal(telemetry_queue_add(&record), 1);

	zassert_equal(telemetry_queue_count(), TELEMETRY_MAX_RECORDS);
}

/**
 * @brief When the queue is full it should discard the oldest.
 *
 * Fills the queue with seq 0 to 15 and adds one more record. Seq 0 is the oldest so
 * that is the one that should be gone which means the oldest record left
 * should be seq 1.
 */
ZTEST(telemetry_queue_test, test_add_to_full_queue_discards_oldest)
{
	const struct telemetry_record record = {.seq = TELEMETRY_MAX_RECORDS};
	struct telemetry_record oldest;

	fill_queue();

	zassert_equal(telemetry_queue_add(&record), 1);

	zassert_ok(telemetry_queue_peek(&oldest));
	zassert_equal(oldest.seq, 1U);
}

/**
 * @brief Overflowing many times in a row should keep only the newest records.
 *
 * Fills the queue with seq 0 to 15 and then keeps adding seq 16 to 31. Every
 * one of those adds should return 1 since the queue is full the whole time.
 * In the end only the last 16 records, seq 16 to 31, should be left, still
 * in order. This makes sure the queue keeps working after it has wrapped
 * around.
 */
ZTEST(telemetry_queue_test, test_add_repeated_overflow_keeps_newest)
{
	struct telemetry_record record = {0};

	fill_queue();

	for (uint8_t seq = TELEMETRY_MAX_RECORDS; seq < 2 * TELEMETRY_MAX_RECORDS; seq++) {
		record.seq = seq;
		zassert_equal(telemetry_queue_add(&record), 1);
	}

	remove_and_check_records(TELEMETRY_MAX_RECORDS, TELEMETRY_MAX_RECORDS);
}

/**
 * @brief After removing from a full queue there should be room again.
 *
 * Fills the queue, removes one record and adds a new one. Since
 * there is a free space now nothing should be thrown away so add should
 * return 0 and not 1. The queue should then hold seq 1 to 16.
 */
ZTEST(telemetry_queue_test, test_add_after_remove_from_full_queue_returns_0)
{
	const struct telemetry_record record = {.seq = TELEMETRY_MAX_RECORDS};

	fill_queue();
	zassert_ok(telemetry_queue_remove(0));

	zassert_ok(telemetry_queue_add(&record));

	remove_and_check_records(1, TELEMETRY_MAX_RECORDS);
}

/**
 * @brief Peek on a queue with something in it should return 0.
 *
 * Adds a record and peeks at it. Peek should return 0.
 */
ZTEST(telemetry_queue_test, test_peek_non_empty_queue_returns_0)
{
	const struct telemetry_record record = {.seq = 1};
	struct telemetry_record peeked;

	zassert_ok(telemetry_queue_add(&record));

	int ret = telemetry_queue_peek(&peeked);

	zassert_ok(ret);
}

/**
 * @brief Peek should give back an exact copy of the oldest record.
 *
 * Adds two records where the first one has a value in every field. Peek
 * should return the first one, since it is the oldest, and every field
 * should match what we put in. If a field is missing or wrong, peek doesn't work.
 */
ZTEST(telemetry_queue_test, test_peek_copies_oldest_record)
{
	const struct telemetry_record first = {
		.seq = 1,
		.uptime_ms = 1000,
		.temperature_mc = 1000,
		.humidity_mpct = 1000,
	};
	const struct telemetry_record second = {.seq = 2};
	struct telemetry_record peeked;

	zassert_ok(telemetry_queue_add(&first));
	zassert_ok(telemetry_queue_add(&second));

	zassert_ok(telemetry_queue_peek(&peeked));

	zassert_equal(peeked.seq, first.seq);
	zassert_equal(peeked.uptime_ms, first.uptime_ms);
	zassert_equal(peeked.temperature_mc, first.temperature_mc);
	zassert_equal(peeked.humidity_mpct, first.humidity_mpct);
}

/**
 * @brief Peek should only look, not take a record out.
 *
 * Adds a record and peeks at it. The record should still be in the queue
 * afterwards so the count should still be 1.
 */
ZTEST(telemetry_queue_test, test_peek_does_not_remove_record)
{
	const struct telemetry_record record = {.seq = 1};
	struct telemetry_record peeked;

	zassert_ok(telemetry_queue_add(&record));

	zassert_ok(telemetry_queue_peek(&peeked));

	zassert_equal(telemetry_queue_count(), 1U);
}

/**
 * @brief Peek on an empty queue should return -ENODATA.
 *
 * There is nothing in the queue to look at so peek should tell us that
 * with -ENODATA.
 */
ZTEST(telemetry_queue_test, test_peek_empty_queue_returns_enodata)
{
	struct telemetry_record peeked;

	int ret = telemetry_queue_peek(&peeked);

	zassert_equal(ret, -ENODATA);
}

/**
 * @brief Peek with NULL should fail with -EINVAL.
 *
 * Adds a record first so the queue is not empty. That way we know the
 * -EINVAL comes from the NULL pointer and not from the queue being empty.
 */
ZTEST(telemetry_queue_test, test_peek_null_returns_einval)
{
	const struct telemetry_record record = {.seq = 1};

	zassert_ok(telemetry_queue_add(&record));

	int ret = telemetry_queue_peek(NULL);

	zassert_equal(ret, -EINVAL);
}

/**
 * @brief Remove on a queue with something in it should return 0.
 *
 * Adds one record and removes it. Remove should return 0.
 */
ZTEST(telemetry_queue_test, test_remove_non_empty_queue_returns_0)
{
	const struct telemetry_record record = {.seq = 1};

	zassert_ok(telemetry_queue_add(&record));

	int ret = telemetry_queue_remove(record.seq);

	zassert_ok(ret);
}

/**
 * @brief Remove should make the count go down by one.
 *
 * Adds two records and removes one. The count should go from 2 to 1.
 */
ZTEST(telemetry_queue_test, test_remove_decrements_count)
{
	add_records(1, 2);

	zassert_ok(telemetry_queue_remove(1));

	zassert_equal(telemetry_queue_count(), 1U);
}

/**
 * @brief Remove should take out the oldest record.
 *
 * Adds seq 1 and seq 2 and removes one record. Seq 1 is the oldest so that
 * is the one that should be gone and seq 2 should now be first in line.
 */
ZTEST(telemetry_queue_test, test_remove_discards_oldest_record)
{
	struct telemetry_record oldest;

	add_records(1, 2);

	zassert_ok(telemetry_queue_remove(1));

	zassert_ok(telemetry_queue_peek(&oldest));
	zassert_equal(oldest.seq, 2U);
}

/**
 * @brief Remove on an empty queue should return -ENODATA.
 *
 * There is nothing to remove so remove should tell us that with -ENODATA.
 */
ZTEST(telemetry_queue_test, test_remove_empty_queue_returns_enodata)
{
	int ret = telemetry_queue_remove(0);

	zassert_equal(ret, -ENODATA);
}

/**
 * @brief Remove with another seq than the oldest record should return -ENOENT.
 *
 * Adds seq 1 and seq 2 and tries to remove seq 2. Seq 1 is the oldest so
 * the seq does not match and remove should refuse with -ENOENT.
 */
ZTEST(telemetry_queue_test, test_remove_wrong_seq_returns_enoent)
{
	add_records(1, 2);

	int ret = telemetry_queue_remove(2);

	zassert_equal(ret, -ENOENT);
}

/**
 * @brief Remove with the wrong seq should leave the queue as it was.
 *
 * Adds seq 1 and seq 2 and tries to remove seq 2. Nothing should be taken
 * out so both records should still be there in the same order.
 */
ZTEST(telemetry_queue_test, test_remove_wrong_seq_keeps_records)
{
	add_records(1, 2);

	zassert_equal(telemetry_queue_remove(2), -ENOENT);

	remove_and_check_records(1, 2);
}

/**
 * @brief A record discarded while it was published should not take another one with it.
 *
 * This is what happens when publishing is slow and the queue fills up in
 * the meantime:
 * 1. The publisher peeks the oldest record, seq 0, and starts publishing it.
 * 2. A new sample is added to the full queue, which discards seq 0.
 * 3. The publisher is done and removes seq 0.
 *
 * Seq 0 is already gone, so remove should return -ENOENT and seq 1, which
 * was never published, should still be the oldest record.
 */
ZTEST(telemetry_queue_test, test_remove_after_oldest_discarded_keeps_next_record)
{
	const struct telemetry_record new_record = {.seq = TELEMETRY_MAX_RECORDS};
	struct telemetry_record published;

	fill_queue();
	zassert_ok(telemetry_queue_peek(&published));

	zassert_equal(telemetry_queue_add(&new_record), 1);

	zassert_equal(telemetry_queue_remove(published.seq), -ENOENT);
	remove_and_check_records(1, TELEMETRY_MAX_RECORDS);
}
