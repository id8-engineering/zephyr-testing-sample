/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef TELEMETRY_APP_PUBLISHING_H_
#define TELEMETRY_APP_PUBLISHING_H_

/**
 * @brief Start publishing.
 *
 * Starts the publish thread. Must be called before
 * telemetry_publishing_trigger(). Call it once.
 */
void telemetry_publishing_start(void);

/**
 * @brief Tell the publish thread that there are records to publish.
 *
 * The publish thread then publishes the queued records. This function does
 * not wait for that to finish, so a slow publish never holds up the caller.
 */
void telemetry_publishing_trigger(void);

/**
 * @brief Publish the queued records, oldest first.
 *
 * Publishes the oldest record with telemetry_publish() and removes it from
 * the queue only after it was published. Repeats until the queue is empty.
 * If publishing fails, the record stays in the queue and publishing stops
 * until the next call.
 *
 * Runs on the publish thread. Exposed so that the tests can call it
 * directly, without the thread.
 *
 * @retval 0 if the queue is empty
 * @retval <0 negative errno code from telemetry_publish()
 */
int telemetry_publish_queued(void);

#endif /* TELEMETRY_APP_PUBLISHING_H_ */
