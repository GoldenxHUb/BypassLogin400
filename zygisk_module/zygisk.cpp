#include <jni.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <android/log.h>
#include <dlfcn.h>
#include "zygisk.hpp"

#define LOG_TAG "BypassLogin400"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

using zygisk::Api;
using zygisk::AppSpecializeArgs;

void InstallLoginHook();

static const char* TARGET_PKGS[] = {
    "com.dts.freefireth",
    "com.dts.freefiremax",
    nullptr
};

class BypassLoginModule : public zygisk::ModuleBase {
public:
    void onLoad(Api* api, JNIEnv* env) override {
        this->api_ = api;
        this->env_ = env;
    }

    void preAppSpecialize(AppSpecializeArgs* args) override {
        is_target_ = false;
        if (!args->nice_name) return;

        const char* process = env_->GetStringUTFChars(args->nice_name, nullptr);
        if (process) {
            for (int i = 0; TARGET_PKGS[i]; i++) {
                if (strcmp(process, TARGET_PKGS[i]) == 0) {
                    is_target_ = true;
                    LOGI("Target: %s", process);
                    break;
                }
            }
            env_->ReleaseStringUTFChars(args->nice_name, process);
        }

        if (is_target_) {
            api_->setOption(zygisk::FORCE_DENYLIST_UNMOUNT);
        }
    }

    void postAppSpecialize(const AppSpecializeArgs* args) override {
        if (!is_target_) return;
        LOGI("=== BypassLogin400 initializing ===");
        sleep(10);
        InstallLoginHook();
        LOGI("=== BypassLogin400 ready ===");
    }

private:
    Api* api_ = nullptr;
    JNIEnv* env_ = nullptr;
    bool is_target_ = false;
};

REGISTER_ZYGISK_MODULE(BypassLoginModule)
