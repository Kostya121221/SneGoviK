#include <iostream>
#include "input_output.h"
#include "ciphers/aes.h"

int main(){
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
    return 0;
}