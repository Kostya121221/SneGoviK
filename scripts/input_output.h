#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <string>


std::vector<uint8_t> fromStreamToData(std::istream& source);
std::string dataToHex(const std::vector<uint8_t>& data);
std::vector<uint8_t>hexToData();
std::vector<uint8_t> readConsoleToBytes();
bool dataToBinaryFile(const std::vector<uint8_t>& data, const std::string& filename);
void clearScreen();
void printMenu();