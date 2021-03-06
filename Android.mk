SHELL=/bin/bash

LOCAL_PATH  := $(call my-dir)
ROOT_PATH   := $(abspath $(LOCAL_PATH))
MODULE_PATH := $(ROOT_PATH)
MAKE_RULES  := $(ROOT_PATH)/makerules

PROJECT_INCLUDES      :=
PROJECT_CFLAGS        += -DBUILD_ANDROID_AP -DENABLE_LOGGER
PROJECT_CFLAGS        += -DENABLE_MEMORY_POOL
PROJECT_CFLAGS        += -DFS_LOG_FULL_PATH=\"/data/vendor/camera/voyager.log\"
PROJECT_CXXFLAGS      := -std=c++11 $(PROJECT_CFLAGS)
PROJECT_LDFLAGS       := -fPIC
PROJECT_STATIC_LIBS   :=
PROJECT_SHARED_LIBS   :=
PROJECT_CPP_FEATURES  := rtti exceptions
PROJECT_TESTS_DIR     :=
PROJECT_EXCLUDE_DIRS  := libs obj release

BUILD_SIMPLE_TESTS    := n
ifneq ($(BUILD_SIMPLE_TESTS),y)
  PROJECT_EXCLUDE_DIRS += $(PROJECT_TESTS_DIR)
endif


PROJECT_MODULES       := libvoyager
PROJECT_STATIC_LIBS   := libvoyager.memory
PROJECT_STATIC_LIBS   += libvoyager.threads
PROJECT_STATIC_LIBS   += libvoyager.utils
PROJECT_STATIC_LIBS   += libvoyager.utils.sp
PROJECT_STATIC_LIBS   += libvoyager.common
PROJECT_STATIC_LIBS   += libvoyager.log

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(wildcard $(MODULE_PATH)/*.cpp)

LOCAL_C_INCLUDES := $(PROJECT_INCLUDES)

LOCAL_CFLAGS  := $(PROJECT_CXXFLAGS)

LOCAL_LDFLAGS := $(PROJECT_LDFLAGS)

LOCAL_STATIC_LIBRARIES := $(PROJECT_STATIC_LIBS)

LOCAL_SHARED_LIBRARIES := $(PROJECT_SHARED_LIBS)

LOCAL_ADDITIONAL_DEPENDENCIES := $(PROJECT_STATIC_LIBS)
LOCAL_ADDITIONAL_DEPENDENCIES += $(PROJECT_SHARED_LIBS)
LOCAL_ADDITIONAL_DEPENDENCIES += $(PROJECT_MODULES)

LOCAL_CPP_FEATURES := $(PROJECT_CPP_FEATURES)

LOCAL_MODULE := Voyager.Compile.Instructions

include $(BUILD_EXECUTABLE)


include $(MAKE_RULES)/submodule.android.make.rule
