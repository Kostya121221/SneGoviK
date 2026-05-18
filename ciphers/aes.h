#pragma once
#include <vector>
#include <cstdint>

std::vector<uint32_t> fromVectorToAES(std::vector<uint32_t> data);
std::vector<uint32_t> fromAESToVector(std::vector<uint32_t> data);