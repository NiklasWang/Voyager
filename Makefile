PROJECT_ROOT_DIR    = $(shell pwd)
PROJECT_STATIC_LIBS = libcommon libsp
PROJECT_SHARED_LIBS = liblog libmemory libutils
PROJECT_EXCLUDE_DIR = release obj libs

PROJECT_DEPENDENCY  = nasm yasm cpp autopoint cmake aclocal automake libtoolize pkg-config

PROJECT_CFLAGS    = -g

include $(PROJECT_ROOT_DIR)/makerules/64bit.32bit.check.make.rule
ifeq ($(BUILD_64BIT),y)
  PROJECT_CFLAGS += -DBUILD_LINUX_X86_64
else
  PROJECT_CFLAGS += -DBUILD_LINUX_X86
endif
PROJECT_CXXFLAGS  = $(PROJECT_CFLAGS)
PROJECT_CXXFLAGS  += -DENABLE_LOGGER -DENABLE_MEMORY_POOL
PROJECT_CXXFLAGS  += -DFS_LOG_FULL_PATH=\"./voyager.log\"

PROJECT_LDFLAGS   = -pthread

PROJECT_RELEASE   = VoyagerTester

PROJECT_MAKEFILE  = $(PROJECT_ROOT_DIR)/makerules/Makefile.make.rule

include $(PROJECT_MAKEFILE)
