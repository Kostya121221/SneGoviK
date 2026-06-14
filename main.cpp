#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <clocale>
#include <limits>
#include <stdexcept>
#include <stdio.h>
#include <dlfcn.h>
#ifdef _WIN32git
    #include <windows.h>
#endif

#include <input_output.h>
enum class RsaKeyGenOptions : int32_t {
    Exit = 0,
    Auto = 1,
    Manual = 2
};

struct RSAKeys {
    int64_t n;
    int64_t e;
    int64_t d;
    int64_t p;
    int64_t q;
};
enum class ElgamalChoise {
    Exit = 0,
    GenerateKey = 1,
    Encrypt = 2
};
enum class ShamirChoise{
    Exit,
    Sender,
    Recipient
};
enum class ShamirKesyGenerate{
    Exit,
    AbsoluteGen,
    GenerateByP
};
enum class MenuFunctions : int32_t {
    Exit = 0,
    Encrypt = 1,
    Decrypt = 2,
    Keys = 3
};

enum class MenuEncOptions : int32_t {
    Exit = 0,
    Elgamal = 1,
    DES = 2,
    Shamir = 3,
    RSA = 4,
    RC4 = 5,
    RC5 = 6
};

enum class MenuInputOutput : int32_t {
    Exit = 0,
    File = 1,
    Console = 2
};
struct CipherPair {
    int64_t a;
    int64_t b;
};
template <typename T>
T readNumber(const std::string& prompt = "", T min_val = std::numeric_limits<T>::lowest(), T max_val = std::numeric_limits<T>::max()){
    T value;
        while (true) {
            std::cout << prompt;
            if (std::cin >> value) {
                if (value >= min_val && value <= max_val) {
                    return value;
                }
                std::cout << "[Ошибка] Число вышло за допустимые пределы! Повторите ввод.\n";
            } else {
                std::cout << "[Ошибка] Некорректный ввод! Ожидалось число.\n";
                std::cin.clear(); // Сбрасываем флаг ошибки cin
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очищаем буфер
            }
        }

             }
