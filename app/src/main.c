/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app/lib/publisher.h>
#include <app/lib/telemetry.h>

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

#define TESTTIME 5

int main(void)
{
	struct telemetry_record record;
	int error;

	while (true) {
		error = telemetry_record_create(&record);
		if (error < 0) {
			LOG_ERR("Failed to create telemetry record: %d", error);
		} else {
			error = telemetry_publish(&record);
			if (error < 0) {
				LOG_ERR("Failed to publish telemetry record: %d", error);
			}
		}

		k_sleep(K_SECONDS(TESTTIME));
	}

	return 0;
}
