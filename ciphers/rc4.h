#ifndef RC4_H
#define RC4_H
#include <vector>
#include <cstdint>

// Генерация случайного ключа заданной длины
std::vector<uint8_t> generateRC4Key(size_t length = 255);

// Трансформация вектора байт, шифрование и расшифрование
std::vector<uint8_t> rc4Transform(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key);

#endif