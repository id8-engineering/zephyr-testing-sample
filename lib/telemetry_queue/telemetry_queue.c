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

void telemetry_queue_reset(void)
{
	k_msgq_purge(&record_msgq);
}

int telemetry_queue_add(const struct telemetry_record *src)
{
	struct telemetry_record oldest_record;
	int ret = 0;

	if (src == NULL) {
		return -EINVAL;
	}

	/* Queue is full: drop the oldest record and try again. */
	while (k_msgq_put(&record_msgq, src, K_NO_WAIT) != 0) {
		k_msgq_get(&record_msgq, &oldest_record, K_NO_WAIT);
		ret = 1;
	}

	return ret;
}

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

	if (k_msgq_get(&record_msgq, &oldest_record, K_NO_WAIT) != 0) {
		return -ENODATA;
	}

	return 0;
}

uint32_t telemetry_queue_count(void)
{
	return k_msgq_num_used_get(&record_msgq);
}
