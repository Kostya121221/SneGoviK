#include "elgamal.h"
#include "../scripts/crypto_math.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <iomanip>


void generateElGamalKeys(int64_t &p, int64_t &g, int64_t &x, int64_t &y) {
    p = 2423; 
    g = 5;    
    x = 103;  
    
    y = powerBinary(g, x, p);
    
    std::cout << "\n=== Ключи системы Эль-Гамаля успешно инициализированы ===\n";
    std::cout << "Открытый ключ (p, g, y): (" << p << ", " << g << ", " << y << ")\n";
    std::cout << "Закрытый (секретный) ключ x: " << x << "\n";
}

std::vector<CipherPair> encryptBytesElGamal(const std::vector<uint8_t> &data, int64_t p, int64_t g, int64_t y) {
    std::cout << "\n--- Шифрование вектора байт по схеме Эль-Гамаля ---\n";
    std::vector<CipherPair> result;
    
    if (data.empty()) return result;

    int64_t k = 151;
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
