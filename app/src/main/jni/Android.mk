
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := facedetection

LOCAL_C_INCLUDES :=  $(LOCAL_PATH)/opencv/include

LOCAL_LDLIBS += -llog -ldl -lz -latomic
LOCAL_LDFLAGS += -ljnigraphics
# import openCV
LOCAL_STATIC_LIBRARIES += openCV

LOCAL_SRC_FILES := $(LOCAL_PATH)/facedetection.cpp\

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := openCV
LOCAL_SRC_FILES := $(LOCAL_PATH)/opencv/libs/$(TARGET_ARCH_ABI)/libopenCV.a
LOCAL_STATIC_LIBRARIES := tbb
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE := tbb
LOCAL_SRC_FILES := $(LOCAL_PATH)/3rdparty/libs/$(TARGET_ARCH_ABI)/libtbb.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/3rdparty/include
include $(PREBUILT_STATIC_LIBRARY)


