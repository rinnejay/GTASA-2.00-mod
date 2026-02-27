#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "GTA_STYLE_HOOK", __VA_ARGS__)

// Fungsi untuk membuka proteksi memori (Read-Write-Execute)
void Unprotect(uintptr_t addr, size_t size) {
    size_t pagesize = sysconf(_SC_PAGESIZE);
    uintptr_t start = addr & ~(pagesize - 1);
    mprotect((void *)start, pagesize, PROT_READ | PROT_WRITE | PROT_EXEC);
}

// Fungsi pencari alamat dasar library
uintptr_t find_library_base(const char* library_name) {
    uintptr_t address = 0;
    char line[512];
    FILE* fp = fopen("/proc/self/maps", "rt");
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, library_name)) {
                address = (uintptr_t)strtoul(line, NULL, 16);
                break;
            }
        }
        fclose(fp);
    }
    return address;
}



extern "C" JNIEXPORT void JNICALL
Java_com_example_ndkapp_MainActivity_activateHookNow(
    JNIEnv* env, jobject thiz,
    jint mNewHP) {
    
    uintptr_t base = find_library_base("libtarget.so");
    
    if (base == 0) {
        LOGI("ERROR: Library tidak ditemukan!");
        return;
    }

    // Alamat target dari IDA: 0x62510
    uintptr_t target_addr = base + 0x62510;
    
    // Kita pecah nilai jint (32-bit) menjadi dua bagian untuk instruksi MOVZ & MOVK
    uint16_t low = (uint16_t)(mNewHP & 0xFFFF);
    uint16_t high = (uint16_t)((mNewHP >> 16) & 0xFFFF);

    // Patching 3 instruksi (12 byte):
    // 1. MOVZ W0, #low       -> Masukkan 16-bit bawah ke W0
    // 2. MOVK W0, #high, LSL #16 -> Masukkan 16-bit atas ke W0 tanpa hapus bawah
    // 3. RET                 -> Keluar fungsi
    
    uint32_t patch_code[] = {
        (uint32_t)(0x52800000 | (low << 5)),         // MOVZ W0, #low
        (uint32_t)(0x72A00000 | (high << 5)),        // MOVK W0, #high, LSL #16
        0xD65F03C0                                   // RET
    };

    LOGI("Memasang HP: %d di Alamat: %p", mNewHP, (void*)target_addr);

    // Eksekusi Patching (12 byte)
    Unprotect(target_addr, 12);
    memcpy((void*)target_addr, patch_code, 12);
    __builtin___clear_cache((char *)target_addr, (char *)target_addr + 12);

    LOGI("PATCH BERHASIL! HP diset ke %d", mNewHP);
}