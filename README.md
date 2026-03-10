[readme.md](https://github.com/user-attachments/files/25824290/readme.md)
# Shellcode Mutator and APC Injection Tool

This project demonstrates various offensive security techniques including XOR encoding/decoding, AES encryption, shellcode mutation, and APC injection. The purpose of this project is to provide a tool for educational and ethical hacking purposes. Use this tool responsibly and only in environments where you have permission to test.
# Techniques Used in the Shellcode Mutator and APC Injection Tool

## 1. XOR Encoding/Decoding
**Description**:
XOR encoding is a simple encryption technique used to obfuscate data. It involves applying the XOR operation between each byte of the data and a key.

**Purpose**:
To obfuscate the shellcode, making it harder to detect by simple signature-based detection mechanisms.

---

## 2. AES Decryption
**Description**:
AES (Advanced Encryption Standard) is a strong encryption algorithm used to decrypt data. In this project, the Windows CryptoAPI is used for AES decryption.

**Purpose**:
To securely encrypt and decrypt the shellcode, ensuring that it is not easily readable or detectable by static analysis tools.

---

## 3. Shellcode Mutation
**Description**:
This technique involves adding NOPs (No Operation instructions) to the shellcode to alter its signature.

**Purpose**:
To evade signature-based detection by changing the signature of the shellcode.

---

## 4. APC Injection
**Description**:
APC (Asynchronous Procedure Call) injection is a technique used to execute shellcode within the context of another process by queuing an APC to a thread in the target process.

**Purpose**:
To execute shellcode within a legitimate process, which can help evade detection and achieve persistence or privilege escalation.



## Files

| File Name | Description |
| - | - |
| Decoder.h | Contains the XOR encoding/decoding function. |
| Decrypt.h | Contains the AES decryption function. |
| Mutate.h | Contains the function to decrypt and mutate the shellcode. |
| ShellcodeMutator.h | Contains the class for mutating the shellcode. |
| Project Stealth Injector.cpp | The main function that ties everything together. |

## Installation

1. **Prerequisites**: Ensure you have the following installed:
   - Visual Studio (for C++ development)
   - Git (for version control)

2. **Clone the Repository**:
   ```bash
   git clone https://github.com/khalid609/ShellcodeMutator-APCInjection.git
3. **how to Use**
   - Open the project in Visual Studio.
   - Ensure all dependencies are installed.
   
   - Encode the Shellcode :
      - First, encode the shellcode using the XOR encoder.
      - Add the XOR key in the Project Stealth Injector.cpp file.
   
   - Encrypt the Shellcode:
      - Next, encrypt the encoded shellcode using the AES encryptor.
      - Add the AES key and the encrypted shellcode in demon.x64.h.
   
   - Build the Project:
      - Build the solution in Visual Studio.
      - Run the Project:
      - Execute the built binary file.

