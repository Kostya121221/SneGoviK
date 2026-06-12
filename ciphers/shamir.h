#pragma once

#include <vector>
#include <cstdint>

enum class ShamirChoise{
    Exit,
    Sender,
    Recipient
};
enum class ShamirKesyGenerate{
    Exit,
    AbsoluteGen,
    GenerateByP
};
// Функция 1: Генерирует случайное безопасное простое число p
int64_t shamirGeneratePrime();

// Функция 2: Подбирает секретный ключ под уже готовое (сгенерированное или введённое) число p
int64_t shamirGenerateKeyForPrime(int64_t p);
// Шаг 1: Превращение исходных байт в 8-байтовые блоки (Шифрование Алисы)
std::vector<uint8_t> shamirStartEncrypt(const std::vector<uint8_t>& plaintext, int64_t key, int64_t p);

// Шаги 2 и 3: Промежуточная обработка 8-байтовых блоков (Шифрование Боба / Дешифрование Алисы)
std::vector<uint8_t> shamirProcessBlocks(const std::vector<uint8_t>& blockData, int64_t key, int64_t p);

// Шаг 4: Финальное сжатие 8-байтовых блоков в исходные байты (Дешифрование Боба)
std::vector<uint8_t> shamirFinalDecrypt(const std::vector<uint8_t>& blockData, int64_t key, int64_t p);