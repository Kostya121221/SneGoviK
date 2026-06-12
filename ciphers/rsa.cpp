
#include "rsa.h"
#include "../scripts/crypto_math.h"
#include <stdexcept>

RSAKeys rsaGenerateKeys(RsaKeyGenOptions mode, int64_t custom_p, int64_t custom_q) {
    int64_t p = 0, q = 0;

    switch (mode) {
        case RsaKeyGenOptions::Auto: {
            p = generateSafePrime(500, 5000);
            q = generateSafePrime(501, 5000);
            while (p == q) {
                q = generateSafePrime(501, 5000);
            }
            break;
        }
        case RsaKeyGenOptions::Manual: {
            if (custom_p == custom_q) {
                throw std::invalid_argument("Ошибка: Числа p и q должны быть разными.");
            }
            if (!isPrimeMillerRabin(custom_p, 10) || !isPrimeMillerRabin(custom_q, 10)) {
                throw std::invalid_argument("Ошибка: Числа p и q должны быть строго простыми.");
            }
            p = custom_p;
            q = custom_q;
            break;
        }
        default:
            throw std::invalid_argument("Ошибка: Неверный выбор режима генерации.");
    }

    RSAKeys keys;
    keys.p = p;
    keys.q = q;
    keys.n = p * q;

    if (keys.n <= 256) {
        throw std::invalid_argument("Ошибка: Произведение p*q должно быть больше 256 для побайтового шифрования.");
    }

    int64_t phi = (p - 1) * (q - 1);

    keys.e = 3;
    while (modInverse(keys.e, phi) == -1 && keys.e < phi) {
        keys.e += 2;
    }

    keys.d = modInverse(keys.e, phi);
    if (keys.d == -1) {
        throw std::runtime_error("Ошибка: Не удалось вычислить закрытый ключ.");
    }

    return keys;
}


std::vector<uint8_t> rsaEncrypt(const std::vector<uint8_t>& plaintext, int64_t e, int64_t n) {
    if (n <= 256) {
        throw std::invalid_argument("Ошибка: Модуль n должен быть больше 256.");
    }

    std::vector<uint8_t> ciphertext;
    ciphertext.reserve(plaintext.size() * 8);

    for (uint8_t byte : plaintext) {
        int64_t m = byte;
        int64_t c = powerBinary(m, e, n); // Твой powerBinary

        for (int32_t i = 7; i >= 0; --i) {
            ciphertext.push_back(static_cast<uint8_t>((c >> (i * 8)) & 0xFF));
        }
    }
    return ciphertext;
}

std::vector<uint8_t> rsaDecrypt(const std::vector<uint8_t>& ciphertext, int64_t d, int64_t n) {
    if (ciphertext.size() % 8 != 0) {
        throw std::invalid_argument("Ошибка: Размер зашифрованных данных не кратен 8 байтам.");
    }

    std::vector<uint8_t> plaintext;
    plaintext.reserve(ciphertext.size() / 8);

    for (size_t i = 0; i < ciphertext.size(); i += 8) {
        int64_t c = 0;
        for (int32_t j = 0; j < 8; ++j) {
            c |= (static_cast<int64_t>(ciphertext[i + j]) << ((7 - j) * 8));
        }

        int64_t m = powerBinary(c, d, n);
        plaintext.push_back(static_cast<uint8_t>(m & 0xFF));
    }
    return plaintext;
}