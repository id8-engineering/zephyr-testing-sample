# SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
# SPDX-License-Identifier: Apache-2.0

add_subdirectory_ifdef(CONFIG_TELEMETRY_PUBLISHER_LOG publisher)
add_subdirectory_ifdef(CONFIG_ENVIRONMENT_FAKE environment)
