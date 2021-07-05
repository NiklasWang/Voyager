LOCAL_PATH  := $(call my-dir)
MODULE_PATH := $(abspath $(LOCAL_PATH))

include $(CLEAR_VARS)

LOCAL_SRC_FILES := LambdaTest.cpp

LOCAL_C_INCLUDES := $(PROJECT_INCLUDES)

LOCAL_CFLAGS  := $(PROJECT_CXXFLAGS)

LOCAL_LDFLAGS := $(PROJECT_LDFLAGS)

LOCAL_STATIC_LIBRARIES := $(PROJECT_STATIC_LIBS)

LOCAL_SHARED_LIBRARIES := $(PROJECT_SHARED_LIBS)

LOCAL_CPP_FEATURES := $(PROJECT_CPP_FEATURES)

LOCAL_MODULE := LambdaTest

include $(BUILD_EXECUTABLE)
