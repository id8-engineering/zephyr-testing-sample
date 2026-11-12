# SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(telemetry_test)
target_sources(app PRIVATE src/test_telemetry.c)
