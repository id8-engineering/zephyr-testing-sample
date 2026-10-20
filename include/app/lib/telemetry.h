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

#endif /* APP_LIB_TELEMETRY_H_ */
