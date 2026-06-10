#include <iostream>
#include "ciphers/elgamal.h"
#include "scripts/crypto_math.h"
#include "ciphers/des.h"
#include "scripts/input_output.h"
#include <cstdlib> 

enum class MenuFunctions : int32_t {
    Exit = 0,
    Encrypt =1,
    Decrypt = 2,
    Keys = 3
};
enum class MenuEncOptions : int32_t {
    Exit = 0,
    Elgamal =1,
    DES = 2,
    Shamir,
    RSA,
    RC4,
    RC5,
};
enum class MenuInputOutput : int32_t {
    Exit = 0,
    File =1,
    Console = 2
};


int main(){
    std::setlocale(LC_ALL, "Russian");
    int userChoise = 0;
    MenuFunctions choiseEncDec = MenuFunctions(0);
    MenuInputOutput choiseIn = MenuInputOutput(0);
    MenuInputOutput choiseOut = MenuInputOutput(0);
    std::vector<uint8_t> processedData;
    do  {
        printMenu();
        std::cout <<"ВЫБЕРИТЕ ШИФРОВКА/ДЕШИФРОВКА\n";
        std::cout <<"0.Выход\n";
        std::cout <<"1.Шифрование\n";
        std::cout <<"2.Дешифрование\n";
        std::cout <<"3.Генерация ключей\n";
        std::cin >> userChoise;
        choiseEncDec = MenuFunctions(userChoise);
        clearScreen();
        switch (choiseEncDec){
            case MenuFunctions::Encrypt: {
                printMenu();
                std::cout <<"ВЫБЕРИТЕ ТИП ВВОДА ДАННЫХ ДЛЯ ШИФРОВКИ\n";
                std::cout <<"0.Выход\n";
                std::cout <<"1.Файл\n";
                std::cout <<"2.Из консоли\n";
                std::cin >> userChoise;
                choiseIn = MenuInputOutput(userChoise);
                switch (choiseIn){
                    case MenuInputOutput::File: {
                        std::cout <<"Введите название файла(с расширением), который вы будете преобразовывать\n";
                        std::string fileName;
                        std::cin >> fileName;
                        std::ifstream file(fileName, std::ios::binary);
                        if (file) {
                        // Прогоняем файл 
                            processedData = fromStreamToData(file);
                            std::cout << "[Успех] Данные считаны, обработаны и сохранены в памяти. Размер: " << processedData.size() << " байт.\n";
                        } else {
                            std::cerr << "Не удалось открыть файл.\n";
                            return 1;
                        }
                        file.close();
                        break;
                    }
                    case MenuInputOutput::Console: {
                        std::cout << "Введите exit, чтобы прекратить ввод из консоли" << std::endl;
                        processedData = readConsoleToBytes();
                        std::cout << "ввод с консоли прекращён!" << std::endl;
                        break;
                    }
                    case MenuInputOutput::Exit:
                        std::cout << "Работа программы завершена.\n";
                        return 0;
                        break;
                    default:
                        std::cerr << "Ошибка: Неверный выбор!\n";
                        return 0;
                        break;
                    }

                clearScreen();
                printMenu();

                std::cout <<"ВЫБЕРИТЕ МЕТОД ШИФРОВАНИЯ \n";
                std::cout <<"0.Выход\n";
                std::cout <<"1.Эль-Гамаль\n";
                std::cout <<"2.DES\n";
                std::cout <<"3.Шамир\n";
                std::cout <<"4.RSA\n";
                std::cout <<"5.RC4\n";
                std::cout <<"6.RC5\n";
                std::cin >> userChoise;
                MenuEncOptions choiseEnc = MenuEncOptions(userChoise);

                switch (choiseEnc)
                {
                case MenuEncOptions::Elgamal:{
                    int64_t p, g, x, y;
                    std::vector<CipherPair> encryptedData;
                    std::cout << "Введите p, g, y через пробел ";
    
                    if (std::cin >> p >> g >> y) {
                        std::cout << "Вы ввели: " << p << ", " << g << ", " << y << "\n";
                    } else {
                    std::cerr << "Ошибка ввода! Вы ввели не целые числа.\n";
                        return 0;
                    }

                    encryptedData = encryptBytesElGamal(processedData, p, g, y);
                    processedData = cipherToBytes(encryptedData);
                    break;
                }
                case MenuEncOptions::DES:{
                    std::string keystr;
                    std::cout << "Введите HEX-ключ (16 символов): ";
                    std::cin >> keystr;
                    uint64_t key = hexToInt(keystr);
                    processedData = desEncrypt(processedData, key);
                    break;  
                }
                case MenuEncOptions::Shamir:{
                    break;  
                }
                case MenuEncOptions::RSA:{
                    break;  
                }
                case MenuEncOptions::RC4:{
                    break;  
                }
                case MenuEncOptions::RC5:{
                    break;  
                }
                case MenuEncOptions::Exit:
                        std::cout << "Работа программы завершена.\n";
                        return 0;
                        break;
                default:
                        std::cerr << "Ошибка: Неверный выбор!\n";
                        return 0;
                        break;
                    }


                clearScreen();
                printMenu();


                std::cout <<"ВЫБЕРИТЕ ТИП ВЫВОДА ЗАШИФРОВАННЫХ ДАННЫХ \n";
                std::cout <<"0.Выход\n";
                std::cout <<"1.В файл\n";
                std::cout <<"2.В консоль\n";
                std::cin >> userChoise;
                MenuInputOutput choiseOut = MenuInputOutput(userChoise);

                switch (choiseOut){
                    case MenuInputOutput::File: {
                    std::cout <<"Введите название файла(с расширением), который вы будете преобразовывать\n";
                    std::string resultFileName;
                    std::cin >> resultFileName;
                    if (dataToBinaryFile(processedData, resultFileName) ){
                        std::cout << "Файл: Данные успешно упакованы в " << resultFileName << std::endl;
                    }
                    break;
                    }
                    case MenuInputOutput::Console: {
                    std::cout << dataToHex(processedData) << std::endl;;
                    std::cout << "вывод в консоль прекращён!" << std::endl;
                    break;
                    }
                    case MenuInputOutput::Exit:
                        std::cout << "Работа программы завершена.\n";
                        return 0;
                        break;
                    default:
                        std::cerr << "Ошибка: Неверный выбор!\n";
                        break;
                        return 0;
                    }
                    break;
                }
            case MenuFunctions::Decrypt: {
                printMenu();
                std::cout <<"ВЫБЕРИТЕ ТИП ВВОДА ДАННЫХ ДЛЯ РАСШИФРОВКИ\n";
                std::cout <<"0.Выход\n";
                std::cout <<"1.Файл\n";
                std::cout <<"2.Из консоли\n";
                std::cin >> userChoise;
                choiseIn = MenuInputOutput(userChoise);
                switch (choiseIn){
                    case MenuInputOutput::File: {
                        std::cout <<"Введите название файла(с расширением), который вы будете прасшифровывать\n";
                        std::string fileName;
                        std::cin >> fileName;
                        std::ifstream file(fileName, std::ios::binary);
                        if (file) {
                        // Прогоняем файл 
                            processedData = fromStreamToData(file);
                            std::cout << "[Успех] Данные считаны, обработаны и сохранены в памяти. Размер: " << processedData.size() << " байт.\n";
                        } else {
                            std::cerr << "Не удалось открыть файл.\n";
                            return 1;
                        }
                        file.close();
                        break;
                    }
                    case MenuInputOutput::Console: {
                        std::cout << "Введите exit, чтобы прекратить ввод из консоли" << std::endl;
                        processedData = readConsoleToBytes();
                        std::cout << "ввод с консоли прекращён!" << std::endl;
                        break;
                    }
                    case MenuInputOutput::Exit:
                        std::cout << "Работа программы завершена.\n";
                        return 0;
                        break;
                    default:
                        std::cerr << "Ошибка: Неверный выбор!\n";
                        return 0;
                        break;
                    }
                clearScreen();
                printMenu();
                std::cout <<"ВЫБЕРИТЕ МЕТОД ШИФРОВАНИЯ \n";
                std::cout <<"0.Выход\n";
                std::cout <<"1.Эль-Гамаль\n";
                std::cout <<"2.DES\n";
                std::cout <<"3.Шамир\n";
                std::cout <<"4.RSA\n";
                std::cout <<"5.RC4\n";
                std::cout <<"6.RC5\n";
                std::cin >> userChoise;
                MenuEncOptions choiseEnc = MenuEncOptions(userChoise);

                switch (choiseEnc)
                {
                case MenuEncOptions::Elgamal:{
                    std::cout << "Введите поочердно значения ваши значения p x: ";
                    int64_t p, g, x, y;
                    
    
                    // std::cin автоматически пропускает пробелы и переносы строк
                    if (std::cin >> p >> x) {
                        std::cout << "Вы ввели: " << p << ", " << x << "\n";
                    } else {
                        std::cout << "Ошибка ввода! Вы ввели не целые числа.\n";
                    }
                    
                    std::vector<CipherPair> dencryptedData;
                    processedData = decryptBytesElGamal(bytesToCipher(processedData), p, x);
                    break;  
                }
                case MenuEncOptions::DES:{
                    std::string keystr;
                    std::cout << "Введите HEX-ключ: ";
                    std::cin >> keystr;
                    uint64_t key = hexToInt(keystr);
                    processedData = desDecrypt(processedData, key);
                    break;  
                }
                case MenuEncOptions::Shamir:{
                    break;  
                }
                case MenuEncOptions::RSA:{
                    break;  
                }
                case MenuEncOptions::RC4:{
                    break;  
                }
                case MenuEncOptions::RC5:{
                    break;  
                }
                case MenuEncOptions::Exit:
                        std::cout << "Работа программы завершена.\n";
                        return 0;
                        break;
                default:
                        std::cerr << "Ошибка: Неверный выбор!\n";
                        return 0;
                        break;
                }
                clearScreen();
                printMenu();


                std::cout <<"ВЫБЕРИТЕ ТИП ВЫВОДА РАСШИФРОВАННЫХ ДАННЫХ \n";
                std::cout <<"0.Выход\n";
                std::cout <<"1.В файл\n";
                std::cout <<"2.В консоль\n";
                std::cin >> userChoise;
                MenuInputOutput choiseOut = MenuInputOutput(userChoise);

                switch (choiseOut){
                    case MenuInputOutput::File: {
                    std::cout <<"Введите название файла(с расширением), который вы будете преобразовывать\n";
                    std::string resultFileName;
                    std::cin >> resultFileName;
                    if (dataToBinaryFile(processedData, resultFileName) ){
                        std::cout << "Файл: Данные успешно упакованы в " << resultFileName << std::endl;
                    }
                    break;
                    }
                    case MenuInputOutput::Console: {
                    std::cout << dataToHex(processedData) << std::endl;;
                    std::cout << "вывод в консоль прекращён!" << std::endl;
                    break;
                    }
                    case MenuInputOutput::Exit:
                        std::cout << "Работа программы завершена.\n";
                        return 0;
                        break;
                    default:
                        std::cerr << "Ошибка: Неверный выбор!\n";
                        break;
                        return 0;
                }
                break;}
            case MenuFunctions::Keys:{
                std::cout <<"ВЫБЕРИТЕ ШИФР ДЛЯ КОТОРОГО НУЖНЫ КЛЮЧИ \n";
                std::cout <<"0.Выход\n";
                std::cout <<"1.Эль-Гамаль\n";
                std::cout <<"2.DES\n";
                std::cout <<"3.Шамир\n";
                std::cout <<"4.RSA\n";
                std::cout <<"5.RC4\n";
                std::cout <<"6.RC5\n";
                std::cin >> userChoise;
                MenuEncOptions choiseEnc = MenuEncOptions(userChoise);

                switch (choiseEnc)
                {
                case MenuEncOptions::Elgamal:{
                    int64_t p, g, x, y;
                    generateElGamalKeys(p, g, x, y);
                    std::cout <<"p = "<< p <<" g = "<< g <<" x = "<< x <<" y = "<< y<<std::endl;
                    std::cout <<"Для копирования "<< p <<" "<< g <<" "<< y<<std::endl;
                    break;  
                }
                case MenuEncOptions::DES:{
                    uint64_t newkey = generateDesKey();
                    std::cout << "\nВаш новый ключ (скопируйте его): " << intToHex(newkey) << "\n";

                    break;  
                }
                case MenuEncOptions::Shamir:{
                    break;  
                }
                case MenuEncOptions::RSA:{
                    break;  
                }
                case MenuEncOptions::RC4:{
                    break;  
                }
                case MenuEncOptions::RC5:{
                    break;  
                }
                case MenuEncOptions::Exit:
                        std::cout << "Работа программы завершена.\n";
                        return 0;
                        break;
                default:
                        std::cerr << "Ошибка: Неверный выбор!\n";
                        return 0;
                        break;
                    }
                break;
                
                    
            }
            case MenuFunctions::Exit:
                    std::cout << "Работа программы завершена.\n";
                    return 0;
                    break;
                default:
                    std::cerr << "Ошибка: Неверный выбор!\n";
                    return 0;
                    break;
                
        }

    }while (choiseEncDec != MenuFunctions::Exit);


    /*
    std::string fileName;
    std::cout << "введите название файла(с расширением), которое вы будете преобразовывать" << std::endl;
    std::cin >> fileName;
    std::vector<uint8_t> processedData;
    //выбираете файл для преобразования в вектор байтов
    std::ifstream file(fileName, std::ios::binary);
    if (file) {
        // Прогоняем файл 
        processedData = fromStreamToData(file);
        std::cout << "[Успех] Данные считаны, обработаны и сохранены в памяти. Размер: " 
                  << processedData.size() << " байт.\n";
    } else {
        std::cerr << "Не удалось открыть файл.\n";
        return 1;
    }
    file.close();
    // !!!!! ТЕПЕРЬ В processedData ЛЕЖАТ БАЙТЫ ИЗ КОТОРЫХ СОСТОЯЛ ФАЙЛ!!!!!
    std::string resultFileName;
    std::cout << "введите название файла(с расширением), в который вы преобразуете информацию" << std::endl;
    std::cin >> resultFileName;
    if (dataToBinaryFile(processedData, resultFileName) ){
        std::cout << "Файл: Данные успешно упакованы в " << resultFileName << std::endl;
    }

    //пример ввода через консоль
    std::cout << "Введите exit, чтобы прекратить ввод из консоли" << std::endl;
    processedData = readConsoleToBytes();
    std::cout << "ввод с консоли прекращён!" << std::endl;

    std::cout << "введите название файла(с расширением), в который вы преобразуете информацию" << std::endl;
    std::cin >> resultFileName;
    if (dataToBinaryFile(processedData, resultFileName) ){
        std::cout << "Файл: Данные успешно упакованы в " << resultFileName << std::endl;
    }
    */
    return 0;
}