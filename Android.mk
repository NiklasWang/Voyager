SHELL=/bin/bash

LOCAL_PATH  := $(call my-dir)
ROOT_PATH   := $(abspath $(LOCAL_PATH))
MODULE_PATH := $(ROOT_PATH)
MAKE_RULES  := $(ROOT_PATH)/makerules

PROJECT_INCLUDES      :=
PROJECT_CFLAGS        += -DENABLE_LOGGER -DPRINT_LOGCAT_LOG  -DBUILD_ANDROID_AP
PROJECT_CFLAGS        += -DENABLE_CHECK_POINTS_TRACKER=1 -DENABLE_MEMORY_POOL -O0
PROJECT_CFLAGS        += -DPANDORA_LIBRARY_PATH=\"/vendor/lib64\"
PROJECT_CFLAGS        += -DXML_CONFIGURATION_PATH=\"/vendor/etc/camera/pandora\"
PROJECT_CFLAGS        += -DDUMP_PATH=\"/data/vendor/camera/pandora_dump\"
PROJECT_CFLAGS        += -DFS_LOG_FULL_PATH=\"/data/vendor/camera/pandora.log\"
PROJECT_CXXFLAGS      := -std=c++11 $(PROJECT_CFLAGS)
PROJECT_LDFLAGS       := -fPIC
PROJECT_STATIC_LIBS   :=
PROJECT_SHARED_LIBS   :=
PROJECT_CPP_FEATURES  := rtti exceptions
PROJECT_TESTS_DIR     := makerules tools
PROJECT_EXCLUDE_DIRS  := legacy libs obj release resources SiriusIntf

BUILD_SIMPLE_TESTS    := n
ifneq ($(BUILD_SIMPLE_TESTS),y)
  PROJECT_EXCLUDE_DIRS += $(PROJECT_TESTS_DIR)
endif


PROJECT_MODULES       := libpandora
PROJECT_STATIC_MODS   := libpandora.pipeline
PROJECT_STATIC_MODS   += libpandora.pipeline.simulator
PROJECT_STATIC_MODS   += libpandora.pipeline.simulator.algorithms
PROJECT_STATIC_MODS   += libpandora.pal
PROJECT_STATIC_MODS   += libpandora.algorithm
PROJECT_STATIC_MODS   += libpandora.algorithms
PROJECT_STATIC_MODS   += libpandora.algorithms.basic
PROJECT_STATIC_MODS   += libpandora.algorithms.log
PROJECT_STATIC_MODS   += libpandora.converter
PROJECT_STATIC_MODS   += libpandora.camera
PROJECT_STATIC_MODS   += libpandora.xml
PROJECT_STATIC_MODS   += libpandora.memory
PROJECT_STATIC_MODS   += libpandora.threads
PROJECT_STATIC_MODS   += libpandora.utils
PROJECT_STATIC_MODS   += libpandora.utils.sp
PROJECT_STATIC_MODS   += libpandora.common
PROJECT_STATIC_MODS   += libpandora.log
PROJECT_SHARED_MODS   := libpandora.render
PROJECT_PLATFORMS     := libpandora.pal.platform.algorithms.module.tester
PROJECT_PLATFORMS     += libpandora.pal.platform.camera.module.tester
PROJECT_PLATFORMS     += libpandora.pal.single.instance.tester
PROJECT_PLATFORMS     += libpandora.pal.platform.single.instance.tester
PROJECT_PLATFORMS     += libpandora.pal.multiple.instance.tester
PROJECT_PLATFORMS     += libpandora.pal.platform.multiple.instance.tester
PROJECT_PLATFORMS     += libpandora.pal.platform.generic.products
PROJECT_PLATFORMS     += libpandora.pal.platform.pandora.tester
PROJECT_TESTER        := Pandora.Memory.Tester
PROJECT_TESTER        += Pandora.Camera.Tester
PROJECT_TESTER        += Pandora.Converter.Tester
PROJECT_TESTER        += Pandora.Algorithms.Tester
PROJECT_TESTER        += Pandora.Pal.Tester
PROJECT_TESTER        += Pandora.Multiple.Pal.Instance.Tester
PROJECT_TESTER        += Pandora.Pipeline.Tester
PROJECT_TESTER        += Pandora.Pipeline.Simulator
PROJECT_TESTER        += Pandora.Xml.Tester
PROJECT_ALGORITHMS    := libpandora.algorithm.niklas.Watermark.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.BmpWatermark.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.TextWatermark.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.JpegSWEncoder.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.YUVRotator.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.YUVScaler.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.JpegExif.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_M1_3F_NV21_Source.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_M2_2F_NV21_Source_SinkS2.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_M3_3F_NV12_SinkS4_SinkM2.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_M4_2F_NV12.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_M5_3F_NV12NV21_SinkM1.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_S1_NV21_Source.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_S2_NV12_Source.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_S3_NV21.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_S4_NV12NV21_Source.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_S5_NV12_Source_SinkS1.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_S6_NV21_SinkS2.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_S7_NV21_SinkS2_SinkS4_SinkS5.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_S8_JPEG.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_S9_BAYER.0
PROJECT_ALGORITHMS    += libpandora.algorithm.niklas.Sim_S10_NV21NV12JPEGBAYER.0
PROJECT_EXTERNAL_LIBS := libpandora.external.yuv
PROJECT_EXTERNAL_LIBS += libpandora.external.ft2
PROJECT_EXTERNAL_LIBS += libpandora.external.exif
PROJECT_EXTERNAL_LIBS += libpandora.external.jpeg
PROJECT_EXTERNAL_LIBS += libpandora.external.png
PROJECT_EXTERNAL_LIBS += libpandora.external.z
PROJECT_EXTERNAL_LIBS += libpandora.external.xerces-c

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
LOCAL_ADDITIONAL_DEPENDENCIES += $(PROJECT_STATIC_MODS)
LOCAL_ADDITIONAL_DEPENDENCIES += $(PROJECT_SHARED_MODS)
LOCAL_ADDITIONAL_DEPENDENCIES += $(PROJECT_ALGORITHMS)
LOCAL_ADDITIONAL_DEPENDENCIES += $(PROJECT_PLATFORMS)
LOCAL_ADDITIONAL_DEPENDENCIES += $(PROJECT_TESTER)
LOCAL_ADDITIONAL_DEPENDENCIES += $(PROJECT_EXTERNAL_LIBS)

LOCAL_CPP_FEATURES := $(PROJECT_CPP_FEATURES)

LOCAL_MODULE := Pandora.Compile.Instructions

include $(BUILD_EXECUTABLE)


include $(MAKE_RULES)/submodule.android.make.rule
