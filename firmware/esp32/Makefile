#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := cpv2017
IDF_PATH ?= $(dir $(realpath $(firstword $(MAKEFILE_LIST))))/../esp-idf
APP_GIT_VERSION := $(shell git describe --dirty --always --tags)

include $(IDF_PATH)/make/project.mk

CPPFLAGS += -D APP_GIT_VERSION=\"$(APP_GIT_VERSION)\"

# Copy some defaults into the sdkconfig by default
sdkconfig: sdkconfig.defaults
	$(Q) cp $< $@

menuconfig: sdkconfig
defconfig: sdkconfig
