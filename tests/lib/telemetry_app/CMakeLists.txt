# SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(telemetry_app_test)
target_sources(app PRIVATE src/test_telemetry_app.c)

# sampling.h and publishing.h are private to the telemetry_app library, so
# they are not in include/. Unit tests use them directly.
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../../lib/telemetry_app)
