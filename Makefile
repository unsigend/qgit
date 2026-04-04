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
SRC_PATH      := $(CUR_DIR)/src
INCLUDE_PATH  := $(CUR_DIR)/include
CONFIG_PATH   := $(CUR_DIR)/config
BUILD_PATH    := $(CUR_DIR)/build
OBJ_PATH      := $(BUILD_PATH)/obj
DEP_PATH      := $(BUILD_PATH)/dep
BIN_PATH      := $(BUILD_PATH)/bin
LIB_PATH      := $(BUILD_PATH)/lib

include $(CONFIG_PATH)/config.mk

ifeq ($(filter $(LIB_BUILD),static shared),)
$(error LIB_BUILD must be static or shared (got $(LIB_BUILD)))
endif

HOST_OS := $(shell uname -s)
SRCS := $(shell find $(SRC_PATH) -name "*.c" 2>/dev/null)
OBJS := $(patsubst $(SRC_PATH)/%.c, $(OBJ_PATH)/%.o, $(SRCS))
DEPS := $(patsubst $(SRC_PATH)/%.c, $(DEP_PATH)/%.d, $(SRCS))
MAIN_OBJ := $(OBJ_PATH)/main.o
LIB_OBJS := $(filter-out $(MAIN_OBJ),$(OBJS))

ifeq ($(LIB_BUILD),shared)
ifeq ($(HOST_OS),Darwin)
LIB_ARTIFACT := $(LIB_PATH)/lib$(LIB_NAME).dylib
else
LIB_ARTIFACT := $(LIB_PATH)/lib$(LIB_NAME).so
endif
else
LIB_ARTIFACT := $(LIB_PATH)/lib$(LIB_NAME).a
endif

CC_FLAGS := -std=c11 -Wall -Wextra -Werror -Wshadow
CC_FLAGS += -I$(INCLUDE_PATH)
ifeq ($(HOST_OS), Linux)
CC_FLAGS += -D_POSIX_C_SOURCE=200809L
endif
ifeq ($(LIB_BUILD),shared)
CC_FLAGS += -fPIC
endif
ifeq ($(DEBUG), 1)
CC_FLAGS += -g -O0
CC_FLAGS += -fsanitize=address,undefined
else
CC_FLAGS += -O2
endif

AR_FLAGS := -rcs
CC_DEP := -MMD -MP -MF

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(DEP_PATH)/$*.d)
	@$(CC) $(CC_FLAGS) $(CC_DEP) $(DEP_PATH)/$*.d -MT $@ -c $< -o $@
	@echo "  + CC	$<"

-include $(DEPS)

.DEFAULT_GOAL := help
.PHONY: all bin lib clean help create_build_dir list info clang format docker

create_build_dir:
	@mkdir -p $(OBJ_PATH)
	@mkdir -p $(DEP_PATH)
	@mkdir -p $(BIN_PATH)
	@mkdir -p $(LIB_PATH)

ifeq ($(strip $(SRCS)),)
$(BIN_PATH)/$(BIN_NAME): create_build_dir
	@echo "Error: no .c files under $(SRC_PATH)"
	@exit 1
else
$(BIN_PATH)/$(BIN_NAME): create_build_dir $(OBJS)
	@$(CC) $(CC_FLAGS) -o $@ $(OBJS)
	@echo "  + LD	$@"
endif

ifeq ($(strip $(LIB_OBJS)),)
$(LIB_ARTIFACT): create_build_dir
	@echo "Error: no library objects (need at least one .c besides main.c under $(SRC_PATH))"
	@exit 1
else
$(LIB_ARTIFACT): create_build_dir $(LIB_OBJS)
ifeq ($(LIB_BUILD),static)
	@$(AR) $(AR_FLAGS) $@ $(LIB_OBJS)
	@echo "  + AR	$@"
else
	@$(LD) -shared -o $@ $(LIB_OBJS)
	@echo "  + LD	$@"
endif
endif

bin: $(BIN_PATH)/$(BIN_NAME)

lib: $(LIB_ARTIFACT)

all: bin lib

clean:
	@rm -rf $(BUILD_PATH)

list:
	@echo "Sources:"
	@echo $(SRCS) | tr ' ' '\n' | sed 's/^/  /'
	@echo "Total: $(words $(SRCS)) files"
	@echo "Library objects (excluding main.o):"
	@echo $(LIB_OBJS) | tr ' ' '\n' | sed 's/^/  /'

info:
	@echo "Build configuration"
	@echo "  BIN_NAME    : $(BIN_NAME)"
	@echo "  LIB_NAME    : $(LIB_NAME)"
	@echo "  LIB_BUILD   : $(LIB_BUILD)"
	@echo "  LIB_ARTIFACT: $(LIB_ARTIFACT)"
	@echo "  DEBUG       : $(DEBUG)"
	@echo "  HOST_OS     : $(HOST_OS)"
	@echo ""

help:
	@echo "USAGE:"
	@echo "  make all       - build executable and library"
	@echo "  make bin       - build executable"
	@echo "  make lib       - build library"
	@echo "  make clean     - clean build directory"
	@echo "  make list      - list source files"
	@echo "  make info      - show build configuration"
	@echo "  make clang     - generate compile_commands.json"
	@echo "  make format    - format .c and .h"
	@echo "  make docker    - build and run development container"
	@echo "  make help      - this message\n"

clang:
	@$(MAKE) clean
	@bear -- $(MAKE) all

format:
	@for d in $(INCLUDE_PATH) $(SRC_PATH); do \
		[ -d "$$d" ] || continue; \
		find "$$d" \( -name "*.c" -o -name "*.h" \) -exec clang-format -i {} +; \
	done
	@echo "Format done."

export CC

DOCKER_IMAGE := qgit
docker:
	@if [ -z "$$(docker images -q $(DOCKER_IMAGE) 2>/dev/null)" ]; then \
		echo "Building Docker image $(DOCKER_IMAGE)..."; \
		docker build -t $(DOCKER_IMAGE) -f Dockerfile .; \
	fi
	@if [ -n "$$(docker ps -aq -f name=$(DOCKER_IMAGE)-container 2>/dev/null)" ]; then \
		docker rm -f $(DOCKER_IMAGE)-container 2>/dev/null || true; \
	fi
	@docker run -it --name $(DOCKER_IMAGE)-container -v $(CUR_DIR):/workspace $(DOCKER_IMAGE) /bin/bash
