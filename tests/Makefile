# qgit - A simplified git like version control system
# Copyright (C) 2025 - 2026 Qiu Yixiang
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

CUR_DIR       := .
ROOT_DIR      := $(CUR_DIR)/..
CONFIG_PATH   := $(ROOT_DIR)/config
BUILD_PATH    := $(ROOT_DIR)/build
BIN_PATH      := $(BUILD_PATH)/bin

include $(CONFIG_PATH)/config.mk

BATS       ?= bats
CMD_TESTS  := $(CUR_DIR)/cmd
QGIT       := $(abspath $(BIN_PATH)/$(BIN_NAME))
GIT        ?= git

export QGIT GIT

.DEFAULT_GOAL := test

.PHONY: test test-%

test:
	@$(MAKE) -C $(ROOT_DIR) bin
	@$(BATS) $(CMD_TESTS)

test-%:
	@$(MAKE) -C $(ROOT_DIR) bin
	@$(BATS) $(CMD_TESTS)/$*.bats

