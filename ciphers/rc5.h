#pragma once
#include <vector>
#include <cstdint>
//w=32
// Генерация случайного ключа заданной длины (по умолчанию 16 байт)
std::vector<uint8_t> generateRC5Key(size_t length);

// Шифрование (паддинг PKCS7)
std::vector<uint8_t> rc5Encrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key, unsigned int rounds);

// Дешифрование (снятие паддинга PKCS7)
std::vector<uint8_t> rc5Decrypt(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key, unsigned int rounds);
