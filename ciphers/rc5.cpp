#include "rc5.h"
#include <algorithm>
#include <random>
#include <cstring>

// Константы для w=32
static const uint32_t Pw = 0xB7E15163;
static const uint32_t Qw = 0x9E3779B9;

static inline uint32_t rol(uint32_t x, uint32_t s) {
    s &= 0x1F;
    return (x << s) | (x >> (32 - s));
}

static inline uint32_t ror(uint32_t x, uint32_t s) {
    s &= 0x1F;
    return (x >> s) | (x << (32 - s));
}

// Развёртка ключа
static std::vector<uint32_t> keyExpansion(const std::vector<uint8_t>& key, unsigned int rounds) {
    size_t t = 2 * (rounds + 1);
    std::vector<uint32_t> S(t);
    S[0] = Pw;
    for (size_t i = 1; i < t; ++i)
        S[i] = S[i - 1] + Qw;

    size_t keyWords = (key.size() + 3) / 4;
    std::vector<uint32_t> L(keyWords, 0);
    for (size_t i = 0; i < key.size(); ++i)
        L[i / 4] |= static_cast<uint32_t>(key[i]) << (8 * (i % 4));

    size_t max_len = std::max(t, keyWords);
    uint32_t A = 0, B = 0;
    for (size_t k = 0; k < 3 * max_len; ++k) {
        A = S[k % t] = rol(S[k % t] + A + B, 3);
        B = L[k % keyWords] = rol(L[k % keyWords] + A + B, A + B);
    }
    return S;
}

// Паддинг PKCS7 (встроен, как в DES)
static void pkcs7Pad(std::vector<uint8_t>& data, size_t block_size) {
    size_t pad_len = block_size - (data.size() % block_size);
    data.insert(data.end(), pad_len, static_cast<uint8_t>(pad_len));
}

static bool pkcs7Unpad(std::vector<uint8_t>& data) {
    if (data.empty()) return false;
    uint8_t pad_len = data.back();
    if (pad_len == 0 || pad_len > data.size()) return false;
    for (size_t i = data.size() - pad_len; i < data.size(); ++i)
        if (data[i] != pad_len) return false;
    data.resize(data.size() - pad_len);
    return true;
}

// Шифрование одного блока (8 байт)
static void rc5EncryptBlock(uint32_t& A, uint32_t& B, const std::vector<uint32_t>& S, unsigned int rounds) {
    A += S[0];
    B += S[1];
    for (unsigned int i = 1; i <= rounds; ++i) {
        A = rol(A ^ B, B) + S[2 * i];
        B = rol(B ^ A, A) + S[2 * i + 1];
    }
}

// Дешифрование одного блока
static void rc5DecryptBlock(uint32_t& A, uint32_t& B, const std::vector<uint32_t>& S, unsigned int rounds) {
    for (unsigned int i = rounds; i >= 1; --i) {
        B = ror(B - S[2 * i + 1], A) ^ A;
        A = ror(A - S[2 * i], B) ^ B;
    }
    B -= S[1];
    A -= S[0];
}

// Генерация ключа
std::vector<uint8_t> generateRC5Key(size_t length) {
    std::vector<uint8_t> key(length);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    for (size_t i = 0; i < length; ++i)
        key[i] = static_cast<uint8_t>(dist(gen));
    return key;
}

// Шифрование данных
std::vector<uint8_t> rc5Encrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key, unsigned int rounds) {
    std::vector<uint8_t> padded = data;
    pkcs7Pad(padded, 8);

    std::vector<uint32_t> S = keyExpansion(key, rounds);
    std::vector<uint8_t> result;
    result.reserve(padded.size());

    for (size_t i = 0; i < padded.size(); i += 8) {
        uint32_t A = 0, B = 0;
        for (int j = 0; j < 4; ++j) {
            A |= static_cast<uint32_t>(padded[i + j]) << (8 * j);
            B |= static_cast<uint32_t>(padded[i + j + 4]) << (8 * j);
        }
        rc5EncryptBlock(A, B, S, rounds);
        for (int j = 0; j < 4; ++j) {
            result.push_back(static_cast<uint8_t>((A >> (8 * j)) & 0xFF));
        }
        for (int j = 0; j < 4; ++j) {
            result.push_back(static_cast<uint8_t>((B >> (8 * j)) & 0xFF));
        }
    }
    return result;
}

// Дешифрование данных
std::vector<uint8_t> rc5Decrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key, unsigned int rounds) {
    if (data.size() % 8 != 0 || data.empty())
        return {};

    std::vector<uint32_t> S = keyExpansion(key, rounds);
    std::vector<uint8_t> result;
    result.reserve(data.size());

    for (size_t i = 0; i < data.size(); i += 8) {
        uint32_t A = 0, B = 0;
        for (int j = 0; j < 4; ++j) {
            A |= static_cast<uint32_t>(data[i + j]) << (8 * j);
            B |= static_cast<uint32_t>(data[i + j + 4]) << (8 * j);
        }
        rc5DecryptBlock(A, B, S, rounds);
        for (int j = 0; j < 4; ++j) {
            result.push_back(static_cast<uint8_t>((A >> (8 * j)) & 0xFF));
        }
        for (int j = 0; j < 4; ++j) {
            result.push_back(static_cast<uint8_t>((B >> (8 * j)) & 0xFF));
        }
    }

    if (!pkcs7Unpad(result))
        return {}; 
    return result;
}