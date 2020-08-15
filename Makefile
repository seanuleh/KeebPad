#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := MK32

COMPONENT_ADD_INCLUDEDIRS := components/include	\							

CFLAGS += -Wno-unused-const-variable

include $(IDF_PATH)/make/project.mk
