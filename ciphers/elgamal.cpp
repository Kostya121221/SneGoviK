#include "elgamal.h"
#include "../scripts/crypto_math.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <iomanip>


int64_t findPrimitiveRoot(int64_t p) {
    int64_t q = (p - 1) / 2;
    
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dist(2, p - 2);

    while (true) {
        int64_t g = dist(gen);
        // Для безопасного простого числа g является первообразным корнем,
        // если g^2 != 1 (mod p) и g^q != 1 (mod p)
        if (powerBinary(g, 2, p) != 1 && powerBinary(g, q, p) != 1) {
            return g;
        }
    }
}
void generateElGamalKeys(int64_t &p, int64_t &g, int64_t &x, int64_t &y) {
    // Задаем диапазон для поиска
    int64_t min_val = 100000000; // Минимальная граница
    int64_t max_val = 2000000000; // Максимальная граница

    // Генерируем случайное простое p
    p = generateSafePrime(min_val, max_val);

    // Находим случайный первообразный корень g
    g = findPrimitiveRoot(p);

    // Выбираем случайный закрытый ключ x в диапазоне [2, p-2]
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dist(2, p - 2);
    x = dist(gen);

    // Вычисляем открытый ключ y
    y = powerBinary(g, x, p);

    std::cout << "\n=== Ключи системы Эль-Гамаля успешно СГЕНЕРИРОВАНЫ ===\n";
    std::cout << "Открытый ключ (p, g, y): (" << p << ", " << g << ", " << y << ")\n";
    std::cout << "Закрытый (секретный) ключ x: " << x << "\n";
}

std::vector<CipherPair> encryptBytesElGamal(const std::vector<uint8_t> &data, int64_t p, int64_t g, int64_t y) {
    std::cout << "\n--- Шифрование вектора байт по схеме Эль-Гамаля ---\n";
    std::vector<CipherPair> result;
    
    if (data.empty()) return result;
    std::random_device rd;
    std::mt19937_64 gen(rd());

    // Диапазон для k: [2, p - 2]
    std::uniform_int_distribution<int64_t> distrib(2, p - 2);

    int64_t k = 0;
    int64_t dummy_u = 0, dummy_v = 0; // Временные переменные для вызова функции

    // Цикл генерации k с проверкой через extendedGCD
    while (true) {
        k = distrib(gen);
    
        // Если НОД(k, p - 1) равен 1, значит число подходит
        if (extendedGCD(k, p - 1, dummy_u, dummy_v) == 1) {
            break;
        }
    }
    int64_t a = powerBinary(g, k, p); 
    int64_t y_k = powerBinary(y, k, p); 
    
    std::cout << "[Шифрование]: Сессионная константа а = " << a << ", общий множитель y^k = " << y_k << "\n";
    
    result.reserve(data.size()); 
    for (uint8_t byte : data) {
        int64_t b = (static_cast<int64_t>(byte) * y_k) % p; 
        result.push_back({a, b});
    }
    
    std::cout << "[Успех]: Данные зашифрованы. Сформировано " << result.size() << " пар (a, b).\n";
    return result;
}

std::vector<uint8_t> decryptBytesElGamal(const std::vector<CipherPair> &cipher, int64_t p, int64_t x) {
    std::cout << "\n--- Дешифрование вектора байт по схеме Эль-Гамаля ---\n";
    if (cipher.empty()) return {};
    
    int64_t a = cipher[0].a;
    int64_t ax = powerBinary(a, x, p);     
    int64_t ax_inv = modInverse(ax, p);    
    
    std::cout << "[Дешифрование]: Общий делитель a^x = " << ax << ", его модульная инверсия = " << ax_inv << "\n";
    
    std::vector<uint8_t> decryptedData;
    decryptedData.reserve(cipher.size()); 
    
    for (const auto &pair : cipher) {
        int64_t M = (pair.b * ax_inv) % p;
        decryptedData.push_back(static_cast<uint8_t>(M));
    }
    
    return decryptedData;
}

#include <vector>
#include <cstdint>

// Из вектора структур в вектор байт
std::vector<uint8_t> cipherToBytes(const std::vector<CipherPair>& cipher) {
    if (cipher.empty()) return {};

    // Указатель на начало данных структур приводим к указателю на байты
    const uint8_t* bytePtr = reinterpret_cast<const uint8_t*>(cipher.data());
    size_t totalBytes = cipher.size() * sizeof(CipherPair);

    //скопируем память по диапазону указателей
    std::cout << std::dec << std::endl;
    return std::vector<uint8_t>(bytePtr, bytePtr + totalBytes);
}

// Из вектора байт в вектор структур
std::vector<CipherPair> bytesToCipher(const std::vector<uint8_t>& bytes) {
    if (bytes.empty() || (bytes.size() % sizeof(CipherPair) != 0)) return {};

    const CipherPair* structPtr = reinterpret_cast<const CipherPair*>(bytes.data());
    size_t totalPairs = bytes.size() / sizeof(CipherPair);

    return std::vector<CipherPair>(structPtr, structPtr + totalPairs);
}
