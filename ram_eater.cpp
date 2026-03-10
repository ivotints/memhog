#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cstring>
#include <csignal>
#include <atomic>

static std::atomic<bool> running(true);

void handle_signal(int) {
    std::cout << "\n[!] Signal received, freeing memory and exiting...\n";
    running = false;
}

int main() {
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);

    const size_t CHUNK_MB = 64;                    // allocate in 64 MB chunks
    const size_t CHUNK_BYTES = CHUNK_MB * 1024 * 1024;
    const int WAIT_MINUTES = 10;
    const int WAIT_SECONDS = WAIT_MINUTES * 60;

    std::vector<char*> blocks;
    size_t total_mb = 0;

    std::cout << "=== RAM EATER ===\n";
    std::cout << "Phase 1: Allocating as much RAM as possible...\n\n";

    // --- Phase 1: Eat all RAM ---
    while (running) {
        char* block = nullptr;
        try {
            block = new (std::nothrow) char[CHUNK_BYTES];
            if (!block) break;

            // Actually touch every page so the OS commits the memory
            std::memset(block, 0xAA, CHUNK_BYTES);

            blocks.push_back(block);
            total_mb += CHUNK_MB;
            std::cout << "\r[+] Allocated: " << total_mb << " MB  " << std::flush;
        } catch (...) {
            if (block) delete[] block;
            break;
        }
    }

    std::cout << "\n\n[*] Hit memory limit at ~" << total_mb << " MB\n";
    std::cout << "Phase 2: Holding for " << WAIT_MINUTES
              << " minutes, probing for more every second...\n\n";

    // --- Phase 2: Hold + keep trying ---
    for (int s = 0; s < WAIT_SECONDS && running; ++s) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Try to grab one more chunk
        char* extra = new (std::nothrow) char[CHUNK_BYTES];
        if (extra) {
            std::memset(extra, 0xAA, CHUNK_BYTES);
            blocks.push_back(extra);
            total_mb += CHUNK_MB;
            std::cout << "[+] Grabbed extra chunk! Total: " << total_mb << " MB\n";
        }

        int remaining = WAIT_SECONDS - s - 1;
        std::cout << "\r[*] Holding " << total_mb << " MB | "
                  << remaining / 60 << "m " << remaining % 60
                  << "s remaining   " << std::flush;
    }

    // --- Phase 3: Free everything ---
    std::cout << "\n\nPhase 3: Releasing " << total_mb << " MB...\n";
    for (char* b : blocks) delete[] b;
    blocks.clear();

    std::cout << "[*] Done. All memory freed.\n";
    return 0;
}
