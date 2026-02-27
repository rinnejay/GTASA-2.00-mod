#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include <thread>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "TARGET_GAME", __VA_ARGS__)

int player_hp = 100;

// Gunakan extern "C" agar nama fungsi tidak berubah (mangled) di IDA
extern "C" __attribute__((noinline)) int get_current_hp() {
    return player_hp;
}

void game_loop() {
    while (true) {
        LOGI("Game Loop: HP = %d", get_current_hp());
        sleep(2);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_ndkapp_MainActivity_startTargetLoop(JNIEnv* env, jobject thiz) {
    std::thread(game_loop).detach();
 
}
