/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_LIB_TELEMETRY_H_
#define APP_LIB_TELEMETRY_H_

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Buffer size that fits any JSON encoded telemetry record.
 *
 * The longest possible record is 107 characters + the NUL terminator.
 */
#define TELEMETRY_JSON_MAX_LEN 128

/**
 * @brief One telemetry record.
 */
struct telemetry_record {
	/** Sequence number, incremented for every record. */
	uint32_t seq;

	/** System uptime when the record was created, in milliseconds. */
	int64_t uptime_ms;

	/** Temperature in milli degrees Celsius (21500 = 21.5 °C). */
	int32_t temperature_mc;

	/** Relative humidity in milli percent (45000 = 45.0 %RH). */
	uint32_t humidity_mpct;
};

/**
 * @brief Read the environment and create a telemetry record.
 *
 * Reads temperature and humidity through environment_read() and adds a
 * sequence number and the current system uptime.
 *
 * The first record after boot or telemetry_reset() has sequence number 0.
 * The sequence number is incremented only when a record is created. A
 * failed environment read does not consume a sequence number.
 *
 * @param dst Record to populate. Must not be NULL.
 *
 * @retval 0 if @p dst was populated
 * @retval -EINVAL if @p dst is NULL
 * @retval <0 negative errno code from environment_read();
 */
int telemetry_record_create(struct telemetry_record *dst);

/**
 * @brief Reset the sequence number to 0.
 */
void telemetry_reset(void);

/**
 * @brief Encode a telemetry record as JSON.
 *
 * Writes JSON object to a caller provided buffer. No memory
 * is allocated dynamically. The field names are the struct member names and
 * the values use the units of the record:
 *
 * @param record Record to encode. Must not be NULL.
 * @param buffer Buffer to write the JSON string to. Must not be NULL.
 * @param buffer_size Size of @p buffer in bytes including the NULL
 *                    terminator. Use TELEMETRY_JSON_MAX_LEN to fit any record.
 *
 * @retval 0 if @p buffer contains the encoded record
 * @retval -EINVAL if @p record or @p buffer is NULL, or @p buffer_size is 0
 * @retval -ENOMEM if @p buffer is too small. @p buffer then contains an
 *         empty string.
 */
int telemetry_encode_record(const struct telemetry_record *record, char *buffer, size_t buffer_size);

#endif /* APP_LIB_TELEMETRY_H_ */
