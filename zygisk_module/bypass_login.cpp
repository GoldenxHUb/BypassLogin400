#include <jni.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <android/log.h>
#include <dlfcn.h>
#include <Dobby/dobby.h>

#define LOG_TAG "BypassLogin400"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

struct Il2CppString {
    void* klass;
    void* monitor;
    int32_t length;
    uint16_t start_char;
};

typedef struct Il2CppString Il2CppString;

static void* (*il2cpp_string_new)(const char* str) = nullptr;

extern "C" {
    void* xdl_open(const char* name, int flag);
    void* xdl_sym(void* handle, const char* sym, size_t* size);
}

static void resolve_il2cpp_api() {
    if (il2cpp_string_new) return;
    void* handle = xdl_open("libil2cpp.so", 0);
    if (!handle) {
        LOGE("Cannot open libil2cpp.so");
        return;
    }
    il2cpp_string_new = (void* (*)(const char*))xdl_sym(handle, "il2cpp_string_new", nullptr);
    LOGI("il2cpp_string_new: %p", il2cpp_string_new);
}

void (*original_CreateLoginReq)(void*, void**);

void Hook_CreateLoginReq(void* _this, void** reqPtr) {
    LOGI("=== CreateLoginReqInfoData called ===");
    LOGI("_this=%p reqPtr=%p", _this, reqPtr);

    original_CreateLoginReq(_this, reqPtr);
    LOGI("Original called, req now %p", reqPtr ? *reqPtr : nullptr);

    if (!reqPtr || !*reqPtr) return;
    void* req = *reqPtr;

    void* verStr = *(void**)((uintptr_t)req + 0x24);
    LOGI("verStr @ 0x24: %p", verStr);
    if (verStr) {
        Il2CppString* s = (Il2CppString*)verStr;
        LOGI("Version length: %d", s->length);
        if (s->length > 0 && s->length < 100) {
            char buf[128] = {0};
            uint16_t* chars = (uint16_t*)((uintptr_t)s + 0x0C);
            for (int i = 0; i < s->length && i < 127; i++)
                buf[i] = (char)chars[i];
            LOGI("Version String: '%s'", buf);

            int dot1 = -1, dot2 = -1;
            for (int32_t i = 0; i < s->length; i++) {
                if (chars[i] == '.') {
                    if (dot1 == -1) dot1 = i;
                    else { dot2 = i; break; }
                }
            }
            if (dot1 >= 0 && dot2 > dot1 + 1) {
                int midVal = 0;
                for (int32_t i = dot1 + 1; i < dot2; i++)
                    midVal = midVal * 10 + (chars[i] - '0');
                if (midVal > 0) {
                    midVal--;
                    int32_t pos = dot2 - 1;
                    do {
                        chars[pos--] = '0' + (midVal % 10);
                        midVal /= 10;
                    } while (midVal > 0 && pos > dot1);
                    while (pos > dot1) chars[pos--] = '0';
                }
                LOGI("Version string patched");
            }
        } else {
            LOGE("Version length invalid: %d (offset 0x24 may be wrong)", s->length);
        }
    } else {
        LOGE("verStr null - offset 0x24 may be wrong");
    }

    void* verCode = *(void**)((uintptr_t)req + 0x108);
    LOGI("verCode @ 0x108: %p", verCode);

    if (il2cpp_string_new) {
        void* newStr = il2cpp_string_new("2019118045");
        *(void**)((uintptr_t)req + 0x108) = newStr;
        LOGI("Version code spoofed to 2019118045, new=%p", newStr);
    }
}

void InstallLoginHook() {
    LOGI("InstallLoginHook starting...");
    resolve_il2cpp_api();

    typedef void* (*Il2CppGetMethodOffset_t)(const char*, const char*, const char*, const char*, int);
    Il2CppGetMethodOffset_t Il2CppGetMethodOffset =
        (Il2CppGetMethodOffset_t)dlsym(RTLD_DEFAULT, "Il2CppGetMethodOffset");

    if (!Il2CppGetMethodOffset) {
        LOGE("Il2CppGetMethodOffset not found");
        return;
    }

    void* target = Il2CppGetMethodOffset(
        "Assembly-CSharp.dll", "COW", "UIModelLogin", "CreateLoginReqInfoData", 1);

    if (!target) {
        LOGE("CreateLoginReqInfoData not found");
        return;
    }

    LOGI("Hooking CreateLoginReqInfoData at %p", target);

    int ret = DobbyHook(target, (void*)Hook_CreateLoginReq, (void**)&original_CreateLoginReq);
    if (ret == 0) {
        LOGI("Hook installed successfully!");
    } else {
        LOGE("DobbyHook failed: %d", ret);
    }
}
