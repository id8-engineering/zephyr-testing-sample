/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>

#include <zephyr/kernel.h>

#include <app/lib/environment.h>
#include <app/lib/telemetry.h>

#define TELEMETRY_SEQ_INITIAL 0U

static uint32_t next_seq = TELEMETRY_SEQ_INITIAL;

int telemetry_record_create(struct telemetry_record *dst)
{
	struct environment_reading reading;
	int err;

	if (dst == NULL) {
		return -EINVAL;
	}

	err = environment_read(&reading);
	if (err < 0) {
		return err;
	}

	dst->seq = next_seq++;
	dst->uptime_ms = k_uptime_get();
	dst->temperature_mc = reading.temperature_mc;
	dst->humidity_mpct = reading.humidity_mpct;

	return 0;
}

void telemetry_reset(void)
{
	next_seq = TELEMETRY_SEQ_INITIAL;
}
