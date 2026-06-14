#pragma once

#include <vector>
#include <cstdint>
#include <string>
extern "C" {
// Генерация случайного 64-битного ключа
uint64_t generateDesKey();

// Шифрование вектора байт (применяется PKCS#7 padding и режим ECB)
std::vector<uint8_t> desEncrypt(const std::vector<uint8_t>& plaintext, uint64_t key);

// Расшифрование вектора байт
std::vector<uint8_t> desDecrypt(const std::vector<uint8_t>& ciphertext, uint64_t key);

}