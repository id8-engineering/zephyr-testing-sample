/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_LIB_TEMP_HUMIDITY_SENSOR_H_
#define APP_LIB_TEMP_HUMIDITY_SENSOR_H_

#include <stdint.h>

/**
 * @brief One temperature and relative humidity reading.
 */
struct temp_humidity_reading {
	/** Temperature in milli degrees Celsius (21500 = 21.5 C). */
	int32_t temperature_mc;

	/** Relative humidity in milli percent (45000 = 45.0 %RH). */
	uint32_t humidity_mpct;
};

/**
 * @brief Read temperature and relative humidity from the sensor.
 *
 * @param reading Reading to fill in. Must not be NULL.
 *
 * @retval 0 if the reading was filled in
 * @retval -EINVAL if @p reading is NULL
 * @retval -EIO if the sensor could not be read
 * @retval <0 other negative errno code on failure
 */
int temp_humidity_sensor_read(struct temp_humidity_reading *reading);

#endif /* APP_LIB_TEMP_HUMIDITY_SENSOR_H_ */
