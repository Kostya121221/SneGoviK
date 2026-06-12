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
template <typename T>
T readNumber(const std::string& prompt = "", 
             T min_val = std::numeric_limits<T>::lowest(), 
             T max_val = std::numeric_limits<T>::max());
bool loginFunc();
std::vector<uint8_t> parseHexToBytes(const std::string& hexStr);
std::vector<uint8_t> fromStreamToData(std::istream& source);
//из вектора в строку и наоборот
std::string dataToHex(const std::vector<uint8_t>& data);
std::vector<uint8_t>hexToData();
//из числа в строку и наоборот
std::string intToHex(uint64_t value);
uint64_t hexToInt(const std::string& hex_str);
//прочитать консоль
std::vector<uint8_t> readConsoleToBytes();
bool dataToBinaryFile(const std::vector<uint8_t>& data, const std::string& filename);
void printMenu(int choi);