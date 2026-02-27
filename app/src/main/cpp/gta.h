#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

// Fungsi untuk mengubah proteksi memori (seperti di GTA Source)
void Unprotect(uintptr_t addr, size_t size) {
    size_t pagesize = sysconf(_SC_PAGESIZE);
    uintptr_t start = addr & ~(pagesize - 1);
    mprotect((void *)start, pagesize, PROT_READ | PROT_WRITE | PROT_EXEC);
}

// Fungsi untuk menimpa instruksi (Patching)
void WriteMemory(uintptr_t addr, uintptr_t data, size_t size) {
    Unprotect(addr, size);
    memcpy((void *)addr, (void *)&data, size);
}