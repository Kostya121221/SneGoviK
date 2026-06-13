#pragma once
#include <vector>
#include <cstdint>

// Генерация случайного ключа заданной длины
std::vector<uint8_t> generateRC4Key(size_t length);

// Трансформация вектора байт, шифрование и расшифрование
std::vector<uint8_t> rc4Transform(const std::vector<uint8_t>& data, const std::vector<uint8_t>& key);
