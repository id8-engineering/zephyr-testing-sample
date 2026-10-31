/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app/lib/environment.h>
#include <app/lib/publisher.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
	struct environment_reading reading;

	int error = environment_read(&reading);
	if (error < 0) {
		LOG_ERR("Failed to read environment: %d", error);
		return 0;
	}

	struct telemetry_record record = {
		.seq = 0,
		.uptime_ms = k_uptime_get(),
		.temperature_mc = reading.temperature_mc,
		.humidity_mpct = reading.humidity_mpct,
	};

	error = telemetry_publish(&record);
	if (error < 0) {
		LOG_ERR("Failed to publish telemetry record: %d", error);
	}

	return 0;
}
