#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <format>
#include <random>
#include <regex>

int main()
{
    auto random = std::random_device();
    auto tmpFile = std::filesystem::temp_directory_path() / std::format("seed_{}", random());

	std::system(std::format("Extract.exe > {}", tmpFile.string().c_str()).c_str());
    long fileSize = std::filesystem::file_size(tmpFile);
    auto srcBuff = std::vector<char>(fileSize);
    {
        auto sr = std::ifstream(tmpFile);
        sr.read(srcBuff.data(), fileSize);
    }
    std::filesystem::remove(tmpFile);

    auto seed = std::regex_replace(srcBuff.data(), std::regex("\\n"), "");
    std::cout << seed << std::endl;
}
