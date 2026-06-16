#include "shamir.h"
#include "../scripts/crypto_math.h"
#include <stdexcept>

extern "C" {

int64_t shamirGeneratePrime() {

    return generateSafePrime(10000, 1000000);

}

// Функция 2: Работает с уже готовым p и подбирает под него ключ 
int64_t shamirGenerateKeyForPrime(int64_t p) {
    if (p <= 256 || !isPrimeMillerRabin(p, 10)) {
        throw std::invalid_argument("Ошибка: Число p должно быть больше 256 и строго простым.");
    }
    std::random_device rd;
    std::mt19937_64 gen(rd());
    
    std::uniform_int_distribution<int64_t> dist(3, p - 2);

    int64_t c = 0;
    int32_t attempts = 0;
    const int32_t maxAttempts = 10000; // Защита от бесконечного цикла, если что-то пойдет не так

    while (attempts < maxAttempts) {
        c = dist(gen);
        
        if (modInverse(c, p - 1) != -1) {
            return c; // Нашли случайный 
        }
        
        attempts++;
    }

    throw std::runtime_error("Ошибка: Не удалось подобрать случайный ключ для данного числа p за отведенное число попыток.");
}

// Шаг 1: Исходные байты -> 8-байтовые блоки
std::vector<uint8_t> shamirStartEncrypt(const std::vector<uint8_t>& plaintext, int64_t key, int64_t p) {
    std::vector<uint8_t> result;
    result.reserve(plaintext.size() * 8);
    
    for (uint8_t byte : plaintext) {
        int64_t m = byte;
        int64_t c = powerBinary(m, key, p);
        
        for (int32_t i = 7; i >= 0; --i) {
            result.push_back(static_cast<uint8_t>((c >> (i * 8)) & 0xFF));
        }
    }
    return result;
}

// Шаги 2 и 3: Промежуточная обработка блоков
std::vector<uint8_t> shamirProcessBlocks(const std::vector<uint8_t>& blockData, int64_t key, int64_t p) {
    if (blockData.size() % 8 != 0) {
        throw std::invalid_argument("Ошибка: Размер данных не кратен 8 байтам. Файл поврежден.");
    }
    
    std::vector<uint8_t> result;
    result.reserve(blockData.size());

    for (size_t i = 0; i < blockData.size(); i += 8) {
        int64_t c = 0;
        for (int32_t j = 0; j < 8; ++j) {
            c |= (static_cast<int64_t>(blockData[i + j]) << ((7 - j) * 8));
        }
        
        int64_t next_c = powerBinary(c, key, p);
        
        for (int32_t i = 7; i >= 0; --i) {
            result.push_back(static_cast<uint8_t>((next_c >> (i * 8)) & 0xFF));
        }
    }
    return result;
}

// Шаг 4: 8-байтовые блоки -> исходные байты
std::vector<uint8_t> shamirFinalDecrypt(const std::vector<uint8_t>& blockData, int64_t key, int64_t p) {
    if (blockData.size() % 8 != 0) {
        throw std::invalid_argument("Ошибка: Размер данных не кратен 8 байтам. Файл поврежден.");
    }
    
    std::vector<uint8_t> result;
    result.reserve(blockData.size() / 8);

    for (size_t i = 0; i < blockData.size(); i += 8) {
        int64_t c = 0;
        for (int32_t j = 0; j < 8; ++j) {
            c |= (static_cast<int64_t>(blockData[i + j]) << ((7 - j) * 8));
        }
        
        int64_t m = powerBinary(c, key, p);
        result.push_back(static_cast<uint8_t>(m & 0xFF));
    }
    return result;
}}