#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <string>
#include <limits>
enum class PrintOptions {
    Clear,
    Main,
    EncDec,
    InOut,
    EncChoise,
};
extern "C" {
bool loginFunc();
std::vector<uint8_t> parseHexToBytes(const std::string& hexStr);
//из потокового ввода в вектор
std::vector<uint8_t> fromStreamToData(std::istream& source);
//из вектора в строку и наоборот
std::string dataToHex(const std::vector<uint8_t>& data);
std::vector<uint8_t>hexToData();
//из числа в строку и наоборот
std::string intToHex(uint64_t value);
uint64_t hexToInt(const std::string& hex_str);
//прочитать консоль
std::vector<uint8_t> readConsoleToBytes();
bool keysToBinaryFile(const std::string& text, const std::string& filename);
std::string fileToKeys(const std::string& filename);
//из вектора в файл
bool dataToBinaryFile(const std::vector<uint8_t>& data, const std::string& filename);

void printMenu(int choi);}
