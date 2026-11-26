/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TELEMETRY_APP_SAMPLING_H_
#define TELEMETRY_APP_SAMPLING_H_

/**
 * @brief Start sampling.
 *
 * Starts the sample thread, takes a sample now and then one every
 * CONFIG_TELEMETRY_APP_SAMPLE_INTERVAL_MS. Starts a publish with
 * telemetry_publishing_trigger() after every sample. Call it once.
 */
void telemetry_sampling_start(void);

/**
 * @brief Take a reading and add the resulting record to the queue.
 *
 * Creates a telemetry record with telemetry_record_create() and adds it to
 * the end of the telemetry queue. If the reading fails, no record is added.
 * If the queue is full, the oldest record is discarded and a warning is
 * logged.
 *
 * Runs on the sample thread. Exposed so that the tests can call it
 * directly, without the thread or the timer.
 *
 * @retval 0 if a record was added to the queue
 * @retval <0 negative errno code from telemetry_record_create()
 */
int telemetry_take_sample(void);

#endif /* TELEMETRY_APP_SAMPLING_H_ */
