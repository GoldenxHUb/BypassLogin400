LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE           := zygisk
LOCAL_SRC_FILES        := zygisk.cpp bypass_login.cpp
LOCAL_LDLIBS           := -llog -ldl
LOCAL_CPPFLAGS         := -std=c++17 -fno-exceptions -fno-rtti -fvisibility=hidden
LOCAL_CFLAGS           := -fvisibility=hidden
LOCAL_STATIC_LIBRARIES := libdobby
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := libdobby
LOCAL_SRC_FILES := Dobby/$(TARGET_ARCH_ABI)/libdobby.a
include $(PREBUILT_STATIC_LIBRARY)
