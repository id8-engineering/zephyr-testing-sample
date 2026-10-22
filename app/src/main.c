/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app/lib/publisher.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
	struct telemetry_record record = {
		.seq = 0,
		.uptime_ms = k_uptime_get(),
		.temperature_mc = 21500,
		.humidity_mpct = 45000,
	};

	int error = telemetry_publish(&record);
	if (error < 0) {
		LOG_ERR("Failed to publish telemetry record: %d", error);
	}

	return 0;
}
