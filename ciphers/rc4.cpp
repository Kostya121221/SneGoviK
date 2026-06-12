
#include "rc4.h"
#include <algorithm>
#include <random>

std::vector<uint8_t> generateRC4Key(size_t length) {
    std::vector<uint8_t> key(length);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);

    for (size_t i = 0; i < length; ++i)
        key[i] = static_cast<uint8_t>(dist(gen));
    return key;
}

std::vector<uint8_t> rc4Transform(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key) {
    std::vector<uint8_t> S(256);
    for (int i = 0; i < 256; ++i) 
        S[i] = i;

    int j = 0;
    for (int i = 0; i < 256; ++i) {
        j = (j + S[i] + key[i % key.size()]) % 256;
        std::swap(S[i], S[j]);
    }

    std::vector<uint8_t> result = data;
    int i = 0;
    j = 0;
    for (size_t k = 0; k < result.size(); ++k) {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        std::swap(S[i], S[j]);
        uint8_t gamma = S[(S[i] + S[j]) % 256];
        result[k] ^= gamma;
    }
    return result;
}