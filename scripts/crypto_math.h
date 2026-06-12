#pragma once
#include <random>
#include <vector>
#include <cstdint> 

// Проверка числа на простоту
bool isPrime(int64_t n);


// Быстрое возведение в степень (Двоичное разложение)
int64_t powerBinary(int64_t a, int64_t x, int64_t p);

//  Расширенный алгоритм Евклида (поиск коэффициентов u, v)
int64_t extendedGCD(int64_t a, int64_t b, int64_t &u, int64_t &v);

//Поиск взаимообратного числа c^-1 mod m
int64_t modInverse(int64_t c, int64_t m);

//тест Миллера-Рябина
bool isPrimeMillerRabin(int64_t n, int k = 20);
//Генерация простых чисел
int64_t generateSafePrime(int64_t min_val, int64_t max_val);

