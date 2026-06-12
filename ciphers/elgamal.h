#pragma once

#include <string>
#include <vector>
#include <cstdint>
enum class ElgamalChoise {
    Exit = 0,
    GenerateKey = 1,
    Encrypt = 2
};
struct CipherPair {
    int64_t a;
    int64_t b;
};
// Поиск первообразного корня g 
int64_t findPrimitiveRoot(int64_t p);
// Генерация параметров ключей получателя
void generateElGamalKeys(int64_t &p, int64_t &g, int64_t &x, int64_t &y);

// Консольный 
std::vector<CipherPair> encryptBytesElGamal(const std::vector<uint8_t> &data, int64_t p, int64_t g, int64_t y);
std::vector<uint8_t> decryptBytesElGamal(const std::vector<CipherPair> &cipher, int64_t p, int64_t x);
std::vector<uint8_t> cipherToBytes(const std::vector<CipherPair>& cipher);
std::vector<CipherPair> bytesToCipher(const std::vector<uint8_t>& bytes);