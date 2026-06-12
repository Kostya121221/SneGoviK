#include "input_output.h"
#include <iostream>
#include <cstdlib> 
#include <thread>
#include <chrono>
#include <algorithm>
#include <functional>
const std::vector<size_t> allowed_passwords = {
        13092460576727867504ULL,
    };
const int64_t TIME_SLEEP = 1;

template <typename T>
T readNumber(const std::string& prompt, T min_val, T max_val) {
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
bool loginFunc(){
    std::cout << "Введите пароль от данного програмного обеспечения"<< std::endl;
    std::string password;
    std::cin >> password;

    std::hash<std::string> hasher;
    auto it = std::find(allowed_passwords.begin(), allowed_passwords.end(), hasher(password));
    return it != allowed_passwords.end();
}




std::vector<uint8_t> fromStreamToData(std::istream& source) {
    std::vector<uint8_t> resultBuffer;
    const size_t chunkSize = 4096; // Размер порции 
    while (source) {
        size_t oldSize = resultBuffer.size();
        resultBuffer.resize(oldSize + chunkSize);
        
        source.read(reinterpret_cast<char*>(&resultBuffer[oldSize]), chunkSize);;
        
        std::streamsize bytesRead = source.gcount();
        
        resultBuffer.resize(oldSize + bytesRead);
    }
    
    return resultBuffer;
}


std::vector<uint8_t> readConsoleToBytes() {
    std::vector<uint8_t> resultBuffer;
    std::string line;

    while (true) {
        if (!std::getline(std::cin, line)) {
            break; 
        }

        if (line == "exit") {
            break;
        }

        for (char c : line) {
            resultBuffer.push_back(static_cast<uint8_t>(c));
        }

        resultBuffer.push_back(static_cast<uint8_t>('\n'));
    }
    
    return resultBuffer;
}
std::string dataToHex(const std::vector<uint8_t>& data){
    std::stringstream ss;
    for (uint8_t byte : data) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
    }
    return ss.str();
};
std::vector<uint8_t>hexToData() {
    std::string input;
    std::cout << "Введите HEX-строку (байты через пробел): ";
    std::getline(std::cin, input);

    std::vector<uint8_t> data;
    std::stringstream ss(input);
    std::string byteStr;

    while (ss >> byteStr) {
        unsigned int byteVal;
        std::stringstream ss;
        
        ss << std::hex << byteStr;
        
        if (ss >> byteVal) {
            data.push_back(static_cast<uint8_t>(byteVal));
        } else {
            std::cerr << "Ошибка: некорректный байт '" << byteStr << "'" << std::endl;
        }
    }
    return data;
}

std::string intToHex(uint64_t value) {
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << value;
    return ss.str();
}

uint64_t hexToInt(const std::string& hex_str) {
    uint64_t value;
    std::stringstream ss;
    ss << std::hex << hex_str;
    if (!(ss >> value)) {
        return 0;
    }
    return value;
}


// функция создаёт абсолютно любой бинарный файл, и возвращает true если всё получилось
bool dataToBinaryFile(const std::vector<uint8_t>& data, const std::string& filename) {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) return false;
    
    outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
    return outFile.good(); 

};



void printMenu(int choi) {
    PrintOptions choise = PrintOptions(choi);
        switch (choise)
        {
        case PrintOptions::Clear:{
<<<<<<< HEAD
            std::this_thread::sleep_for(std::chrono::seconds(TIME_SLEEP));
=======
            std::this_thread::sleep_for(std::chrono::seconds(10));
>>>>>>> 808e67d (увелечение отклика программы)
            #if defined(_WIN32) || defined(_WIN64)
                std::system("cls");
            #elif defined(__linux__) || defined(__APPLE__)
                std::system("clear");
            #endif
            
            break;}
        
        case PrintOptions::Main:{
            std::cout << "\n=========================================================\n";
            std::cout << "               МЕНЮ ШИФРАТОРА 'SNEGOVIK'\n";
            std::cout << "=========================================================\n";
            break;
        }
        case PrintOptions::EncDec:{
            std::cout <<"ВЫБЕРИТЕ ШИФРОВКА/ДЕШИФРОВКА\n";
            std::cout <<"0.Выход\n";
            std::cout <<"1.Шифрование\n";
            std::cout <<"2.Дешифрование\n";
            std::cout <<"3.Генерация ключей\n";
            break;
        }
        case PrintOptions::InOut:{
                std::cout <<"0.Выход\n";
                std::cout <<"1.Файл\n";
                std::cout <<"2.Из консоли\n";
            break;
        }
        case PrintOptions::EncChoise:{
                std::cout <<"ВЫБЕРИТЕ МЕТОД ШИФРОВАНИЯ \n";
                std::cout <<"0.Выход\n";
                std::cout <<"1.Эль-Гамаль\n";
                std::cout <<"2.DES\n";
                std::cout <<"3.Шамир\n";
                std::cout <<"4.RSA\n";
                std::cout <<"5.RC4\n";
                std::cout <<"6.RC5\n";
            break;
        }
        default:
            break;
        }

}
std::vector<uint8_t> parseHexToBytes(const std::string& hexString) {
    std::string cleanHex;
    cleanHex.reserve(hexString.length());

<<<<<<< HEAD

=======
<<<<<<< HEAD
>>>>>>> ad785d6 (увелечение отклика программы)
    // оставляем только валидные HEX-символы
    for (char ch : hexString) {
        if (std::isxdigit(static_cast<unsigned char>(ch))) {
            cleanHex.push_back(ch);
        }
    }

    if (cleanHex.empty()) {
        throw std::invalid_argument("Пустой HEX-ввод.");
    }
    if (cleanHex.length() % 2 != 0) {
        throw std::invalid_argument("Нечетная длина HEX-строки после фильтрации.");
    }

    std::vector<uint8_t> bytes;
    bytes.reserve(cleanHex.length() / 2);

    for (size_t i = 0; i < cleanHex.length(); i += 2) {
        std::string byteString = cleanHex.substr(i, 2);
        
        uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
        bytes.push_back(byte);
    }

    return bytes;
}

<<<<<<< HEAD
<<<<<<< HEAD
template unsigned long readNumber<unsigned long>(const std::string&, unsigned long, unsigned long);
template int32_t readNumber<int32_t>(const std::string&, int32_t, int32_t);
template int64_t readNumber<int64_t>(const std::string&, int64_t, int64_t);
=======
template int readNumber<int>(const std::string&, int, int);
template long readNumber<long>(const std::string&, long, long);
=======
>>>>>>> 89f3547 (Исправление вайбкодинга maksima)
template unsigned long readNumber<unsigned long>(const std::string&, unsigned long, unsigned long);
<<<<<<< HEAD
=======
template int32_t readNumber<int32_t>(const std::string& prompt);
template int64_t readNumber<int64_t>(const std::string& prompt);
>>>>>>> 808e67d (увелечение отклика программы)
<<<<<<< HEAD
>>>>>>> ad785d6 (увелечение отклика программы)
=======
=======
template int32_t readNumber<int32_t>(const std::string&, int32_t, int32_t);
template int64_t readNumber<int64_t>(const std::string&, int64_t, int64_t);
>>>>>>> c3b8ea4 (Исправление вайбкодинга maksima)
>>>>>>> 89f3547 (Исправление вайбкодинга maksima)
