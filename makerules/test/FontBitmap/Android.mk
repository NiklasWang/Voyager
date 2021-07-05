LOCAL_PATH  := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := $(wildcard $(abspath $(LOCAL_PATH))/*.c)

LOCAL_C_INCLUDES := $(PROJECT_INCLUDES)

LOCAL_CFLAGS  := $(PROJECT_CFLAGS)

LOCAL_LDFLAGS := $(PROJECT_LDFLAGS)

LOCAL_STATIC_LIBRARIES := $(PROJECT_STATIC_LIBS)

LOCAL_SHARED_LIBRARIES := $(PROJECT_SHARED_LIBS)
LOCAL_STATIC_LIBRARIES += libpandora.external.ft2
LOCAL_STATIC_LIBRARIES += libpandora.external.png

LOCAL_C_INCLUDES += $(ROOT_PATH)/external/freetype/freetype-2.6.5/include
LOCAL_C_INCLUDES += $(ROOT_PATH)/external/freetype/freetype-2.6.5/include/freetype

LOCAL_CPP_FEATURES := $(PROJECT_CPP_FEATURES)

LOCAL_MODULE := FontBitmapTest

include $(BUILD_EXECUTABLE)
