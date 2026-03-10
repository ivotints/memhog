# 🧠 memhog

> Allocates every last megabyte of RAM it can find, holds it hostage, and refuses to let go.

---

## What is this?

`memhog` is a no-nonsense RAM stress-testing tool written in C++. It hammers your system memory as hard as possible — allocating RAM in large chunks, touching every single page to force the OS to actually commit it, and then sitting on all of it for 10 minutes while continuously trying to grab even more. Useful for stress testing, OOM killer behavior testing, leak detection in other processes, or just watching your fans spin up.

---

## ⚠️ Windows Warning

**Running this on Windows will very likely crash your entire system.**

Windows handles memory overcommit very differently from Linux. Once the pagefile is exhausted, the kernel tends to panic rather than gracefully invoke an OOM killer. Expect:

- Total system freeze requiring a hard reboot
- Blue Screen of Death (BSOD)
- Filesystem corruption if you're unlucky
- Unsaved work in every open app — gone

You have been warned. **Seriously.** If you insist on running it on Windows, save everything, close all applications, and accept the consequences.

---

## Platform Support

| Platform | Status | Notes |
|----------|--------|-------|
| Linux    | ✅ Works well | OOM killer may terminate the process gracefully |
| macOS    | ⚠️ Mostly fine | May cause heavy swap pressure and slowdowns |
| Windows  | 💀 Danger | Will likely freeze or BSOD the system |

---

## How it works

**Phase 1 — Eat:** Allocates RAM in 64 MB chunks. Every chunk is `memset` to force the OS to actually commit the pages (not just reserve virtual address space). Keeps going until `new` returns `nullptr`.

**Phase 2 — Hold:** Holds all allocated memory for 10 minutes. Every second, it tries to grab another 64 MB in case anything was freed or swapped out.

**Phase 3 — Release:** Frees all allocated memory cleanly and exits. `SIGINT` (`Ctrl+C`) is also handled gracefully.

---

## Build & Run

```bash
# Compile
g++ -O2 -o memhog ram_eater.cpp

# Run (Linux — may need sudo for maximum effect)
./memhog

# Or with higher priority
sudo nice -n -20 ./memhog
```

---

## Example Output

```
=== RAM EATER ===
Phase 1: Allocating as much RAM as possible...

[+] Allocated: 15360 MB  

[*] Hit memory limit at ~15360 MB
Phase 2: Holding for 10 minutes, probing for more every second...

[*] Holding 15360 MB | 9m 58s remaining
```

---

## Use Cases

- Stress testing system stability under memory pressure
- Testing OOM killer behavior and process priority on Linux
- Verifying swap configuration
- Reproducing memory-pressure bugs in other applications
- Benchmarking memory bandwidth (it touches every byte)

---

## License

Do whatever you want with it. MIT.

---

> **Disclaimer:** The author is not responsible for crashed systems, lost data, existential dread, or voided warranties. Use responsibly.
