/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>

#include <app/lib/telemetry_app.h>

#include "publishing.h"
#include "sampling.h"

LOG_MODULE_REGISTER(telemetry_app, LOG_LEVEL_INF);

void telemetry_app_start(void)
{
	/* Publishing first since every sample starts a publish. */
	telemetry_publishing_start();
	telemetry_sampling_start();
}
