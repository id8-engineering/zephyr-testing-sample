/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/kernel.h>

#include <app/lib/telemetry_queue.h>

/**
 * @brief Message queue holding the buffered telemetry records.
 *
 * Defined with K_MSGQ_DEFINE_STATIC_TYPE, which statically allocates
 * the ring buffer in RAM and keeps the queue private to
 * this file. All access goes through the telemetry_queue.h interface.
 *
 */
K_MSGQ_DEFINE_STATIC_TYPE(record_msgq, struct telemetry_record, TELEMETRY_MAX_RECORDS);

/**
 * @brief Lock held by every function that changes the queue.
 *
 * Every k_msgq call is thread safe on its own, but discarding the oldest
 * record in telemetry_queue_add() takes several calls. The lock keeps another
 * thread from changing the queue in between. telemetry_queue_reset() and
 * telemetry_queue_remove() take it too, so they never run in the middle of
 * an add.
 */
static K_MUTEX_DEFINE(queue_lock);

void telemetry_queue_reset(void)
{
	k_mutex_lock(&queue_lock, K_FOREVER);
	k_msgq_purge(&record_msgq);
	k_mutex_unlock(&queue_lock);
}

int telemetry_queue_add(const struct telemetry_record *src)
{
	struct telemetry_record oldest_record;
	int ret = 0;

	if (src == NULL) {
		return -EINVAL;
	}

	k_mutex_lock(&queue_lock, K_FOREVER);

	/* Queue is full: drop the oldest record and try again. */
	while (k_msgq_put(&record_msgq, src, K_NO_WAIT) != 0) {
		k_msgq_get(&record_msgq, &oldest_record, K_NO_WAIT);
		ret = 1;
	}

	k_mutex_unlock(&queue_lock);

	return ret;
}

/* k_msgq_peek() uses a spinlock while it copies the oldest record, so no other
 * thread or interrupt can change the queue during the copy.
 */
int telemetry_queue_peek(struct telemetry_record *dst)
{
	if (dst == NULL) {
		return -EINVAL;
	}

	if (k_msgq_peek(&record_msgq, dst) != 0) {
		return -ENODATA;
	}

	return 0;
}

int telemetry_queue_remove(void)
{
	struct telemetry_record oldest_record;
	int ret = 0;

	k_mutex_lock(&queue_lock, K_FOREVER);

	if (k_msgq_get(&record_msgq, &oldest_record, K_NO_WAIT) != 0) {
		ret = -ENODATA;
	}

	k_mutex_unlock(&queue_lock);

	return ret;
}

uint32_t telemetry_queue_count(void)
{
	return k_msgq_num_used_get(&record_msgq);
}
