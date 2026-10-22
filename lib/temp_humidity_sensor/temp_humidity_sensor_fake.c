/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>

#include <zephyr/sys/util.h>

#include <app/lib/temp_humidity_sensor.h>

int temp_humidity_sensor_read(struct temp_humidity_reading *reading)
{
	if (reading == NULL) {
		return -EINVAL;
	}

	if (IS_ENABLED(CONFIG_TEMP_HUMIDITY_SENSOR_FAKE_FAIL)) {
		return -EIO;
	}

	reading->temperature_mc = CONFIG_TEMP_HUMIDITY_SENSOR_FAKE_TEMPERATURE_MC;
	reading->humidity_mpct = CONFIG_TEMP_HUMIDITY_SENSOR_FAKE_HUMIDITY_MPCT;

	return 0;
}
