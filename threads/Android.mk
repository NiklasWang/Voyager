LOCAL_PATH  := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(wildcard $(abspath $(LOCAL_PATH))/*.cpp)

LOCAL_C_INCLUDES := $(PROJECT_INCLUDES)

LOCAL_CFLAGS  := $(PROJECT_CXXFLAGS)

LOCAL_STATIC_LIBRARIES := $(PROJECT_STATIC_LIBS)

LOCAL_SHARED_LIBRARIES := $(PROJECT_SHARED_LIBS)

LOCAL_CPP_FEATURES := $(PROJECT_CPP_FEATURES)

LOCAL_MODULE := libvoyager.threads

LOCAL_INCLUDE_MODULE := log common memory utils sp core
include $(MAKE_RULES)/find.library.android.make.rule
LOCAL_C_INCLUDES += $(_LOCAL_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY)
