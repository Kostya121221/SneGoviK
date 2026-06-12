#include "des.h"
#include <random>
#include <stdexcept>

// Начальная перестановка (Initial Permutation - IP)
static const uint8_t IP[] = {
    58, 50, 42, 34, 26, 18, 10, 2, 60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6, 64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9,  1, 59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5, 63, 55, 47, 39, 31, 23, 15, 7
};

// Конечная перестановка (Final Permutation - FP / Inverse IP)
static const uint8_t FP[] = {
    40, 8, 48, 16, 56, 24, 64, 32, 39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30, 37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28, 35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26, 33, 1, 41, 9,  49, 17, 57, 25
};

// Таблица расширения (E-box) с 32 до 48 бит
static const uint8_t E[] = {
    32, 1,  2,  3,  4,  5,  4,  5,  6,  7,  8,  9,
    8,  9,  10, 11, 12, 13, 12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21, 20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29, 28, 29, 30, 31, 32, 1
};

// Перестановка P внутри функции Фейстеля (32 бита)
static const uint8_t P[] = {
    16, 7,  20, 21, 29, 12, 28, 17, 1,  15, 23, 26, 5,  18, 31, 10,
    2,  8,  24, 14, 32, 27, 3,  9,  19, 13, 30, 6,  22, 11, 4,  25
};

// Выбор ключа PC-1 (сокращение 64 бит до 56 бит)
static const uint8_t PC1[] = {
    57, 49, 41, 33, 25, 17, 9,  1,  58, 50, 42, 34, 26, 18,
    10, 2,  59, 51, 43, 35, 27, 19, 11, 3,  60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15, 7,  62, 54, 46, 38, 30, 22,
    14, 6,  61, 53, 45, 37, 29, 21, 13, 5,  28, 20, 12, 4
};

// Выбор ключа PC-2 (сокращение 56 бит до 48-битных ключей)
static const uint8_t PC2[] = {
    14, 17, 11, 24, 1,  5,  3,  28, 15, 6,  21, 10,
    23, 19, 12, 4,  26, 8,  16, 7,  27, 20, 13, 2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

// Количество циклических сдвигов влево для каждого из 16 раундов
static const uint8_t SHIFTS[] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

// Таблицы замен S-Boxes 
static const uint8_t S_BOXES[8][64] = {
    // S1
    {
        14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
        0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
        4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
        15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13
    },
    // S2
    {
        15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
        3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
        0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
        13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9
    },
    // S3
    {
        10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
        13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
        13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
        1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12
    },
    // S4
    {
        7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
        13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
        10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
        3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14
    },
    // S5
    {
        2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
        14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
        4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
        11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3
    },
    // S6
    {
        12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
        10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
        9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
        4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13
    },
    // S7
    {
        4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
        13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
        1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
        6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12
    },
    // S8
    {
        13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
        1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
        7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
        2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
    }
};

uint64_t generateDesKey() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    return dis(gen);
}

static uint64_t permute(uint64_t input, const uint8_t* table, int32_t table_size, int32_t input_size) {
    uint64_t output = 0;
    for (int32_t i = 0; i < table_size; ++i) {
        // Вычисляем позицию бита в стиле DES 
        int32_t bit_pos = input_size - table[i];
        
        // Извлекаем конкретный бит
        uint64_t bit = (input >> bit_pos) & 1ULL;
        
        // Записываем его в выходное значение слева направо
        output |= (bit << (table_size - 1 - i));
    }
    return output;
}

static void generateSubkeys(uint64_t key, uint64_t subkeys[16]) {
    uint64_t key56 = permute(key, PC1, 56, 64);
    uint32_t C = static_cast<uint32_t>((key56 >> 28) & 0x0FFFFFFF);
    uint32_t D = static_cast<uint32_t>(key56 & 0x0FFFFFFF);

    for (int32_t i = 0; i < 16; ++i) {
        C = ((C << SHIFTS[i]) | (C >> (28 - SHIFTS[i]))) & 0x0FFFFFFF;
        D = ((D << SHIFTS[i]) | (D >> (28 - SHIFTS[i]))) & 0x0FFFFFFF;
        uint64_t combined = (static_cast<uint64_t>(C) << 28) | D;
        subkeys[i] = permute(combined, PC2, 48, 56);
    }
}

