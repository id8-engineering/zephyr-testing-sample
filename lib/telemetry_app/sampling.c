/*
 * SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Sampling: takes a sample every sampling interval on a thread of its own.
 *
 * sample_timer expires every CONFIG_TELEMETRY_APP_SAMPLE_INTERVAL_MS and
 * calls sample_timer_handler() in interrupt context. That only submits
 * sample_work to sample_workq. The sample thread then reads the sensor and
 * adds the record to the queue with telemetry_take_sample(), and
 * calls telemetry_publishing_trigger() to start a publish. The sample thread
 * has a higher priority than the publish thread, so a publish that is in
 * progress or blocks does not hold up sampling.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <app/lib/telemetry.h>
#include <app/lib/telemetry_queue.h>

#include "publishing.h"
#include "sampling.h"

LOG_MODULE_DECLARE(telemetry_app);

/* Stack and priority of the sample thread. The priority is higher (a lower
 * number) than the publish thread, so a publish does not hold up sampling.
 */
#define SAMPLE_WORKQ_STACK_SIZE 1024
#define SAMPLE_WORKQ_PRIORITY   5

static void sample_timer_handler(struct k_timer *timer);
static void sample_work_handler(struct k_work *work);

/* Sample thread and its stack. Started by telemetry_sampling_start(). */
static struct k_work_q sample_workq;
static K_THREAD_STACK_DEFINE(sample_workq_stack, SAMPLE_WORKQ_STACK_SIZE);

/* Takes a sample. Runs on sample_workq thread. */
static K_WORK_DEFINE(sample_work, sample_work_handler);

/* Starts a sample every sampling interval. */
static K_TIMER_DEFINE(sample_timer, sample_timer_handler, NULL);

/**
 * @brief Start a sample.
 *
 * Called by sample_timer every sampling interval. It runs in interrupt
 * context, so it only hands the sample over to sample_workq.
 */
static void sample_timer_handler(struct k_timer *timer)
{
	ARG_UNUSED(timer);

	k_work_submit_to_queue(&sample_workq, &sample_work);
}

/**
 * @brief Take a sample and start a publish.
 *
 * Runs on sample_workq thread on every sampling interval.
 */
static void sample_work_handler(struct k_work *work)
{
	ARG_UNUSED(work);

	/* Read the sensor and add a record to the queue. If the reading
	 * fails, no record is added.
	 */
	int err = telemetry_take_sample();
	if (err < 0) {
		LOG_WRN("Sample failed (%d)", err);
	}

	/* Start a publish even if the reading failed, so that records left
	 * from an earlier failed publish are tried again.
	 */
	telemetry_publishing_trigger();
}

int telemetry_take_sample(void)
{
	struct telemetry_record record;
	int err;

	err = telemetry_record_create(&record);
	if (err < 0) {
		return err;
	}

	err = telemetry_queue_add(&record);
	if (err < 0) {
		return err;
	}

	if (err > 0) {
		LOG_WRN("Queue full, discarded the oldest record");
	}

	return 0;
}

void telemetry_sampling_start(void)
{
	k_work_queue_start(&sample_workq, sample_workq_stack,
			   K_THREAD_STACK_SIZEOF(sample_workq_stack), SAMPLE_WORKQ_PRIORITY, NULL);

	/* Take the first sample now and then one every sampling interval. */
	k_timer_start(&sample_timer, K_NO_WAIT, K_MSEC(CONFIG_TELEMETRY_APP_SAMPLE_INTERVAL_MS));
}
