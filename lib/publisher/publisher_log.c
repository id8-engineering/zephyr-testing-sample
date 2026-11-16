/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>

#include <app/lib/publisher.h>

LOG_MODULE_REGISTER(publisher_log, LOG_LEVEL_INF);

int telemetry_publish(const struct telemetry_record *record)
{
	char buffer[TELEMETRY_JSON_MAX_LEN];
	int err;

	err = telemetry_encode_record(record, buffer, sizeof(buffer));
	if (err < 0) {
		return err;
	}

	LOG_INF("%s", buffer);

	return 0;
}
