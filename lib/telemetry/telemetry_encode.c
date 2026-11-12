/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stddef.h>

#include <zephyr/data/json.h>
#include <zephyr/sys/util.h>

#include <app/lib/telemetry.h>

static const struct json_obj_descr telemetry_descr[] = {
	JSON_OBJ_DESCR_PRIM(struct telemetry_record, seq, JSON_TOK_UINT),
	JSON_OBJ_DESCR_PRIM(struct telemetry_record, uptime_ms, JSON_TOK_INT64),
	JSON_OBJ_DESCR_PRIM(struct telemetry_record, temperature_mc, JSON_TOK_INT),
	JSON_OBJ_DESCR_PRIM(struct telemetry_record, humidity_mpct, JSON_TOK_UINT),
};

int telemetry_encode_record(const struct telemetry_record *record, char *buffer, size_t buffer_size)
{

	if (record == NULL || buffer == NULL || buffer_size == 0) {
		return -EINVAL;
	}

	int err = json_obj_encode_buf(telemetry_descr, ARRAY_SIZE(telemetry_descr), record, buffer, buffer_size);
	if (err < 0) {
		return err;
	}

	return 0;
}
