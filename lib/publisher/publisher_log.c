/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <inttypes.h>
#include <stddef.h>

#include <zephyr/logging/log.h>

#include <app/lib/publisher.h>

LOG_MODULE_REGISTER(publisher_log, LOG_LEVEL_INF);

int telemetry_publish(const struct telemetry_record *record)
{
	if (record == NULL) {
		return -EINVAL;
	}

	LOG_INF("seq=%u uptime_ms=%" PRId64 " temperature_mc=%d humidity_mpct=%u",
		record->seq, record->uptime_ms,
		record->temperature_mc, record->humidity_mpct);

	return 0;
}
