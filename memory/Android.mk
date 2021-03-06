LOCAL_PATH  := $(call my-dir)
MODULE_PATH := $(abspath $(LOCAL_PATH))

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(wildcard $(MODULE_PATH)/*.cpp)

LOCAL_C_INCLUDES := $(PROJECT_INCLUDES)

LOCAL_CFLAGS  := $(PROJECT_CXXFLAGS)

LOCAL_STATIC_LIBRARIES := $(PROJECT_STATIC_LIBS)

LOCAL_SHARED_LIBRARIES := $(PROJECT_SHARED_LIBS)

LOCAL_CPP_FEATURES := $(PROJECT_CPP_FEATURES)

LOCAL_MODULE := libvoyager.memory

LOCAL_INCLUDE_MODULE := log common utils
include $(MAKE_RULES)/find.library.android.make.rule
LOCAL_C_INCLUDES += $(_LOCAL_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)

compile_order :=
exclude_dirs  :=
include $(MAKE_RULES)/submodule.android.make.rule