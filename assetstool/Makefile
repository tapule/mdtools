# SPDX-License-Identifier: MIT
#
# -- MegaDrive development tools --
# Coded by: Juan Ángel Moreno Fernández (@_tapule) 2025
# Github: https://github.com/tapule/mdtools
#
# assetstool a Sega Megadrive/Genesis assets builder
#

#**
# \file             Makefile
# \brief            Project compiler makefile script
#

# Default paths for our tool
BUILD_DIR ?= $(shell pwd)/build
APP := $(BUILD_DIR)/assetstool

# Some ANSI terminal color codes
COLOR_RESET      = $'\033[0m
COLOR_RED        = $'\033[1;31;49m
COLOR_GREEN      = $'\033[1;32;49m
COLOR_YELLOW     = $'\033[1;33;49m
COLOR_BLUE       = $'\033[1;34;49m
COLOR_MAGENTA    = $'\033[1;35;49m
COLOR_CYAN       = $'\033[1;36;49m
COLOR_WHITE      = $'\033[1;37;49m

# Tools
CC      := gcc
MKDIR   := mkdir -p
RM      := rm -f

# Sources paths
SRCDIR  := src
LIBDIR  := lib

# Includes
INCS     = -Isrc -Ilib

# Default base flags
CFLAGS  := $(CFLAGS) -Wall -Wextra -pedantic -std=c23
LDFLAGS := $(LDFLAGS)

# Extra flags set by debug or release target as needed
EXFLAGS  =

# Sources
CSRC    = $(shell find src/ -type f -name '*.c')
CSRC   += $(shell find libs/ -type f -name '*.c')

# Objets files
OBJS    = $(CSRC:.c=.o)
OUTOBJS = $(addprefix build/obj/, $(OBJS))

.PHONY: all release debug

all: release

# Release target including optimizations
release: EXFLAGS  = -O3
release: $(APP)

# Debug target, enables GDB tracing
debug: EXFLAGS = -g -Og -DDEBUG
debug: $(APP)

$(APP): build-msg $(OUTOBJS)
	@mkdir -p $(dir $@)
	$(CC) -o $@ $(LDFLAGS) $(OUTOBJS)

build/obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(EXFLAGS) $(INCS) -c $< -o $@

# Simple build message
.PHONY: build-msg
build-msg:
	@echo "$(COLOR_GREEN)>> Building assetstool...$(COLOR_RESET)"

# Help
.PHONY: help
help:
	@echo "$(COLOR_MAGENTA)== Mega Drive assetstool builder ==$(COLOR_RESET)"
	@echo "$(COLOR_WHITE)    make help            display this help"
	@echo "    make info            prints paths and other flags"
	@echo "    make all             builds assetstool"
	@echo "    make clean           removes tempory files and build dir"

# Info
.PHONY: info
info:
	@echo "$(COLOR_MAGENTA)== Mega Drive assetstool builder ==$(COLOR_RESET)"
	@echo "$(COLOR_WHITE)    * Paths:"
	@echo "        BUILD_DIR: $(BUILD_DIR)"
	@echo "        APP_PATH: $(APP)"
	@echo "    * Other flags and files:"
	@echo "        PREFIX $(PREFIX)"
	@echo "        PATH $(PATH)"
	@echo "        SOURCES $(CSRC)"
	@echo "        OBJS $(OUTOBJS)"

.PHONY: clean
clean:
	@echo "$(COLOR_MAGENTA)> Cleaning assetstool...$(COLOR_RESET)"
	@rm -rf build
