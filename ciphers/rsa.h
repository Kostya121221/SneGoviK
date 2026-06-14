#pragma once

#include <vector>
#include <cstdint>

// Структура для возврата полной ключевой пары RSA
enum class RsaKeyGenOptions : int32_t {
    Exit = 0,
    Auto = 1,
    Manual = 2
};
struct RSAKeys {
    int64_t n; // Общий модуль
    int64_t e; // Открытая экспонента (открытый ключ)
    int64_t d; // Секретная экспонента (закрытый ключ)
    int64_t p; // Число p 
    int64_t q; // Число q 
};

// Функция генерации ключей
RSAKeys rsaGenerateKeys(RsaKeyGenOptions mode, int64_t custom_p = 0, int64_t custom_q = 0);

// Функция шифрования (каждый байт -> 8 байт)
std::vector<uint8_t> rsaEncrypt(const std::vector<uint8_t>& plaintext, int64_t e, int64_t n);

// Функция дешифрования (каждые 8 байт -> 1 байт)
std::vector<uint8_t> rsaDecrypt(const std::vector<uint8_t>& ciphertext, int64_t d, int64_t n);