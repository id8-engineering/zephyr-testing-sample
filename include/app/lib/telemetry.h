/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_LIB_TELEMETRY_H_
#define APP_LIB_TELEMETRY_H_

#include <stdint.h>

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

#endif /* APP_LIB_TELEMETRY_H_ */
