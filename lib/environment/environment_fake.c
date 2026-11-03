/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>

#include <app/lib/environment.h>

int environment_read(struct environment_reading *dst)
{
	if (dst == NULL) {
		return -EINVAL;
	}

	dst->temperature_mc = CONFIG_ENVIRONMENT_FAKE_TEMPERATURE_MC;
	dst->humidity_mpct = CONFIG_ENVIRONMENT_FAKE_HUMIDITY_MPCT;

	return 0;
}
