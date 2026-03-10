[readme.md](https://github.com/user-attachments/files/25824290/readme.md)
# Shellcode Mutator and APC Injection Tool

This project demonstrates various offensive security techniques including XOR encoding/decoding, AES encryption, shellcode mutation, and APC injection. The purpose of this project is to provide a tool for educational and ethical hacking purposes. Use this tool responsibly and only in environments where you have permission to test.

## Techniques Used

1. **XOR Encoding/Decoding**: A simple encoding technique used to obfuscate data.
2. **AES Decryption**: Uses the Windows CryptoAPI to decrypt data.
3. **Shellcode Mutation**: Adds NOPs to the shellcode to evade signature-based detection.
4. **APC Injection**: Uses Asynchronous Procedure Calls to execute shellcode in a remote process.

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