static uint32_t feistelFunction(uint32_t R, uint64_t subkey) {
    uint64_t expanded_R = permute(R, E, 48, 32);
    uint64_t xor_res = expanded_R ^ subkey;
    uint32_t s_box_output = 0;

    for (int32_t i = 0; i < 8; ++i) {
        uint8_t chunk = static_cast<uint8_t>((xor_res >> (42 - i * 6)) & 0x3F);
        
        uint8_t row = ((chunk & 0x20) >> 4) | (chunk & 0x01);
        uint8_t col = (chunk >> 1) & 0x0F;
        
        uint8_t val = S_BOXES[i][row * 16 + col];
        s_box_output |= (static_cast<uint32_t>(val) << (28 - i * 4));
    }
    return static_cast<uint32_t>(permute(s_box_output, P, 32, 32));
}

static uint64_t desProcessBlock(uint64_t block, const uint64_t subkeys[16], bool decrypt) {
    uint64_t permuted_block = permute(block, IP, 64, 64);
    uint32_t L = static_cast<uint32_t>((permuted_block >> 32) & 0xFFFFFFFF);
    uint32_t R = static_cast<uint32_t>(permuted_block & 0xFFFFFFFF);

    for (int32_t i = 0; i < 16; ++i) {
        uint32_t next_L = R;
        uint64_t current_subkey = subkeys[decrypt ? (15 - i) : i];
        uint32_t next_R = L ^ feistelFunction(R, current_subkey);
        L = next_L;
        R = next_R;
    }

    uint64_t combined = (static_cast<uint64_t>(R) << 32) | L;
    return permute(combined, FP, 64, 64);
}

static uint64_t bytesToU64(const uint8_t* bytes) {
    uint64_t res = 0;
    for (int32_t i = 0; i < 8; ++i) {
        res |= (static_cast<uint64_t>(bytes[i]) << (56 - i * 8));
    }
    return res;
}

static void u64ToBytes(uint64_t val, uint8_t* bytes) {
    for (int32_t i = 0; i < 8; ++i) {
        bytes[i] = static_cast<uint8_t>((val >> (56 - i * 8)) & 0xFF);
    }
}

std::vector<uint8_t> desEncrypt(const std::vector<uint8_t>& plaintext, uint64_t key) {
    uint64_t subkeys[16];
    generateSubkeys(key, subkeys);

    std::vector<uint8_t> padded = plaintext;
    uint8_t padding_len = 8 - (padded.size() % 8);
    for (int32_t i = 0; i < padding_len; ++i) {
        padded.push_back(padding_len);
    }

    std::vector<uint8_t> ciphertext;
    ciphertext.reserve(padded.size());

    for (size_t i = 0; i < padded.size(); i += 8) {
        uint64_t block = bytesToU64(&padded[i]);
        uint64_t encrypted_block = desProcessBlock(block, subkeys, false);
        uint8_t out_bytes[8];
        u64ToBytes(encrypted_block, out_bytes);
        ciphertext.insert(ciphertext.end(), out_bytes, out_bytes + 8);
    }
    return ciphertext;
}

std::vector<uint8_t> desDecrypt(const std::vector<uint8_t>& ciphertext, uint64_t key) {
    if (ciphertext.size() % 8 != 0 || ciphertext.empty()) {
        throw std::invalid_argument("Неверный размер зашифрованных данных.");
    }

    uint64_t subkeys[16];
    generateSubkeys(key, subkeys);

    std::vector<uint8_t> decrypted;
    decrypted.reserve(ciphertext.size());

    for (size_t i = 0; i < ciphertext.size(); i += 8) {
        uint64_t block = bytesToU64(&ciphertext[i]);
        uint64_t decrypted_block = desProcessBlock(block, subkeys, true);
        uint8_t out_bytes[8];
        u64ToBytes(decrypted_block, out_bytes);
        decrypted.insert(decrypted.end(), out_bytes, out_bytes + 8);
    }

    uint8_t padding_len = decrypted.back();
    if (padding_len > 8 || padding_len == 0) {
        throw std::runtime_error("Ошибка целостности данных или неверный ключ.");
    }
    decrypted.resize(decrypted.size() - padding_len);

    return decrypted;
}