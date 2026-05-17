#include <vector>
#include <cstdint>
#include <string>


std::vector<uint32_t> transformStream(std::istream& source);
std::string dataToHex(const std::vector<uint8_t>& data);
bool dataToBinaryFile(const std::vector<uint8_t>& data, const std::string& filename);