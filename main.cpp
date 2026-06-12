#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <clocale>
#include <limits>

#include "ciphers/elgamal.h"
#include "ciphers/shamir.h"
#include "ciphers/des.h"
#include "ciphers/rsa.h" // Подключили RSA
#include "scripts/crypto_math.h"
#include "scripts/input_output.h"

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

int main() {
    std::setlocale(LC_ALL, "Russian");
    
    if (!loginFunc()) {
        std::cout << "Введён неверный пароль\n";
        return 0;
    }

    MenuFunctions choiseEncDec;
    std::vector<uint8_t> processedData;

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
                        std::cout << "Введите данные в консоль. Если это Шаг 2, вставляйте HEX-строку без пробелов.\n";
                        std::cout << "Для окончания ввода введите 'exit' на новой строке:\n";
    
                        std::string rawInput;
                        std::string line;
                        while (std::cin >> line && line != "exit") {
                            rawInput += line;
                        }

                        std::cout << "Как интерпретировать ввод? (1 - Обычный текст, 2 - HEX-строка): ";
                        int32_t inputType = readNumber<int32_t>("");

                        if (inputType == 2) {
                            processedData.clear();
                            for (size_t i = 0; i < rawInput.length(); i += 2) {
                                if (i + 1 < rawInput.length()) {
                                    std::string byteString = rawInput.substr(i, 2);
                                    uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
                                    processedData.push_back(byte);
                                }
                            }
                            std::cout << "[Успех] HEX-строка успешно распарсена в " << processedData.size() << " байт.\n";
                        } else {
                            processedData = std::vector<uint8_t>(rawInput.begin(), rawInput.end());
                        }
    
                        dataLoaded = true;
                        break;
                    }
                    case MenuInputOutput::Exit:
                        std::cout << "Возврат в главное меню.\n";
                        break;
                    default:
                        std::cerr << "[Ошибка] Неверный выбор типа ввода!\n";
                        break;
                }

                if (!dataLoaded) {
                    break; 
                }

                printMenu(0);
                printMenu(1);
                printMenu(4);
                
                MenuEncOptions choiseEnc = static_cast<MenuEncOptions>(readNumber<int32_t>("Выберите алгоритм: "));

                switch (choiseEnc) {
                    case MenuEncOptions::Elgamal: {
                        std::cout << "Ввод параметров для ElGamal:\n";
                        int64_t p = readNumber<int64_t>("Введите p: ");
                        int64_t g = readNumber<int64_t>("Введите g: ");
                        int64_t y = readNumber<int64_t>("Введите y: ");

                        std::vector<CipherPair> encryptedData = encryptBytesElGamal(processedData, p, g, y);
                        processedData = cipherToBytes(encryptedData);
                        break;
                    }
                    case MenuEncOptions::DES: {
                        std::string keystr;
                        std::cout << "Введите HEX-ключ (16 символов): ";
                        std::cin >> keystr;
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
                        int64_t p = readNumber<int64_t>("Введите общее простое число p: ");
                        int64_t key = readNumber<int64_t>("Введите ВАШ секретный ключ шифрования: ");

                        if (modInverse(key, p - 1) == -1) {
                            std::cerr << "[Ошибка] Ключ не является взаимно простым с (p-1)! Операция отменена.\n";
                            break;
                        }

                        ShamirChoise shamChoise = static_cast<ShamirChoise>(shamirStep);
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
                                }
                                break;
                            }
                            default:
                                std::cerr << "[Ошибка] Неверный подэтап протокола Шамира.\n";
                                break;
                        }
                        break;  
                    }
                    case MenuEncOptions::RSA: {
                        std::cout << "\n--- ШИФРОВАНИЕ RSA ---\n";
                        int64_t e = readNumber<int64_t>("Введите открытую экспоненту e: ");
                        int64_t n = readNumber<int64_t>("Введите общий модуль n: ");
                        try {
                            processedData = rsaEncrypt(processedData, e, n);
                            std::cout << "[Успех] Шифрование RSA успешно завершено.\n";
                        } catch (const std::exception& ex) {
                            std::cerr << "[Ошибка RSA] " << ex.what() << "\n";
                        }
                        break;
                    }
                    case MenuEncOptions::RC4:
                    case MenuEncOptions::RC5:
                        std::cout << "Данный алгоритм еще не реализован.\n";
                        break;
                    case MenuEncOptions::Exit:
                        std::cout << "Возврат в главное меню.\n";
                        break;
                    default:
                        std::cerr << "[Ошибка] Неверный выбор алгоритма!\n";
                        break;
                }

                if (choiseEnc != MenuEncOptions::Exit && choiseEnc <= MenuEncOptions::RC5) {
                    printMenu(0);
                    printMenu(1);
                    std::cout << "ВЫБЕРИТЕ ТИП ВЫВОДА ЗАШИФРОВАННЫХ ДАННЫХ \n";
                    printMenu(3);
                    
                    MenuInputOutput choiseOut = static_cast<MenuInputOutput>(readNumber<int32_t>("Ваш выбор: "));
                    switch (choiseOut) {
                        case MenuInputOutput::File: {
                            std::cout << "Введите название файла для сохранения результата:\n";
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
                            std::cout << "[Успех] Данные считаны. Размер: " << processedData.size() << " байт.\n";
                            dataLoaded = true;
                        } else {
                            std::cerr << "[Ошибка] Не удалось открыть файл. Возврат в меню.\n";
                        }
                        file.close();
                        break;
                    }
                    case MenuInputOutput::Console: {
                        printMenu(0);
                        std::cout << "Вставьте HEX-строку для дешифрования (без пробелов) и введите 'exit' для завершения:\n";
    
                        std::string rawHex;
                        std::string line;
                        while (std::cin >> line && line != "exit") {
                            rawHex += line;
                        }

                        processedData.clear();
                        for (size_t i = 0; i < rawHex.length(); i += 2) {
                            if (i + 1 < rawHex.length()) {
                                std::string byteString = rawHex.substr(i, 2);
                                uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
                                processedData.push_back(byte);
                            }
                        }

                        std::cout << "[Успех] Считано " << processedData.size() << " байт из HEX-ввода.\n";
                        dataLoaded = true;
                        break;
                    }
                    default:
                        std::cerr << "[Ошибка] Неверный выбор ввода.\n";
                        break;
                }

                if (!dataLoaded) break;

                printMenu(0);
                printMenu(1);
                printMenu(4);
                
                MenuEncOptions choiseEnc = static_cast<MenuEncOptions>(readNumber<int32_t>("Выберите алгоритм: "));

                switch (choiseEnc) {
                    case MenuEncOptions::Elgamal: {
                        int64_t p = readNumber<int64_t>("Введите p: ");
                        int64_t x = readNumber<int64_t>("Введите x: ");
                        
                        try {
                            processedData = decryptBytesElGamal(bytesToCipher(processedData), p, x);
                            std::cout << "[Успех] Дешифрование ElGamal завершено.\n";
                        } catch (const std::exception& e) {
                            std::cerr << "[Ошибка] " << e.what() << "\n";
                        }
                        break;  
                    }
                    case MenuEncOptions::DES: {
                        std::string keystr;
                        std::cout << "Введите HEX-ключ: ";
                        std::cin >> keystr;
                        uint64_t key = hexToInt(keystr);
                        processedData = desDecrypt(processedData, key);
                        break;  
                    }
                    case MenuEncOptions::Shamir: {
                        std::cout << "\n--- ПРОТОКОЛ ШАМИРА (ДЕШИФРОВАНИЕ) ---\n";
                        std::cout << "0. Выход\n";
                        std::cout << "1. Шаг 3: Снятие первого шифра (Отправитель)\n";
                        std::cout << "2. Шаг 4: Финальное дешифрование (Получатель)\n";
                        
                        int32_t shamirStep = readNumber<int32_t>("Выберите подэтап: ");
                        int64_t p = readNumber<int64_t>("Введите общее простое число p: ");
                        int64_t key = readNumber<int64_t>("Введите ВАШ секретный ключ шифрования: ");
                        
                        int64_t d = modInverse(key, p - 1);
                        if (d == -1) {
                            std::cerr << "[Ошибка] Не удалось рассчитать обратный ключ дешифрования.\n";
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
                                }
                                break;
                            }
                            case ShamirChoise::Recipient: {
                                try {
                                    processedData = shamirFinalDecrypt(processedData, d, p);
                                    std::cout << "[Успех] Шаг 4 выполнен! Файл восстановлен.\n";
                                } catch (const std::exception& e) {
                                    std::cerr << "[Исключение] " << e.what() << "\n";
                                }
                                break;
                            }
                            default:
                                std::cerr << "[Ошибка] Неверный выбор подэтапа.\n";
                                break;
                        }
                        break;  
                    }
                    case MenuEncOptions::RSA: {
                        std::cout << "\n--- ДЕШИФРОВАНИЕ RSA ---\n";
                        int64_t d = readNumber<int64_t>("Введите секретную экспоненту d: ");
                        int64_t n = readNumber<int64_t>("Введите общий модуль n: ");
                        try {
                            processedData = rsaDecrypt(processedData, d, n);
                            std::cout << "[Успех] Дешифрование RSA успешно завершено.\n";
                        } catch (const std::exception& ex) {
                            std::cerr << "[Ошибка RSA] " << ex.what() << "\n";
                        }
                        break;
                    }
                    case MenuEncOptions::RC4:
                    case MenuEncOptions::RC5:
                        std::cout << "Данный алгоритм еще не реализован.\n";
                        break;
                    default:
                        std::cerr << "[Ошибка] Метод дешифрования неверен.\n";
                        break;
                }

                if (choiseEnc != MenuEncOptions::Exit && choiseEnc <= MenuEncOptions::RC5) {
                    printMenu(0);
                    printMenu(1);
                    std::cout << "ВЫБЕРИТЕ ТИП ВЫВОДА РАСШИФРОВАННЫХ ДАННЫХ \n";
                    printMenu(3);
                    
                    MenuInputOutput choiseOut = static_cast<MenuInputOutput>(readNumber<int32_t>("Ваш выбор: "));
                    switch (choiseOut) {
                        case MenuInputOutput::File: {
                            std::cout << "Введите название файла для сохранения результата:\n";
                            std::string resultFileName;
                            std::cin >> resultFileName;
                            if (dataToBinaryFile(processedData, resultFileName)) {
                                std::cout << "[Успех] Данные успешно сохранены in " << resultFileName << std::endl;
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
                            break;
                    }
                }
                break;
            }

            case MenuFunctions::Keys: {
                printMenu(4);
                MenuEncOptions choiseEnc = static_cast<MenuEncOptions>(readNumber<int32_t>("Выберите алгоритм для генерации ключей: "));
                printMenu(0);
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
                        // Кастуем введенное число в наш enum class
                        RsaKeyGenOptions genChoice = static_cast<RsaKeyGenOptions>(genChoiceInput);
                        
                        int64_t input_p = 0, input_q = 0;

                        // Если пользователь выбрал выход, сразу тормозим процесс
                        if (genChoice == RsaKeyGenOptions::Exit) {
                            std::cout << "Возврат в меню.\n";
                            break;
                        }

                        // Свитч кейс строго по перечислению, как требует ТЗ
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

                        // Если выбор был корректным, выполняем генерацию
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
                                std::cerr << "[Ошибка генерации RSA] " << ex.what() << "\n";
                            }
                        }
                        break;
                    }
                    case MenuEncOptions::RC4:
                    case MenuEncOptions::RC5:
                        std::cout << "Данный алгоритм еще не реализован.\n";
                        break;  
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
