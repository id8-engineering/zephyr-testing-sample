/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_LIB_PUBLISHER_H_
#define APP_LIB_PUBLISHER_H_

#include <app/lib/telemetry.h>

/**
 * @brief Publish a telemetry record.
 *
 * @param record Record to publish. Must not be NULL.
 *
 * @retval 0 if the record was published
 * @retval -EINVAL if @p record is NULL
 * @retval <0 other negative errno code on failure
 */
int telemetry_publish(const struct telemetry_record *record);

#endif /* APP_LIB_PUBLISHER_H_ */
