/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_LIB_TELEMETRY_QUEUE_H_
#define APP_LIB_TELEMETRY_QUEUE_H_

#include <stdint.h>
#include <app/lib/telemetry.h>

/**
 * @brief Maximum number of records the queue can hold.
 *
 * When the queue is full, adding a record discards the oldest one.
 */
#define TELEMETRY_MAX_RECORDS 16

/**
 * @brief Remove all records from the queue.
 */
void telemetry_queue_reset(void);

/**
 * @brief Add a record to the end of the queue.
 *
 * The record is copied into the queue. If the queue is full, the
 * oldest record is discarded.
 *
 * @param src Record to add. Must not be NULL.
 *
 * @retval 0 if @p src was added
 * @retval 1 if @p src was added and the oldest record was discarded
 * @retval -EINVAL if @p src is NULL
 */
int telemetry_queue_add(const struct telemetry_record *src);

/**
 * @brief Copy the oldest record without removing it.
 *
 * Create a struct telemetry_record and pass its address. The oldest record
 * is copied into it and stays in the queue. Use the copy, for example to
 * publish it, and then remove the record with telemetry_queue_remove(), using
 * the sequence number of the copy.
 *
 * @param dst Struct to copy the oldest record into. Must not be NULL.
 *
 * @retval 0 if the oldest record was copied to @p dst
 * @retval -ENODATA if the queue is empty
 * @retval -EINVAL if @p dst is NULL
 */
int telemetry_queue_peek(struct telemetry_record *dst);

/**
 * @brief Remove the oldest record if it has the given sequence number.
 *
 * Pass the sequence number of the record returned by telemetry_queue_peek().
 * The record is removed only if it is still the oldest one. If
 * telemetry_queue_add() discarded it in the meantime because the queue was
 * full, nothing is removed, so a record that was never read is not lost.
 *
 * @param seq Sequence number of the record to remove.
 *
 * @retval 0 if the oldest record was removed
 * @retval -ENODATA if the queue is empty
 * @retval -ENOENT if the oldest record has another sequence number. Nothing
 *         is removed.
 */
int telemetry_queue_remove(uint32_t seq);

/**
 * @brief Get the number of records currently in the queue.
 *
 * @return Number of records.
 */
uint32_t telemetry_queue_count(void);

#endif /* APP_LIB_TELEMETRY_QUEUE_H_ */
