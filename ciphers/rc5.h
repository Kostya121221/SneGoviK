#pragma once
#include <vector>
#include <cstdint>
//w=32
// Генерация случайного ключа заданной длины (по умолчанию 16 байт)
extern "C" {
std::vector<uint8_t> generateRC5Key(size_t length = 16);

// Шифрование (паддинг PKCS7)
std::vector<uint8_t> rc5Encrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key, unsigned int rounds = 12);

// Дешифрование (снятие паддинга PKCS7)
std::vector<uint8_t> rc5Decrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key, unsigned int rounds = 12);
}