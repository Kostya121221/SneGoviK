#include "input_output.h"
#include <iostream>
#include <cstdlib> 
#include <thread>
#include <chrono>
#include <algorithm>
const std::vector<std::string> allowed_passwords = {
        "SGK"
    };

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
    auto it = std::find(allowed_passwords.begin(), allowed_passwords.end(), password);
    return it != allowed_passwords.end();
}




std::vector<uint8_t> fromStreamToData(std::istream& source) {
    std::vector<uint8_t> resultBuffer;
    // Мы не можем просто перебрать весь поток, поэтому берём порционно информацию
    const size_t chunkSize = 4096; // Размер порции 
    //Пока у нас есть инфа в source
    while (source) {
        size_t oldSize = resultBuffer.size();
        // Выделяем место под новую порцию данных
        resultBuffer.resize(oldSize + chunkSize);
        
        // Читаем напрямую в хвост вектора. 
        // Так как мы обрабатываем абсолтно любую информацию,
        // Мы должны отдавать сырые байты на обработку, 
        // при помощи reinterpret_cast<char*> мы говорим, что это всё поросто байты
        //(&resultBuffer[oldSize]), chunkSize) - в скобках сообщаем куда читать и сколько
        source.read(reinterpret_cast<char*>(&resultBuffer[oldSize]), chunkSize);;
        
        std::streamsize bytesRead = source.gcount();
        
        // Корректируем размер вектора под реально прочитанное количество, нужен когда аканчивается инфа в source
        resultBuffer.resize(oldSize + bytesRead);
    }
    
    return resultBuffer;
}


std::vector<uint8_t> readConsoleToBytes() {
    std::vector<uint8_t> resultBuffer;
    std::string line;

    while (true) {
        // Проверка, что с потоком всё окей
        if (!std::getline(std::cin, line)) {
            break; 
        }

        // Выйти если написано в консоль слово exit
        if (line == "exit") {
            break;
        }

        // Записываем символы строки в вектор
        for (char c : line) {
            resultBuffer.push_back(static_cast<uint8_t>(c));
        }

        // Сохраняем нажатый Enter как байт переноса строки (\n)
        resultBuffer.push_back(static_cast<uint8_t>('\n'));
    }
    
    return resultBuffer;
}
// функция возвращает строку с 16-ти ричными числами
// Она нужна, чтобы сделать всё через строчный поток,
// потому что это легче и практичнее. Так бы пришлось все числа сначала поочерёдно форматировать, потом в строку
// А тут уже встроенные инструменты превращающие всё в поток 
std::string dataToHex(const std::vector<uint8_t>& data){
    std::stringstream ss;
    for (uint8_t byte : data) {
        //если коротко - Все числа переведи в 16-тиричные -> каждому элементу выделено 2 символа под вывод ->пустота заполняется нулями
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
    }
    //Строчный поток вернётся в виде строки
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
        // для проверки количества введённых байт
        unsigned int byteVal;
        std::stringstream ss;
        
        // Передаем строку и сразу настраиваем на HEX-формат
        ss << std::hex << byteStr;
        
        // Проверяем, прошла ли конвертация успешно
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
    //готовим файл для записи с именем filename 
    std::ofstream outFile(filename, std::ios::binary);//std::ios::binary - флаг работы с бинарным файлом
    if (!outFile) return false; //если на проглом шаге что-то не так - вернёт false
    
    // Тут мы просто 
    outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
    // метод good() Вернет true, если запись прошла без ошибок диска
    return outFile.good(); 

};



void printMenu(int choi) {
    PrintOptions choise = PrintOptions(choi);
        switch (choise)
        {
        case PrintOptions::Clear:{
            std::this_thread::sleep_for(std::chrono::seconds(10));
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


    // оставляем только валидные HEX-символы
    for (char ch : hexString) {
        if (std::isxdigit(static_cast<unsigned char>(ch))) {
            cleanHex.push_back(ch);
        }
    }

    // Если после чистки строка пустая или нечетная — это уже явная ошибка ввода
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

template int readNumber<int>(const std::string&, int, int);
template long readNumber<long>(const std::string&, long, long);
template unsigned long readNumber<unsigned long>(const std::string&, unsigned long, unsigned long);
template int32_t readNumber<int32_t>(const std::string& prompt);
template int64_t readNumber<int64_t>(const std::string& prompt);
