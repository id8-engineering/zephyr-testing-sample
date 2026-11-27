/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_LIB_TELEMETRY_APP_H_
#define APP_LIB_TELEMETRY_APP_H_

/**
 * @brief Start sampling and publishing.
 *
 * Start the application and take one sample directly when called, then one every
 * CONFIG_TELEMETRY_APP_SAMPLE_INTERVAL_MS. Call it once.
 */
void telemetry_app_start(void);

#endif /* APP_LIB_TELEMETRY_APP_H_ */
