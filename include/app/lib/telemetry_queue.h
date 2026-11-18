/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_LIB_TELEMETRY_QUEUE_H_
#define APP_LIB_TELEMETRY_QUEUE_H_

#include <stdint.h>
#include <app/lib/telemetry.h>

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
 * @param dst Record to populate with the oldest record. Must not be NULL.
 *
 * @retval 0 if @p dst was populated
 * @retval -ENODATA if the queue is empty
 * @retval -EINVAL if @p dst is NULL
 */
int telemetry_queue_peek(struct telemetry_record *dst);

/**
 * @brief Remove the oldest record.
 *
 * @retval 0 if the oldest record was removed
 * @retval -ENODATA if the queue is empty
 */
int telemetry_queue_remove(void);

/**
 * @brief Get the number of records currently in the queue.
 *
 * @return Number of records.
 */
uint32_t telemetry_queue_count(void);

#endif /* APP_LIB_TELEMETRY_QUEUE_H_ */
