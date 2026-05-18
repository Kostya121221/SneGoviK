#include "input_output.h"

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
std::string dataToHex(const std::vector<uint32_t>& data){
    std::stringstream ss;
    for (uint8_t byte : data) {
        //если коротко - Все числа переведи в 16-тиричные -> каждому элементу выделено 2 символа под вывод ->пустота заполняется нулями
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
    }
    //Строчный поток вернётся в виде строки
    return ss.str();
};


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