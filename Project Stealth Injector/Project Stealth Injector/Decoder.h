#pragma once
#include <vector>
#include <cstdint>
#include <string>

inline void XOR(uint8_t* buffer, size_t size, const std::string& key, int nopCount = 0)
{
    if (!buffer || size == 0 || key.empty()) { return; }
    size_t nops = nopCount > 0 ? static_cast<size_t>(nopCount) : 0;
    if (nops > size) nops = size;

    const size_t keylen = key.length();
    if (keylen == 0) return;

    for (size_t i = 0; i < size; ++i)
    {
        if (i < nops)
        {
            buffer[i] ^= key[0];
        }
        else
        {
            buffer[i] ^= key[(i - nops) % keylen];
        }
    }
}