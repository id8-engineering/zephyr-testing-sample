/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Publishing: publishes the queued records on a thread of its own.
 *
 * The sample thread calls telemetry_publishing_trigger() after every sample.
 * That submits publish_work to publish_workq, and the publish thread then
 * publishes every queued record, oldest first, with
 * telemetry_publish_queued(). The publish thread has a lower
 * priority than the sample thread, so a publish that blocks does not delay
 * sampling.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app/lib/publisher.h>
#include <app/lib/telemetry_queue.h>

#include "publishing.h"

LOG_MODULE_DECLARE(telemetry_app);

/* Stack and priority of the publish thread. The priority is lower than the
 * sample thread, so a publish that blocks does not delay sampling.
 */
#define PUBLISH_WORKQ_STACK_SIZE 1024
#define PUBLISH_WORKQ_PRIORITY   7

static void publish_work_handler(struct k_work *work);

/* Publish thread and its stack. Started by telemetry_publishing_start(). */
static struct k_work_q publish_workq;
static K_THREAD_STACK_DEFINE(publish_workq_stack, PUBLISH_WORKQ_STACK_SIZE);

/* Publishes the queued records. Runs on publish_workq thread. */
static K_WORK_DEFINE(publish_work, publish_work_handler);

/**
 * @brief Publish the queued records, oldest first.
 *
 * Runs on the publish_workq thread each time telemetry_publishing_trigger()
 * is called.
 *
 * If a publish fails, the record stays in the queue and publishing stops.
 * The record is tried again the next time this handler runs.
 */
static void publish_work_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	int err = telemetry_publish_queued();
	if (err < 0) {
		LOG_WRN("Publish failed (%d), record kept for the next try", err);
	}
}

int telemetry_publish_queued(void)
{
	struct telemetry_record record;
	int err;

	while (telemetry_queue_peek(&record) == 0) {
		err = telemetry_publish(&record);
		if (err < 0) {
			return err;
		}

		/* Fails with -ENOENT if the record was discarded while it was
		 * published. The next record is then still in the queue.
		 */
		(void)telemetry_queue_remove(record.seq);
	}

	return 0;
}

void telemetry_publishing_start(void)
{
	k_work_queue_start(&publish_workq, publish_workq_stack,
			   K_THREAD_STACK_SIZEOF(publish_workq_stack), PUBLISH_WORKQ_PRIORITY,
			   NULL);
}

void telemetry_publishing_trigger(void)
{
	/* Does nothing if a publish is already waiting. That publish also
	 * sends the new record.
	 */
	k_work_submit_to_queue(&publish_workq, &publish_work);
}
