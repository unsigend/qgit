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
ROOT_DIR      := $(abspath $(CUR_DIR)/..)
CONFIG_PATH   := $(ROOT_DIR)/config
BUILD_PATH    := $(ROOT_DIR)/build
BIN_PATH      := $(BUILD_PATH)/bin
LIB_PATH      := $(BUILD_PATH)/lib
INCLUDE_PATH  := $(ROOT_DIR)/include
EXTERNAL_PATH := $(CUR_DIR)/external
UNIT_PATH     := $(CUR_DIR)/unit
UNIT_BUILD    := $(CUR_DIR)/build
UNIT_OBJ      := $(UNIT_BUILD)/obj
UNIT_DEP      := $(UNIT_BUILD)/dep

include $(CONFIG_PATH)/config.mk

BATS       ?= bats
CMD_TESTS  := $(CUR_DIR)/cmd
QGIT       := $(abspath $(BIN_PATH)/$(BIN_NAME))
GIT        ?= git

HOST_OS := $(shell uname -s)

ifeq ($(LIB_BUILD),shared)
ifeq ($(HOST_OS),Darwin)
LIB_ARTIFACT := $(LIB_PATH)/lib$(LIB_NAME).dylib
else
LIB_ARTIFACT := $(LIB_PATH)/lib$(LIB_NAME).so
endif
else
LIB_ARTIFACT := $(LIB_PATH)/lib$(LIB_NAME).a
endif

ifeq ($(HOST_OS),Darwin)
OPENSSL_PREFIX ?= $(shell \
	if [ -d /opt/homebrew/opt/openssl ]; then \
		echo /opt/homebrew/opt/openssl; \
	elif [ -d /usr/local/opt/openssl ]; then \
		echo /usr/local/opt/openssl; \
	else \
		echo ""; \
	fi)
ifneq ($(strip $(OPENSSL_PREFIX)),)
UNIT_CC_FLAGS += -I$(OPENSSL_PREFIX)/include
UNIT_LDFLAGS += -L$(OPENSSL_PREFIX)/lib
endif
endif

UNIT_CC_FLAGS += -std=c11 -Wall -Wextra -Werror -Wshadow
UNIT_CC_FLAGS += -I$(INCLUDE_PATH)
UNIT_CC_FLAGS += -I$(EXTERNAL_PATH)/includes
UNIT_CC_FLAGS += $(addprefix -I,$(shell find $(UNIT_PATH) -type d 2>/dev/null))
ifeq ($(HOST_OS), Linux)
UNIT_CC_FLAGS += -D_XOPEN_SOURCE=700
endif
ifeq ($(DEBUG), 1)
UNIT_CC_FLAGS += -g -O0
UNIT_CC_FLAGS += -fsanitize=address,undefined
UNIT_LDFLAGS += -fsanitize=address,undefined
else
UNIT_CC_FLAGS += -O2
endif

UNIT_LD_FLAGS := -L$(LIB_PATH) -l$(LIB_NAME)
UNIT_LD_FLAGS += -lssl -lcrypto -lz
UNIT_LD_FLAGS += $(UNIT_LDFLAGS)

ifeq ($(LIB_BUILD),shared)
UNIT_LD_FLAGS += -Wl,-rpath,$(abspath $(LIB_PATH))
endif

SRCS_UNIT := $(shell find $(UNIT_PATH) -name "*.c" 2>/dev/null)
OBJS_UNIT := $(patsubst $(UNIT_PATH)/%.c,$(UNIT_OBJ)/%.o,$(SRCS_UNIT))
DEPS_UNIT := $(patsubst $(UNIT_OBJ)/%.o,$(UNIT_DEP)/%.d,$(OBJS_UNIT))
SRCS_EXT  := $(shell find $(EXTERNAL_PATH)/src -name "*.c" 2>/dev/null)
OBJS_EXT  := $(patsubst $(EXTERNAL_PATH)/src/%.c,$(UNIT_OBJ)/external/%.o,$(SRCS_EXT))
DEPS_EXT  := $(patsubst $(UNIT_OBJ)/external/%.o,$(UNIT_DEP)/external/%.d,$(OBJS_EXT))
OBJS_ALL  := $(OBJS_UNIT) $(OBJS_EXT)
DEPS_ALL  := $(DEPS_UNIT) $(DEPS_EXT)

export QGIT GIT

.DEFAULT_GOAL := test

.PHONY: test test-% unit unit-all clean

$(UNIT_OBJ)/external/%.o: $(EXTERNAL_PATH)/src/%.c
	@mkdir -p $(dir $@) $(dir $(UNIT_DEP)/external/$*.d)
	@$(CC) $(UNIT_CC_FLAGS) -MMD -MP -MF $(UNIT_DEP)/external/$*.d -MT $@ -c $< -o $@

$(UNIT_OBJ)/%.o: $(UNIT_PATH)/%.c
	@mkdir -p $(dir $@) $(dir $(UNIT_DEP)/$*.d)
	@$(CC) $(UNIT_CC_FLAGS) -MMD -MP -MF $(UNIT_DEP)/$*.d -MT $@ -c $< -o $@

-include $(DEPS_ALL)

.SECONDARY: $(DEPS_ALL)

$(UNIT_BUILD)/test: $(OBJS_ALL) $(LIB_ARTIFACT)
	@mkdir -p $(dir $@)
	@$(CC) $(UNIT_CC_FLAGS) -o $@ $(OBJS_ALL) $(UNIT_LD_FLAGS)
	@echo "  + LD	$@"

test:
	@$(MAKE) -C $(ROOT_DIR) bin
	@$(BATS) $(CMD_TESTS)

test-%:
	@$(MAKE) -C $(ROOT_DIR) bin
	@$(BATS) $(CMD_TESTS)/$*.bats

unit: $(UNIT_BUILD)/test
	@$(UNIT_BUILD)/test

unit-%: $(UNIT_BUILD)/test
	@$(UNIT_BUILD)/test $*

clean:
	@rm -rf $(UNIT_BUILD)