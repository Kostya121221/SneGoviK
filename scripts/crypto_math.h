#pragma once

#include <vector>
#include <cstdint> // Подключаем стандартные типы фиксированной ширины

// Проверка числа на простоту
bool isPrime(int64_t n);

// Модульное возведение в степень (Теорема Ферма)
int64_t powerFermat(int64_t a, int64_t x, int64_t p);

// Быстрое возведение в степень (Двоичное разложение)
int64_t powerBinary(int64_t a, int64_t x, int64_t p);

//  Расширенный алгоритм Евклида (поиск коэффициентов u, v)
int64_t extendedGCD(int64_t a, int64_t b, int64_t &u, int64_t &v);

//Поиск взаимообратного числа c^-1 mod m
int64_t modInverse(int64_t c, int64_t m);