int main() {
    std::setlocale(LC_ALL, "Russian");
    
    #ifdef _WIN32
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #endif
    
    if (!loginFunc()) {
        std::cout << "Введён неверный пароль\n";
        return 0;
    }
    printMenu(0);
    typedef int64_t (*shamirGeneratePrime_t)();
    typedef int64_t (*shamirGenerateKeyForPrime_t)(int64_t);
    typedef std::vector<uint8_t> (*shamirStartEncrypt_t)(const std::vector<uint8_t>&, int64_t, int64_t);
    typedef std::vector<uint8_t> (*shamirProcessBlocks_t)(const std::vector<uint8_t>&, int64_t, int64_t);
    typedef std::vector<uint8_t> (*shamirFinalDecrypt_t)(const std::vector<uint8_t>&, int64_t, int64_t);

    shamirGeneratePrime_t shamirGeneratePrime = nullptr;
    shamirGenerateKeyForPrime_t shamirGenerateKeyForPrime = nullptr;
    shamirStartEncrypt_t shamirStartEncrypt = nullptr;
    shamirProcessBlocks_t shamirProcessBlocks = nullptr;
    shamirFinalDecrypt_t shamirFinalDecrypt = nullptr;
    void* des_handle = dlopen("libdes.so", RTLD_LAZY);
    if (!des_handle) {
        std::cerr << "Ошибка загрузки .so: " << dlerror() << std::endl;
        return 1;
    }

    typedef uint64_t (*generateKey_t)();
    typedef std::vector<uint8_t> (*encrypt_t)(const std::vector<uint8_t>&, uint64_t);
    typedef std::vector<uint8_t> (*decrypt_t)(const std::vector<uint8_t>&, uint64_t);

    generateKey_t generateDesKey = (generateKey_t)dlsym(des_handle, "generateDesKey");
    encrypt_t desEncrypt = (encrypt_t)dlsym(des_handle, "desEncrypt");
    decrypt_t desDecrypt = (decrypt_t)dlsym(des_handle, "desDecrypt");

    if (!generateDesKey || !desEncrypt || !desDecrypt) {
        std::cerr << "Ошибка поиска функций: " << dlerror() << std::endl;
        dlclose(des_handle);
        return 1;
    }
    void* el_handle = dlopen("libelgamal.so", RTLD_LAZY);
    if (!el_handle) {
        std::cerr << "Ошибка загрузки библиотеки: " << dlerror() << std::endl;
        return 1;
    }

    typedef int64_t (*findPrimitiveRoot_t)(int64_t);
    typedef void (*generateElGamalKeys_t)(int64_t&, int64_t&, int64_t&, int64_t&);
    typedef std::vector<CipherPair> (*encryptBytesElGamal_t)(const std::vector<uint8_t>&, int64_t, int64_t, int64_t);
    typedef std::vector<uint8_t> (*decryptBytesElGamal_t)(const std::vector<CipherPair>&, int64_t, int64_t);
    typedef std::vector<uint8_t> (*cipherToBytes_t)(const std::vector<CipherPair>&);
    typedef std::vector<CipherPair> (*bytesToCipher_t)(const std::vector<uint8_t>&);

    generateElGamalKeys_t generateElGamalKeys = (generateElGamalKeys_t)dlsym(el_handle, "generateElGamalKeys");
    encryptBytesElGamal_t encryptBytesElGamal = (encryptBytesElGamal_t)dlsym(el_handle, "encryptBytesElGamal");
    decryptBytesElGamal_t decryptBytesElGamal = (decryptBytesElGamal_t)dlsym(el_handle, "decryptBytesElGamal");
    cipherToBytes_t cipherToBytes = (cipherToBytes_t)dlsym(el_handle, "cipherToBytes");
    bytesToCipher_t bytesToCipher = (bytesToCipher_t)dlsym(el_handle, "bytesToCipher");

    if (!generateElGamalKeys || !encryptBytesElGamal || !decryptBytesElGamal || !cipherToBytes || !bytesToCipher) {
        std::cerr << "Ошибка поиска функций: " << dlerror() << std::endl;
        dlclose(el_handle);
        return 1;
    }
    void* rc4_handle = dlopen("librc4.so", RTLD_LAZY);
    if (!rc4_handle) {
        std::cerr << "Ошибка загрузки .so: " << dlerror() << std::endl;
        return 1;
    }

    typedef std::vector<uint8_t> (*generateRC4Key_t)(size_t);
    typedef std::vector<uint8_t> (*rc4Transform_t)(const std::vector<uint8_t>&, const std::vector<uint8_t>&);

    generateRC4Key_t generateRC4Key = (generateRC4Key_t)dlsym(rc4_handle, "generateRC4Key");
    rc4Transform_t rc4Transform = (rc4Transform_t)dlsym(rc4_handle, "rc4Transform");

    if (!generateRC4Key || !rc4Transform) {
        std::cerr << "Ошибка поиска функций: " << dlerror() << std::endl;
        dlclose(rc4_handle);
        return 1;
    }

    void* rc5_handle = dlopen("librc5.so", RTLD_LAZY);
    if (!rc5_handle) {
        std::cerr << "Не удалось загрузить RC5: " << dlerror() << std::endl;
        return 1;
    }

    typedef std::vector<uint8_t> (*generateRC5Key_t)(size_t);
    typedef std::vector<uint8_t> (*rc5Encrypt_t)(const std::vector<uint8_t>&, const std::vector<uint8_t>&, unsigned int);
    typedef std::vector<uint8_t> (*rc5Decrypt_t)(const std::vector<uint8_t>&, const std::vector<uint8_t>&, unsigned int);

    generateRC5Key_t generateRC5Key = (generateRC5Key_t)dlsym(rc5_handle, "generateRC5Key");
    rc5Encrypt_t rc5Encrypt = (rc5Encrypt_t)dlsym(rc5_handle, "rc5Encrypt");
    rc5Decrypt_t rc5Decrypt = (rc5Decrypt_t)dlsym(rc5_handle, "rc5Decrypt");

    if (!generateRC5Key || !rc5Encrypt || !rc5Decrypt) {
        std::cerr << "Ошибка загрузки функций RC5: " << dlerror() << std::endl;
        dlclose(rc5_handle);
        return 1;
    }
    void* rsa_handle = dlopen("librsa.so", RTLD_LAZY);
    if (!rsa_handle) {
        std::cerr << "Ошибка загрузки RSA библиотеки: " << dlerror() << std::endl;
        return 1;
    }

    typedef RSAKeys (*rsaGenerateKeys_t)(RsaKeyGenOptions, int64_t, int64_t);
    typedef std::vector<uint8_t> (*rsaEncrypt_t)(const std::vector<uint8_t>&, int64_t, int64_t);
    typedef std::vector<uint8_t> (*rsaDecrypt_t)(const std::vector<uint8_t>&, int64_t, int64_t);

    rsaGenerateKeys_t rsaGenerateKeys = (rsaGenerateKeys_t)dlsym(rsa_handle, "rsaGenerateKeys");
    rsaEncrypt_t rsaEncrypt = (rsaEncrypt_t)dlsym(rsa_handle, "rsaEncrypt");
    rsaDecrypt_t rsaDecrypt = (rsaDecrypt_t)dlsym(rsa_handle, "rsaDecrypt");

    if (!rsaGenerateKeys || !rsaEncrypt || !rsaDecrypt) {
        std::cerr << "Ошибка поиска функций RSA: " << dlerror() << std::endl;
        dlclose(rsa_handle);
        return 1;
    }
    void* shamir_handle = dlopen("libshamir.so", RTLD_LAZY);
    if (!shamir_handle) {
        std::cerr << "Ошибка загрузки библиотеки Шамира: " << dlerror() << std::endl;
        return 1;
    }
    shamirGeneratePrime = (shamirGeneratePrime_t)dlsym(shamir_handle, "shamirGeneratePrime");
    shamirGenerateKeyForPrime = (shamirGenerateKeyForPrime_t)dlsym(shamir_handle, "shamirGenerateKeyForPrime");
    shamirStartEncrypt = (shamirStartEncrypt_t)dlsym(shamir_handle, "shamirStartEncrypt");
    shamirProcessBlocks = (shamirProcessBlocks_t)dlsym(shamir_handle, "shamirProcessBlocks");
    shamirFinalDecrypt = (shamirFinalDecrypt_t)dlsym(shamir_handle, "shamirFinalDecrypt");

    if (!shamirGeneratePrime || !shamirGenerateKeyForPrime || !shamirStartEncrypt || !shamirProcessBlocks || !shamirFinalDecrypt) {
        std::cerr << "Ошибка поиска функций Шамира: " << dlerror() << std::endl;
        dlclose(shamir_handle);
        return 1;
    }
    
    typedef int64_t (*shamirGeneratePrime_t)();
    typedef int64_t (*shamirGenerateKeyForPrime_t)(int64_t);
    typedef std::vector<uint8_t> (*shamirStartEncrypt_t)(const std::vector<uint8_t>&, int64_t, int64_t);
    typedef std::vector<uint8_t> (*shamirProcessBlocks_t)(const std::vector<uint8_t>&, int64_t, int64_t);
    typedef std::vector<uint8_t> (*shamirFinalDecrypt_t)(const std::vector<uint8_t>&, int64_t, int64_t);

    


    if (!shamirGeneratePrime || !shamirGenerateKeyForPrime || !shamirStartEncrypt || !shamirProcessBlocks || !shamirFinalDecrypt) {
        std::cerr << "Ошибка поиска функций Шамира: " << dlerror() << std::endl;
        dlclose(shamir_handle);
        return 1;
    }
    
    void* math_handle = dlopen("libcrypto_math.so", RTLD_LAZY);
    if (!math_handle) {
        std::cerr << "Ошибка загрузки библиотеки математики: " << dlerror() << std::endl;
        return 1;
    }

    typedef bool (*isPrime_t)(int64_t);
    isPrime_t isPrime = nullptr;
    typedef int64_t (*powerBinary_t)(int64_t, int64_t, int64_t);
    typedef int64_t (*extendedGCD_t)(int64_t, int64_t, int64_t&, int64_t&);
    typedef int64_t (*modInverse_t)(int64_t, int64_t);
    typedef bool (*isPrimeMillerRabin_t)(int64_t, int);
    typedef int64_t (*generateSafePrime_t)(int64_t, int64_t);

    extendedGCD_t extendedGCD = (extendedGCD_t)dlsym(math_handle, "extendedGCD");
    modInverse_t modInverse = (modInverse_t)dlsym(math_handle, "modInverse");
    isPrimeMillerRabin_t isPrimeMillerRabin = (isPrimeMillerRabin_t)dlsym(math_handle, "isPrimeMillerRabin");
    isPrime = (isPrime_t)dlsym(math_handle, "isPrime");
    if (!isPrime) {
        std::cerr << "Функция isPrime не найдена: " << dlerror() << std::endl;
        dlclose(math_handle);
        return 1;
    }
    if (!extendedGCD || !modInverse || !isPrimeMillerRabin) {
        std::cerr << "Ошибка поиска математических функций: " << dlerror() << std::endl;
        dlclose(math_handle);
        return 1;
    }
    MenuFunctions choiseEncDec;
    std::vector<uint8_t> processedData;
    std::cout<< "Динамические библиотеки подгружены!"<<std::endl;
    do {
        printMenu(1);
        printMenu(2);
        
        int32_t userChoise = readNumber<int32_t>("Выберите пункт меню: ");
        choiseEncDec = static_cast<MenuFunctions>(userChoise);
        printMenu(0);
        
        switch (choiseEncDec) {
            case MenuFunctions::Encrypt: {
                printMenu(1);

                std::cout << "ВЫБЕРИТЕ ТИП ВВОДA ДАННЫХ ДЛЯ ШИФРОВКИ\n";
                printMenu(3);
                
                MenuInputOutput choiseIn = static_cast<MenuInputOutput>(readNumber<int32_t>("Ваш выбор: "));
                bool dataLoaded = false;

                switch (choiseIn) {
                    case MenuInputOutput::File: {
                        std::cout << "Введите название файла (с расширением), который вы будете преобразовывать:\n";
                        std::string fileName;
                        std::cin >> fileName;
                        
                        std::ifstream file(fileName, std::ios::binary);
                        if (file) {
                            processedData = fromStreamToData(file);
                            std::cout << "[Успех] Данные считаны. Размер: " << processedData.size() << " байт.\n";
                            dataLoaded = true;
                        } else {
                            std::cerr << "[Ошибка] Не удалось открыть файл \"" << fileName << "\". Возврат в меню.\n";
                        }
                        file.close();
                        break;
                    }
                    case MenuInputOutput::Console: {
                        printMenu(0);
                        
                        std::cout << "\n--- ВЫБОР ФОРМАТА ВВОДА ---\n";
                        std::cout << "1. Обычный текст\n";
                        std::cout << "2. HEX-строка (для схемы Шамира)\n";
                        
                        int32_t formatChoice = readNumber<int32_t>("Выберите формат: ");
                        
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        
                        processedData.clear();
                        dataLoaded = false;

                        if (formatChoice == 1) {
                            std::cout << "Введите текст. Для завершения введите 'exit' с новой строки:\n";
                            
                            processedData = readConsoleToBytes();

                            if (!processedData.empty() && processedData.back() == '\n') {
                                processedData.pop_back();
                            }
                            
                            if (processedData.empty()) {
                                std::cout << "Ввод пуст. Возврат в меню.\n";
                                break;
                            }
                            
                            std::cout << "[Успех] Текст успешно считан. Размер: " << processedData.size() << " байт.\n";
                            dataLoaded = true;
                        } 
                        else if (formatChoice == 2) {
                            std::cout << "Вставьте HEX-строку. Для завершения введите 'exit' с новой строки:\n";

                            std::vector<uint8_t> hexChars = readConsoleToBytes();

                            if (!hexChars.empty() && hexChars.back() == '\n') {
                                hexChars.pop_back();
                            }

                            if (hexChars.empty()) {
                                std::cout << "Ввод пуст. Возврат в меню.\n";
                                break;
                            }

                            std::string rawHex(hexChars.begin(), hexChars.end());

                            try {
                                processedData = parseHexToBytes(rawHex);
                                std::cout << "[Успех] HEX успешно распарсен. Считано " << processedData.size() << " байт.\n";
                                dataLoaded = true;
                            } catch (const std::exception& e) {
                                std::cerr << "[Ошибка] Строка содержит некорректный HEX-код! Возврат в меню.\n";
                                processedData.clear();
                            }
                        } 
                        else {
                            std::cout << "[Ошибка] Неверный вариант формата. Возврат в меню.\n";
                        }

                        break;
                    }
                    case MenuInputOutput::Exit:
                        std::cout << "Возврат в главное меню.\n";
                        break;
                    default:
                        std::cerr << "[Ошибка] Неверный выбор типа ввода!\n";
                        break;
                }

                if (!dataLoaded || choiseIn == MenuInputOutput::Exit) {
                    break; 
                }

                printMenu(0);
                printMenu(1);
                printMenu(4);
                
                MenuEncOptions choiseEnc = static_cast<MenuEncOptions>(readNumber<int32_t>("Выберите алгоритм: "));
                
                if (choiseEnc == MenuEncOptions::Exit) {
                    std::cout << "Возврат в главное меню.\n";
                    break;
                }

                bool algoExecuted = true;

                switch (choiseEnc) {
                    case MenuEncOptions::Elgamal: {
                        std::cout << "Ввод параметров для ElGamal:\n";
                        int64_t p = readNumber<int64_t>("Введите простое число p: ", 3, std::numeric_limits<int64_t>::max());
                        
                        if (!isPrime(p)) { 
                            std::cerr << "[Ошибка] Число p должно быть простым! Операция отменена.\n";
                            algoExecuted = false;
                            break;
                        }

                        int64_t g = readNumber<int64_t>("Введите g (1 < g < p): ", 2, p - 1);
                        int64_t y = readNumber<int64_t>("Введите y (1 < y < p): ", 2, p - 1);

                        std::vector<CipherPair> encryptedData = encryptBytesElGamal(processedData, p, g, y);
                        processedData = cipherToBytes(encryptedData);
                        break;
                    
                    }
                    case MenuEncOptions::DES: {
                        std::string keystr;
                        while (true) {
                            std::cout << "Введите HEX-ключ DES (строго 16 символов): ";
                            std::cin >> keystr;
                            
                            if (keystr.length() == 16 && keystr.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos) {
                                break;
                            }
                            std::cerr << "[Ошибка] Неверный формат ключа! Ключ DES должен состоять ровно из 16 HEX-символов.\n";
                        }
                        uint64_t key = hexToInt(keystr);
                        processedData = desEncrypt(processedData, key);
                        break;   
                    }
                    case MenuEncOptions::Shamir: {
                        std::cout << "\n--- ПРОТОКОЛ ШАМИРА (ШИФРОВАНИЕ) ---\n";
                        std::cout << "0. Выход\n";
                        std::cout << "1. Шаг 1: Первичное шифрование (Отправитель)\n";
                        std::cout << "2. Шаг 2: Наложение второго шифра (Получатель)\n";
                        
                        int32_t shamirStep = readNumber<int32_t>("Выберите подэтап: ");
                        if (shamirStep == 0) {
                            std::cout << "Возврат в главное меню.\n";
                            algoExecuted = false;
                            break;
                        }

                        std::cout << "\n--- ПРОТОКОЛ ШАМИРА (ШИФРОВАНИЕ) ---\n";

                        int64_t p = readNumber<int64_t>("Введите общее простое число p (p > 255): ", 257, std::numeric_limits<int64_t>::max());
                        
                        if (!isPrime(p)) {
                            std::cerr << "[Ошибка] Число p должно быть простым!\n";
                            algoExecuted = false;
                            break;
                        }

                        int64_t key = readNumber<int64_t>("Введите ВАШ секретный ключ шифрования: ", 2, p - 2);
                        if (modInverse(key, p - 1) == -1) {
                            std::cerr << "[Ошибка] Ключ не является взаимно простым с (p-1)! Операция отменена.\n";
                            algoExecuted = false;
                            break;
                        }

                        ShamirChoise shamChoise = static_cast<ShamirChoise>(shamirStep);
                        if (processedData.empty()) {
                             std::cerr << "[Ошибка] Нет данных для обработки! Сначала считайте файл или введите текст.\n";
                            break; 
                        }       
                        switch (shamChoise) {
                            case ShamirChoise::Sender: {
                                processedData = shamirStartEncrypt(processedData, key, p);
                                std::cout << "[Успех] Шаг 1 выполнен. Файл готов к отправке Получателю.\n";
                                break;
                            }
                            case ShamirChoise::Recipient: {
                                try {
                                    processedData = shamirProcessBlocks(processedData, key, p);
                                    std::cout << "[Успех] Шаг 2 выполнен. Верните файл Отправителю.\n";
                                } catch (const std::exception& e) {
                                    std::cerr << "[Исключение] " << e.what() << "\n";
                                    algoExecuted = false;
                                }
                                break;
                            }
                            default:
                                std::cerr << "[Ошибка] Неверный подэтап протокола Шамира.\n";
                                algoExecuted = false;
                                break;
                        }
                        break;  
                    }
                    case MenuEncOptions::RSA: {
                        std::cout << "\n--- ШИФРОВАНИЕ RSA ---\n";
                        int64_t n = readNumber<int64_t>("Введите общий модуль n (n > 255): ", 256, std::numeric_limits<int64_t>::max());
                        int64_t e = readNumber<int64_t>("Введите открытую экспоненту e (e < n): ", 3, n - 1);
                        
                        if (modInverse(e, n) == -1)  { 
                            std::cout << "[Предупреждение] e и n имеют общие делители. Возможна ошибка шифрования.\n";
                        }

                        try {
                            processedData = rsaEncrypt(processedData, e, n);
                            std::cout << "[Успех] Шифрование RSA успешно завершено.\n";
                        } catch (const std::exception& ex) {
                            std::cerr << "[Ошибка RSA] " << ex.what() << "\n";
                            algoExecuted = false;
                        }
                        break;
                    }
                    case MenuEncOptions::RC4: {
                        std::cout << "\n--- ШИФРОВАНИЕ RC4 ---\n";
                        std::string keystr;
                        std::cout << "Введите HEX-ключ: ";
                        std::cin >> keystr;
                        if (keystr.length() % 2 != 0 || keystr.find_first_not_of("0123456789abcdefABCDEF") != std::string::npos) {
                            std::cerr << "[Ошибка] Некорректная HEX-строка! Длина должна быть четной, символы от 0-9 и A-F.\n";
                            algoExecuted = false;
                            break;
                        }
                        // ОЧИСТКА БУФЕРА 
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        
                        try {
                            std::vector<uint8_t> keyBytes = parseHexToBytes(keystr);
                            if (keyBytes.empty()) {
                                throw std::invalid_argument("Ключ не может быть пустым.");
                            }
                            processedData = rc4Transform(processedData, keyBytes);
                            std::cout << "[Успех] Шифрование RC4 выполнено.\n";
                        } catch (const std::exception& e) {
                            std::cerr << "[Ошибка RC4] Некорректный ключ: " << e.what() << "\n";
                            algoExecuted = false;
                        }
                        break;
                    }
                    case MenuEncOptions::RC5: {
                        std::cout << "\n--- ШИФРОВАНИЕ RC5 ---\n";
                        std::string keystr;
                        std::cout << "Введите HEX-ключ: ";
                        std::cin >> keystr;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        int64_t rounds = readNumber<int64_t>("Введите количество раундов (по умолчанию 12): ");
                        if (rounds < 0) rounds = 12;

                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                        try {
                            std::vector<uint8_t> keyBytes = parseHexToBytes(keystr);
                            if (keyBytes.empty()) {
                                throw std::invalid_argument("Ключ не может быть пустым.");
                            }
                            processedData = rc5Encrypt(processedData, keyBytes, rounds);
                            std::cout << "[Успех] Шифрование RC5 успешно завершено.\n";
                        } catch (const std::exception& e) {
                            std::cerr << "[Ошибка RC5] " << e.what() << "\n";
                            algoExecuted = false;
                        }
                        break;
                    }
                    default:
                        std::cerr << "[Ошибка] Неверный выбор алгоритма!\n";
                        algoExecuted = false;
                        break;
                }

                if (algoExecuted) {
                    printMenu(0);
                    printMenu(1);
                    std::cout << "ВЫБЕРИТЕ ТИП ВЫВОДА ЗАШИФРОВАННЫХ ДАННЫХ \n";
                    printMenu(3);
                    
                    MenuInputOutput choiseOut = static_cast<MenuInputOutput>(readNumber<int32_t>("Ваш выбор: "));
                    if (choiseOut == MenuInputOutput::Exit) {
                        std::cout << "Возврат в главное меню (данные не сохранены).\n";
                        break;
                    }

                    switch (choiseOut) {
                        case MenuInputOutput::File: {
                            std::cout << "Введите название файла с расширением(желательно .enc) для сохранения результата:\n";
                            std::string resultFileName;
                            std::cin >> resultFileName;
                            if (dataToBinaryFile(processedData, resultFileName)) {
                                std::cout << "[Успех] Данные сохранены в " << resultFileName << std::endl;
                            }
                            break;
                        }
                        case MenuInputOutput::Console: {
                            std::cout << "\n--- РЕЗУЛЬТАТ (HEX) ---\n";
                            std::cout << dataToHex(processedData) << std::endl;
                            std::cout << "-----------------------\n";
                            std::cout << "\n--- РЕЗУЛЬТАТ (ОБЫЧНЫЙ ТЕКСТ) ---\n";
                            for (uint8_t byte : processedData) {
                                std::cout << static_cast<char>(byte);
                            }
                            std::cout << "\n---------------------------------\n";
                            break;
                        }
                        default:
                            std::cerr << "[Предупреждение] Данные не сохранены: неверный выбор вывода.\n";
                            break;
                    }
                }
                break;
            }

            case MenuFunctions::Decrypt: {
                printMenu(1);
                std::cout << "ВЫБЕРИТЕ ТИП ВВОДА ДАННЫХ ДЛЯ РАСШИФРОВКИ\n";
                printMenu(3);
                
                MenuInputOutput choiseIn = static_cast<MenuInputOutput>(readNumber<int32_t>("Ваш выбор: "));
                bool dataLoaded = false;

                switch (choiseIn) {
                    case MenuInputOutput::File: {
                        std::cout << "Введите название файла (с расширением) для расшифровки:\n";
                        std::string fileName;
                        std::cin >> fileName;
                        std::ifstream file(fileName, std::ios::binary);
                        if (file) {
                            processedData = fromStreamToData(file);
                            if (processedData.empty()) {
                                std::cerr << "[Ошибка] Файл пуст. Возврат в меню.\n";
                            } else {
                                std::cout << "[Успех] Данные считаны. Размер: " << processedData.size() << " байт.\n";
                                dataLoaded = true;
                            }
                        } else {
                            std::cerr << "[Ошибка] Не удалось открыть файл. Возврат в меню.\n";
                        }
                        file.close();
                        break;
                    }
                    case MenuInputOutput::Console: {
                        printMenu(0);
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        
                        processedData.clear();
                        dataLoaded = false;

                        std::cout << "Вставьте HEX-строку для дешифрования.\n";
                        std::cout << "Для завершения ввода введите 'exit' с новой строки:\n";
                        
                        std::vector<uint8_t> hexChars = readConsoleToBytes();

                        if (!hexChars.empty() && hexChars.back() == '\n') {
                            hexChars.pop_back();
                        }

                        if (hexChars.empty()) {
                            std::cout << "Ввод пуст. Возврат в меню.\n";
                            break;
                        }

                        std::string rawHex(hexChars.begin(), hexChars.end());

                        try {
                            processedData = parseHexToBytes(rawHex);
                            if (processedData.empty()) {
                                throw std::invalid_argument("Пустой результат конвертации.");
                            }
                            std::cout << "[Успех] HEX успешно прочитан. Считано " << processedData.size() << " байт для дешифрования.\n";
                            dataLoaded = true;
                        } catch (const std::exception& e) {
                            std::cerr << "[Ошибка] Строка содержит некорректные HEX-символы! Данные не сохранены.\n";
                            processedData.clear();
                        }
                        break;
                    }
                    case MenuInputOutput::Exit:
                        std::cout << "Возврат в главное меню.\n";
                        break;
                    default:
                        std::cerr << "[Ошибка] Неверный выбор ввода.\n";
                        break;
                }

                if (!dataLoaded || choiseIn == MenuInputOutput::Exit || processedData.empty()) {
                    break;
                }

                printMenu(0);
                printMenu(1);
                printMenu(4);
                
                MenuEncOptions choiseEnc = static_cast<MenuEncOptions>(readNumber<int32_t>("Выберите алгоритм: "));

                if (choiseEnc == MenuEncOptions::Exit) {
                    std::cout << "Возврат в главное меню.\n";
                    break;
                }

                bool algoExecuted = true;

                switch (choiseEnc) {
                    case MenuEncOptions::Elgamal: {
                        int64_t p = readNumber<int64_t>("Введите p: ");
                        int64_t x = readNumber<int64_t>("Введите x: ");
                        
                        if (p <= 1 || x <= 0) {
                            std::cerr << "[Ошибка] Некорректные параметры Эль-Гамаля. p должно быть > 1, x > 0.\n";
                            algoExecuted = false;
                            break;
                        }
                        
                        try {
                            processedData = decryptBytesElGamal(bytesToCipher(processedData), p, x);
                            std::cout << "[Успех] Дешифрование ElGamal завершено.\n";
                        } catch (const std::exception& e) {
                            std::cerr << "[Ошибка] " << e.what() << "\n";
                            algoExecuted = false;
                        }
                        break;  
                    }
                    case MenuEncOptions::DES: {
                        std::string keystr;
                        std::cout << "Введите HEX-ключ: ";
                        std::cin >> keystr;
                        
                        try {
                            std::vector<uint8_t> keyBytes = parseHexToBytes(keystr);
                            if (keyBytes.empty()) {
                                throw std::invalid_argument("Ключ не может быть пустым.");
                            }
                            
                            uint64_t key = hexToInt(keystr);
                            processedData = desDecrypt(processedData, key);
                            std::cout << "[Успех] Дешифрование DES завершено.\n";
                        } catch (const std::exception& e) {
                            std::cerr << "[Ошибка DES] Некорректный HEX-ключ! Дешифрование отменено.\n";
                            algoExecuted = false;
                        }
                        break;   
                    }
                    case MenuEncOptions::Shamir: {
                        std::cout << "\n--- ПРОТОКОЛ ШАМИРА (ДЕШИФРОВАНИЕ) ---\n";
                        std::cout << "0. Выход\n";
                        std::cout << "1. Шаг 3: Снятие первого шифра (Отправитель)\n";
                        std::cout << "2. Шаг 4: Финальное дешифрование (Получатель)\n";
                        
                        int32_t shamirStep = readNumber<int32_t>("Выберите подэтап: ");
                        if (shamirStep == 0) {
                            std::cout << "Возврат в главное меню.\n";
                            algoExecuted = false;
                            break;
                        }

                        int64_t p = readNumber<int64_t>("Введите общее простое число p: ");
                        int64_t key = readNumber<int64_t>("Введите ВАШ секретный ключ шифрования: ");
                        
                        if (p <= 1 || key <= 0) {
                            std::cerr << "[Ошибка] Параметры p и ключ должны быть положительными, а p > 1.\n";
                            algoExecuted = false;
                            break;
                        }

                        int64_t d = modInverse(key, p - 1);
                        if (d == -1) {
                            std::cerr << "[Ошибка] Не удалось рассчитать обратный ключ дешифрования. Возможно, ключ и (p-1) не взаимно просты.\n";
                            algoExecuted = false;
                            break;
                        }

                        ShamirChoise shamChoise = static_cast<ShamirChoise>(shamirStep);
                        switch (shamChoise) {
                            case ShamirChoise::Sender: {
                                try {
                                    processedData = shamirProcessBlocks(processedData, d, p);
                                    std::cout << "[Успех] Шаг 3 выполнен. Отправьте файл Получателю.\n";
                                } catch (const std::exception& e) {
                                    std::cerr << "[Исключение] " << e.what() << "\n";
                                    algoExecuted = false;
                                }
                                break;
                            }
                            case ShamirChoise::Recipient: {
                                try {
                                    processedData = shamirFinalDecrypt(processedData, d, p);
                                    std::cout << "[Успех] Шаг 4 выполнен! Файл восстановлен.\n";
                                } catch (const std::exception& e) {
                                    std::cerr << "[Исключение] " << e.what() << "\n";
                                    algoExecuted = false;
                                }
                                break;
                            }
                            default:
                                std::cerr << "[Ошибка] Неверный выбор подэтапа.\n";
                                algoExecuted = false;
                                break;
                        }
                        break;  
                    }
                    case MenuEncOptions::RSA: {
                        std::cout << "\n--- ДЕШИФРОВАНИЕ RSA ---\n";
                        int64_t d = readNumber<int64_t>("Введите секретную экспоненту d: ");
                        int64_t n = readNumber<int64_t>("Введите общий модуль n: ");
                        
                        if (n <= 1 || d <= 0) {
                            std::cerr << "[Ошибка] Некорректные параметры RSA. n должно быть > 1, d > 0.\n";
                            algoExecuted = false;
                            break;
                        }
                        try {
                            processedData = rsaDecrypt(processedData, d, n);
                            std::cout << "[Успех] Дешифрование RSA успешно завершено.\n";
                        } catch (const std::exception& ex) {
                            std::cerr << "[Ошибка RSA] " << ex.what() << "\n";
                            algoExecuted = false;
                        }
                        break;
                    }
                    case MenuEncOptions::RC4: {
                        std::cout << "\n--- ДЕШИФРОВАНИЕ RC4 ---\n";
                        std::string keystr;
                        std::cout << "Введите HEX-ключ: ";
                        std::cin >> keystr;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        try {
                            std::vector<uint8_t> keyBytes = parseHexToBytes(keystr); 
                            if (keyBytes.empty()) {
                                throw std::invalid_argument("Ключ не может быть пустым.");
                            }
                            processedData = rc4Transform(processedData, keyBytes);
                            std::cout << "[Успех] Расшифрование RC4 выполнено.\n";
                        } catch (const std::exception& e) {
                            std::cerr << "[Ошибка RC4] Некорректный ключ: " << e.what() << "\n";
                            algoExecuted = false;
                        }
                        break;
                    }
                    case MenuEncOptions::RC5: {
                        std::cout << "\n--- ДЕШИФРОВАНИЕ RC5 ---\n";
                        std::string keystr;
                        std::cout << "Введите HEX-ключ: ";
                        std::cin >> keystr;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        uint64_t rounds = readNumber<uint64_t>("Введите количество раундов (по умолчанию 12): ");
                        if (rounds == 0) rounds = 12;
                        
                        if (rounds > 128) {
                            std::cerr << "[Предупреждение] Слишком много раундов. Сброшено до 12.\n";
                            rounds = 12;
                        }

                        try {
                            std::vector<uint8_t> keyBytes = parseHexToBytes(keystr);
                            if (keyBytes.empty()) {
                                throw std::invalid_argument("Ключ не может быть пустым.");
                            }
                            
                            std::vector<uint8_t> decrypted = rc5Decrypt(processedData, keyBytes, rounds);
                            if (decrypted.empty()) {
                                throw std::runtime_error("Ошибка дешифрования или неверный паддинг PKCS7.");
                            }
                            
                            processedData = decrypted;
                            std::cout << "[Успех] Расшифрование RC5 выполнено.\n";
                        } catch (const std::exception& e) {
                            std::cerr << "[Ошибка RC5] " << e.what() << "\n";
                            algoExecuted = false;
                        }
                        break;
                    }
                    default:
                        std::cerr << "[Ошибка] Метод дешифрования неверен.\n";
                        algoExecuted = false;
                        break;
                }

                // Защита: Если алгоритм упал или вернул флаг false, не даем перезаписать файлы пустышкой
                if (algoExecuted && !processedData.empty()) {
                    printMenu(0);
                    printMenu(1);
                    std::cout << "ВЫБЕРИТЕ ТИП ВЫВОДА РАСШИФРОВАННЫХ ДАННЫХ \n";
                    printMenu(3);
                    
                    MenuInputOutput choiseOut = static_cast<MenuInputOutput>(readNumber<int32_t>("Ваш выбор: "));
                    if (choiseOut == MenuInputOutput::Exit) {
                        std::cout << "Возврат в главное меню.\n";
                        break;
                    }

                    switch (choiseOut) {
                        case MenuInputOutput::File: {
                            std::cout << "Введите название файла(с нужным расширением) для сохранения результата:\n";
                            std::string resultFileName;
                            std::cin >> resultFileName;
                            if (dataToBinaryFile(processedData, resultFileName)) {
                                std::cout << "[Успех] Данные успешно сохранены в " << resultFileName << std::endl;
                            }
                            break;
                        }
                        case MenuInputOutput::Console: {
                            std::cout << "\n--- РЕЗУЛЬТАТ (HEX) ---\n";
                            std::cout << dataToHex(processedData) << std::endl;
                            std::cout << "-----------------------\n";
                            std::cout << "\n--- РЕЗУЛЬТАТ (ОБЫЧНЫЙ ТЕКСТ) ---\n";
                            
                            for (uint8_t byte : processedData) {
                                std::cout << static_cast<char>(byte);
                            }
                            std::cout << "\n---------------------------------\n";
                            break;
                        }
                        default:
                            std::cerr << "[Ошибка] Неверный выбор типа вывода.\n";
                            break;
                    }
                } else {
                    std::cerr << "[Защита системы] Данные повреждены или не были расшифрованы. Вывод заблокирован.\n";
                }
                break;
            }

            case MenuFunctions::Keys: {
                printMenu(4);
                MenuEncOptions choiseEnc = static_cast<MenuEncOptions>(readNumber<int32_t>("Выберите алгоритм для генерации ключей: "));
                printMenu(0);

                if (choiseEnc == MenuEncOptions::Exit) {
                    std::cout << "Возврат в главное меню.\n";
                    break;
                }

                switch (choiseEnc) {
                    case MenuEncOptions::Elgamal: {
                        int64_t p, g, x, y;
                        generateElGamalKeys(p, g, x, y);
                        std::cout << "Сгенерированные ключи:\np = " << p << " g = " << g << " x = " << x << " y = " << y << std::endl;
                        break;  
                    }
                    case MenuEncOptions::DES: {
                        uint64_t newkey = generateDesKey();
                        std::cout << "\nВаш новый ключ: " << intToHex(newkey) << "\n";
                        break;  
                    }
                    case MenuEncOptions::Shamir: {
                        std::cout << "\n--- ГЕНЕРАЦИЯ КЛЮЧЕЙ ДЛЯ ПРОТОКОЛА ШАМИРА ---\n";
                        std::cout << "0. Выход\n";
                        std::cout << "1. Автоматическая генерация (создать новое число p и ключ)\n";
                        std::cout << "2. Ручной ввод (сгенерировать ключ под уже готовое число p)\n";
                        
                        int32_t keyGenOption = readNumber<int32_t>("Выберите вариант: ");
                        if (keyGenOption == 0) {
                            std::cout << "Возврат в главное меню.\n";
                            break;
                        }

                        ShamirKesyGenerate keyChoise = static_cast<ShamirKesyGenerate>(keyGenOption);
                        printMenu(0);
                        switch (keyChoise) {
                            case ShamirKesyGenerate::AbsoluteGen: {
                                try {
                                    int64_t p = shamirGeneratePrime();
                                    int64_t secret_key = shamirGenerateKeyForPrime(p);
                                    std::cout << "\n=== ВАШ КЛЮЧ УСПЕШНО СГЕНЕРИРОВАН ===\n";
                                    std::cout << "Используемый модуль p = " << p << "\n";
                                    std::cout << "Ваш секретный ключ: " << secret_key << "\n";
                                } catch (const std::exception& e) {
                                    std::cerr << "[Исключение] " << e.what() << "\n";
                                }
                                break;
                            }
                            case ShamirKesyGenerate::GenerateByP: {
                                try {
                                    int64_t p = readNumber<int64_t>("Введите общее простое число p: ");
                                    int64_t secret_key = shamirGenerateKeyForPrime(p);
                                    std::cout << "\n=== ВАШ КЛЮЧ УСПЕШНО СГЕНЕРИРОВАН ===\n";
                                    std::cout << "Используемый модуль p = " << p << "\n";
                                    std::cout << "Ваш секретный ключ: " << secret_key << "\n";
                                } catch (const std::exception& e) {
                                    std::cerr << "[Исключение] " << e.what() << "\n";
                                }
                                break;
                            }
                            default:
                                break;
                        }
                        break;  
                    }
                    case MenuEncOptions::RSA: {
                        std::cout << "\n--- ГЕНЕРАЦИЯ КЛЮЧЕЙ RSA ---\n";
                        std::cout << "0. Выход\n";
                        std::cout << "1. Автоматическая генерация (создать случайные простые числа p, q)\n";
                        std::cout << "2. Ручной ввод (вычислить ключи на основе ваших p и q)\n";
                        
                        int32_t genChoiceInput = readNumber<int32_t>("Выберите вариант: ");
                        if (genChoiceInput == 0) {
                            std::cout << "Возврат в меню.\n";
                            break;
                        }

                        RsaKeyGenOptions genChoice = static_cast<RsaKeyGenOptions>(genChoiceInput);
                        int64_t input_p = 0, input_q = 0;

                        switch (genChoice) {
                            case RsaKeyGenOptions::Auto:
                                std::cout << "Генерация параметров системы, подождите...\n";
                                break;
                            case RsaKeyGenOptions::Manual:
                                input_p = readNumber<int64_t>("Введите простое число p: ");
                                input_q = readNumber<int64_t>("Введите простое число q: ");
                                break;
                            default:
                                std::cerr << "[Ошибка] Неверный выбор режима генерации!\n";
                                break;
                        }

                        if (genChoice == RsaKeyGenOptions::Auto || genChoice == RsaKeyGenOptions::Manual) {
                            try {
                                RSAKeys keys = rsaGenerateKeys(genChoice, input_p, input_q);

                                std::cout << "\n=== КЛЮЧИ УСПЕШНО СГЕНЕРИРОВАНЫ ===\n";
                                if (genChoice == RsaKeyGenOptions::Auto) {
                                    std::cout << "[Авто] Сгенерированы числа: p = " << keys.p << ", q = " << keys.q << "\n";
                                }
                                std::cout << "Общий модуль (n) = " << keys.n << "\n";
                                std::cout << "ОТКРЫТЫЙ КЛЮЧ: e = " << keys.e << ", n = " << keys.n << "\n";
                                std::cout << "ЗАКРЫТЫЙ КЛЮЧ: d = " << keys.d << ", n = " << keys.n << "\n";
                            } catch (const std::exception& ex) {
                                std::cerr << "[Ошибка generation RSA] " << ex.what() << "\n";
                            }
                        }
                        break;
                    }
                    case MenuEncOptions::RC4: {
                        std::cout << "\n--- ГЕНЕРАЦИЯ КЛЮЧА RC4 ---\n";
                        size_t len = readNumber<size_t>("Введите длину ключа в байтах (рекомендуется 16-256): ");
                        if (len == 0 || len > 256) len = 16;

                        std::vector<uint8_t> rc4Key = generateRC4Key(len);
                        std::cout << "\n=== ВАШ НОВЫЙ КЛЮЧ RC4 (HEX) ===\n";
                        std::cout << dataToHex(rc4Key) << "\n";
                        std::cout << "============================\n";
                        break;
                    }
                    case MenuEncOptions::RC5: {
                        std::cout << "\n--- ГЕНЕРАЦИЯ КЛЮЧА RC5 ---\n";
                        size_t len = readNumber<size_t>("Введите длину ключа в байтах (рекомендуется 16): ");
                        if (len <= 0) len = 16;

                        std::vector<uint8_t> rc5Key = generateRC5Key(len);
                        std::cout << "\n=== ВАШ НОВЫЙ КЛЮЧ RC5 (HEX) ===\n";
                        std::cout << dataToHex(rc5Key) << "\n";
                        std::cout << "============================\n";
                        break;  
                    }
                    default:
                        std::cerr << "[Ошибка] Неверный выбор!\n";
                        break;
                }
                break;
            }

            case MenuFunctions::Exit:
                std::cout << "Работа программы завершена.\n";
                return 0;

            default:
                std::cerr << "[Предупреждение] Такого пункта меню не существует. Попробуйте снова.\n";
                break;
        }

    } while (choiseEncDec != MenuFunctions::Exit);

    return 0;
}
