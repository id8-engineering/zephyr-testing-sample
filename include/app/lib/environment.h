/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_LIB_ENVIRONMENT_H_
#define APP_LIB_ENVIRONMENT_H_

#include <stdint.h>

/**
 * @brief One environment reading.
 */
struct environment_reading {
	/** Temperature in milli degrees Celsius (21500 = 21.5 C). */
	int32_t temperature_mc;

	/** Relative humidity in milli percent (45000 = 45.0 %RH). */
	uint32_t humidity_mpct;
};

/**
 * @brief Read the current environment values.
 *
 * @param dst Reading to populate. Must not be NULL.
 *
 * @retval 0 if @p dst was populated
 * @retval -EINVAL if @p dst is NULL
 * @retval -EIO if the environment could not be read
 * @retval <0 other negative errno code on failure
 */
int environment_read(struct environment_reading *dst);

#endif /* APP_LIB_ENVIRONMENT_H_ */
