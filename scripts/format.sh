#!/usr/bin/env bash
# SPDX-FileCopyrightText: Copyright 2026 ID8 Engineering AB
# SPDX-License-Identifier: Apache-2.0
#
# Usage: scripts/format.sh          format all C sources in place
#        scripts/format.sh --check  only check, exit non-zero on diff

set -euo pipefail
cd "$(git rev-parse --show-toplevel)"

if [[ "${1:-}" == "--check" ]]; then
	git ls-files '*.c' '*.h' | xargs clang-format --dry-run --Werror
else
	git ls-files '*.c' '*.h' | xargs clang-format -i
fi
