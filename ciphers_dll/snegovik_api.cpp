#include <cstdint>
#include <cstddef>
#include <vector>
#include <algorithm>

// Подключаем ваши оригинальные заголовочные файлы
#include "../ciphers/elgamal.h"
#include "../ciphers/shamir.h"
#include "../ciphers/des.h"
#include "../ciphers/rsa.h" 
#include "../ciphers/rc4.h"
#include "../ciphers/rc5.h" 
// Макрос экспорта для DLL
#define SNEGOVIK_API __declspec(dllexport);

extern "C" {

    // =========================================================================
    // БЛОК 1: ГЕНЕРАЦИЯ КЛЮЧЕЙ
    // =========================================================================

    // --- 1. Генерация ключа для DES ---
    // Возвращает сгенерированное 64-битное число прямо как результат функции
    SNEGOVIK_API uint64_t DllGenerateDesKey() {
        return generateDesKey(); // Вызываем твою родную функцию из des.cpp
    }

    // --- 2. Генерация ключей для Эль-Гамаля ---
    // Так как у Эль-Гамаля целых 4 ключа (p, g, x, y), мы принимаем указатели на них,
    // чтобы функция записала сгенерированные значения в память внешней программы.
    SNEGOVIK_API void DllGenerateElGamalKeys(int64_t* p, int64_t* g, int64_t* x, int64_t* y) {
        if (!p || !g || !x || !y) return;
        
        // Вызываем твою оригинальную функцию генерации из elgamal.cpp
        generateElGamalKeys(*p, *g, *x, *y);
    }

    // --- 3. Генерация ключа для RC4 ---
    // Поскольку ключ RC4 — это массив байт произвольной длины, мы принимаем 
    // желаемую длину и пустой буфер, куда запишем сгенерированный ключ.
    SNEGOVIK_API void DllGenerateRC4Key(size_t len, uint8_t* outputKey) {
        if (!outputKey || len == 0) return;

        // Вызываем твой генератор ключей RC4
        std::vector<uint8_t> keyVector = generateRC4Key(len);

        // Копируем байты сгенерированного ключа во внешнюю память
        std::copy(keyVector.begin(), keyVector.end(), outputKey);
    }


    // =========================================================================
    // БЛОК 2: ШИФРОВАНИЕ И ДЕШИФРОВАНИЕ
    // =========================================================================

    // --- DES: Шифрование ---
    SNEGOVIK_API void DllEncryptDES(const uint8_t* input, size_t size, uint64_t key, uint8_t* output) {
        if (!input || !output || size == 0) return;
        std::vector<uint8_t> buffer(input, input + size);
        std::vector<uint8_t> result = desEncrypt(buffer, key);
        std::copy(result.begin(), result.end(), output);
    }

    // --- DES: Дешифрование ---
    SNEGOVIK_API void DllDecryptDES(const uint8_t* input, size_t size, uint64_t key, uint8_t* output) {
        if (!input || !output || size == 0) return;
        std::vector<uint8_t> buffer(input, input + size);
        std::vector<uint8_t> result = desDecrypt(buffer, key);
        std::copy(result.begin(), result.end(), output);
    }

    // --- ЭЛЬ-ГАМАЛЬ: Шифрование ---
    SNEGOVIK_API size_t DllEncryptElGamal(const uint8_t* inputData, size_t dataSize, int64_t p, int64_t g, int64_t y, uint8_t* outputBytes) {
        if (!inputData || !outputBytes || dataSize == 0) return 0;
        std::vector<uint8_t> buffer(inputData, inputData + dataSize);
        std::vector<uint8_t> cipherResult = encryptBytesElGamal(buffer, p, g, y);
        
        // Используем твои функции конвертации
        std::vector<uint8_t> finalBytes = cipherToBytes(cipherResult);
        std::copy(finalBytes.begin(), finalBytes.end(), outputBytes);
        return finalBytes.size();
    }

    // --- ЭЛЬ-ГАМАЛЬ: Дешифрование ---
    SNEGOVIK_API void DllDecryptElGamal(const uint8_t* encryptedBytes, size_t encryptedSize, int64_t p, int64_t x, uint8_t* outputDecryptedData) {
        if (!encryptedBytes || !outputDecryptedData || encryptedSize == 0) return;
        std::vector<uint8_t> buffer(encryptedBytes, encryptedBytes + encryptedSize);
        std::vector<CipherPair> cipherStructs = bytesToCipher(buffer);
        std::vector<uint8_t> decryptedResult = decryptBytesElGamal(cipherStructs, p, x);
        std::copy(decryptedResult.begin(), decryptedResult.end(), outputDecryptedData);
    }
}