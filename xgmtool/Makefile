# SPDX-License-Identifier: MIT
#
# SGKD
# Coded by: Stephane Dallongeville
# Github: https://github.com/Stephane-D/SGDK
#
# xgmtool a Sega Megadrive VGM-XGM conversor utility
#

BUILD_DIR ?= $(shell pwd)/build
APP := $(BUILD_DIR)/xgmtool

# Tools
CC      := gcc
MKDIR   := mkdir -p
RM      := rm -f

SRCDIR  := src
SRCTREE := $(shell find $(SRCDIR) -type d)
OBJDIR  := obj
OBJTREE := $(SRCTREE:$(SRCDIR)%=$(OBJDIR)%)

# Default base flags
CFLAGS  := $(CFLAGS) -Wall -Wextra -pedantic -std=c17
LDFLAGS := $(LDFLAGS)
LIBS    := -lm

# Sources and objects
CSRC  := $(foreach DIR,$(SRCTREE),$(wildcard $(DIR)/*.c))
OBJS  := $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(CSRC:.c=.o))

.PHONY: all release debug

all: release

release: EXFLAGS  = -O3
release: $(APP)

debug: EXFLAGS = -g -Og -DDEBUG
debug: $(APP)

$(APP): $(BUILD_DIR) $(OBJTREE) $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

obj/%.o: src/%.c
	$(CC) $(CCFLAGS) $(EXFLAGS) -c $< -o $@

$(BUILD_DIR) $(OBJTREE):
	@$(MKDIR) $@

.PHONY: clean
clean:
	@rm -rf obj
	@rm -f $(APP)

.PHONY: info
info:
	$(info $(SRCTREE))
	$(info $(OBJTREE))
	$(info $(CSRC))
	$(info $(OBJS))
