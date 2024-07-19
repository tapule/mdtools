# SPDX-License-Identifier: MIT
#
# -- MegaDrive development tools --
# Coded by: Juan Ángel Moreno Fernández (@_tapule) 2024
# Github: https://github.com/tapule/mdtools
#

BUILD_DIR   := $(shell pwd)/build
INSTALL_DIR ?= $(shell pwd)/tools

# Some ANSI terminal color codes
COLOR_RESET      = $'\033[0m
COLOR_RED        = $'\033[1;31;49m
COLOR_GREEN      = $'\033[1;32;49m
COLOR_YELLOW     = $'\033[1;33;49m
COLOR_BLUE       = $'\033[1;34;49m
COLOR_MAGENTA    = $'\033[1;35;49m
COLOR_CYAN       = $'\033[1;36;49m
COLOR_WHITE      = $'\033[1;37;49m

# Packages versions
BINTOC_VER        ?= 0.01
PALTOOL_VER       ?= 0.03
TILEIMAGETOOL_VER ?= 0.02
TILESETTOOL_VER   ?= 0.02
WAVTORAW_VER      ?= 1.2
XGMTOOL_VER       ?= 1.73

.PHONY: all bintoc paltool tileimagetool tilesettool  wavtoraw xgmtool

all: info bintoc paltool tileimagetool tilesettool  wavtoraw xgmtool

bintoc:
	@echo "$(COLOR_GREEN)>> Building bintoc...$(COLOR_RESET)"
	@make -C bintoc BUILD_DIR=$(BUILD_DIR)

paltool:
	@echo "$(COLOR_GREEN)>> Building paltool...$(COLOR_RESET)"
	@make -C paltool BUILD_DIR=$(BUILD_DIR)

tileimagetool:
	@echo "$(COLOR_GREEN)>> Building tileimagetool...$(COLOR_RESET)"
	@make -C tileimagetool BUILD_DIR=$(BUILD_DIR)

tilesettool:
	@echo "$(COLOR_GREEN)>> Building tilesettool...$(COLOR_RESET)"
	@make -C tilesettool BUILD_DIR=$(BUILD_DIR)

wavtoraw:
	@echo "$(COLOR_GREEN)>> Building wawtoraw...$(COLOR_RESET)"
	@make -C wavtoraw BUILD_DIR=$(BUILD_DIR)

xgmtool:
	@echo "$(COLOR_GREEN)>> Building xgmtool...$(COLOR_RESET)"
	@make -C xgmtool BUILD_DIR=$(BUILD_DIR)

# Help
.PHONY: help
help:
	@echo "$(COLOR_MAGENTA)== MegaDrive development tools ==$(COLOR_RESET)"
	@echo "$(COLOR_WHITE)    make help            display this help"
	@echo "    make info            prints paths, packages version, etc."
	@echo "    make all             builds the toolset"
	@echo "    make <tool>          builds a concrete tool"
	@echo "    make install         builds and installs the toolset"
	@echo "    make clean           removes tempory files and build dirs"

# Info
.PHONY: info
info:
	@echo "$(COLOR_MAGENTA)== MegaDrive development tools ==$(COLOR_RESET)"
	@echo "$(COLOR_WHITE)    * Paths:"
	@echo "        BUILD_DIR: $(BUILD_DIR)"
	@echo "        INSTALL_DIR: $(INSTALL_DIR)"
	@echo "    * Packages:"
	@echo "        BINTOC_VER $(BINTOC_VER)"
	@echo "        PALTOOL_VER $(PALTOOL_VER)"
	@echo "        TILEIMAGETOOL_VER $(TILEIMAGETOOL_VER)"
	@echo "        TILESETTOOL_VER $(TILESETTOOL_VER)"
	@echo "        WAVTORAW_VER $(WAVTORAW_VER)"
	@echo "        XGMTOOL_VER $(XGMTOOL_VER)"

# Install
.PHONY: install
install: all
	@echo "$(COLOR_YELLOW)> Installing tools...$(COLOR_RESET)"
	@mkdir -p $(INSTALL_DIR)
	cp -rf $(BUILD_DIR)/* $(INSTALL_DIR)
	@echo "$(COLOR_GREEN)> Toolset installed to $(INSTALL_DIR).$(COLOR_RESET)"

# Cleaning
.PHONY: clean
clean:
	@echo "$(COLOR_MAGENTA)> Cleaning...$(COLOR_RESET)"
	@make -C bintoc clean
	@make -C paltool clean
	@make -C tileimagetool clean
	@make -C tilesettool clean
	@make -C wavtoraw clean
	@make -C xgmtool clean
	rm -rf $(BUILD_DIR)
