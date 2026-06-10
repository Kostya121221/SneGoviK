#include "crypto_math.h"
#include <iostream>
#include <vector>
#include <cstdint>

bool isPrime(int64_t n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (int64_t i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

//Возведение в степень
int64_t powerBinary(int64_t a, int64_t x, int64_t p) {
    std::cout << "\n--- Двоичный алгоритм возведения в степень ---\n";
    int64_t res = 1;
    a = a % p;
    int64_t step = 1;
    int64_t initialX = x;
    
    while (x > 0) {
        if (x % 2 == 1) {
            res = (res * a) % p;
        }
        a = (a * a) % p;
        x /= 2;
    }
    return res;
}
//Алгоритм Евклида
int64_t extendedGCD(int64_t a, int64_t b, int64_t &u, int64_t &v) {
    std::cout << "\n--- Расширенный алгоритм Евклида для пар (" << a << ", " << b << ") ---\n";
    
    std::vector<int64_t> X = {a, 1, 0}; 
    std::vector<int64_t> Y = {b, 0, 1}; 
    
    int64_t step = 1;
    while (Y[0] != 0) { 
        int64_t q = X[0] / Y[0]; 
        std::vector<int64_t> T = {
            X[0] % Y[0],          
            X[1] - q * Y[1],      
            X[2] - q * Y[2]       
        };
        X = Y; 
        Y = T;  
    }
    
    u = X[1];
    v = X[2];
    std::cout << "[Результат Евклида]: НОД = " << X[0] << ", Итоговые u = " << u << ", v = " << v << "\n";
    return X[0];
}

int64_t modInverse(int64_t c, int64_t m) {
    std::cout << "\n--- Нахождение инверсии (" << c << "^-1) mod " << m << " ---\n";
    int64_t u, v;
    int64_t gcd = extendedGCD(m, c, u, v);
    
    if (gcd != 1) {
        std::cout << "[Ошибка]: Обратного элемента не существует, НОД=" << gcd << " != 1\n";
        return -1;
    }
    
    int64_t res = (v % m + m) % m; 
    std::cout << "[Результат]: Взаимообратное число: " << res << "\n";
    return res;
}


//тест Миллера-Рябина
bool isPrimeMillerRabin(int64_t n, int k) {
    if (n < 2) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0) return false;

    int64_t d = n - 1;
    int s = 0;
    while (d % 2 == 0) {
        d /= 2;
        s++;
    }

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dist(2, n - 2);

    for (int i = 0; i < k; i++) {
        int64_t a = dist(gen);
        int64_t x = powerBinary(a, d, n);

        if (x == 1 || x == n - 1) continue;

        bool composite = true;
        for (int r = 1; r < s; r++) {
            x = (static_cast<__int128>(x) * x) % n;
            if (x == n - 1) {
                composite = false;
                break;
            }
        }
        if (composite) return false; 
    }
    return true; 
}

int64_t generateSafePrime(int64_t min_val, int64_t max_val) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dist(min_val, max_val);

    while (true) {
        // Генерируем случайное нечетное число q
        int64_t q = dist(gen) | 1; 
        
        if (isPrimeMillerRabin(q)) {
            int64_t p = 2 * q + 1;
            if (p <= max_val && isPrimeMillerRabin(p)) {
                return p; // p — безопасное простое число
            }
        }
    }
}