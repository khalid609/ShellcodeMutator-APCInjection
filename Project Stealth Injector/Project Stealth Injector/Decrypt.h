#include <windows.h>
#include <bcrypt.h>
#include <vector>
#include <iostream>

#pragma comment(lib, "bcrypt.lib")

std::vector<uint8_t> AESDecrypt(const std::vector<uint8_t>& ciphertext, const std::vector<uint8_t>& key) {
    BCRYPT_ALG_HANDLE hAlg = NULL;
    BCRYPT_KEY_HANDLE hKey = NULL;
    std::vector<uint8_t> plaintext;
    NTSTATUS status;

    // Validate input
    if (ciphertext.empty() || key.empty()) {
        return plaintext;
    }

    // Check key size (AES: 16, 24, or 32 bytes)
    if (key.size() != 16 && key.size() != 24 && key.size() != 32) {
        return plaintext;
    }

    // Check ciphertext block alignment (must be multiple of 16 for AES/ECB)
    if (ciphertext.size() % 16 != 0) {
        return plaintext;
    }

    // Open algorithm provider
    status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, NULL, 0);
    if (status != 0) {
        return plaintext;
    }

    // Set chaining mode to ECB
    status = BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, (PUCHAR)BCRYPT_CHAIN_MODE_ECB, sizeof(BCRYPT_CHAIN_MODE_ECB), 0);
    if (status != 0) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return plaintext;
    }

    // Generate key
    status = BCryptGenerateSymmetricKey(hAlg, &hKey, NULL, 0, (PUCHAR)key.data(), static_cast<ULONG>(key.size()), 0);
    if (status != 0) {
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return plaintext;
    }

    // Determine required output size
    ULONG requiredSize = 0;
    status = BCryptDecrypt(hKey, (PUCHAR)ciphertext.data(), static_cast<ULONG>(ciphertext.size()), NULL, NULL, 0, NULL, 0, &requiredSize, 0);
    if (status != 0) {
        if (hKey) BCryptDestroyKey(hKey);
        BCryptCloseAlgorithmProvider(hAlg, 0);
        return plaintext;
    }

    // Allocate output buffer and perform decryption
    plaintext.resize(requiredSize);
    ULONG resultSize = 0;
    status = BCryptDecrypt(hKey, (PUCHAR)ciphertext.data(), static_cast<ULONG>(ciphertext.size()), NULL, NULL, 0, plaintext.data(), requiredSize, &resultSize, 0);
    if (status != 0) {
        plaintext.clear();
    }
    else if (resultSize != requiredSize) {
        plaintext.resize(resultSize);
    }

    // Cleanup
    if (hKey) BCryptDestroyKey(hKey);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    return plaintext;
}
