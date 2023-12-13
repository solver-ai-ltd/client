#include <string>
#include <filesystem>
#include <fstream>
#include <map>

const std::string data_file_folder_path = std::filesystem::absolute(std::filesystem::path(__FILE__).parent_path() / ".." / "example_files").string();
const std::string setupFilePath = std::filesystem::absolute(std::filesystem::path(__FILE__).parent_path() / ".." / "setup/setup.txt").string();

std::map<std::string, std::string>
readSetupFile()
{
    std::map<std::string, std::string> config;
    std::ifstream file(setupFilePath);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '=')) {
            std::string value;
            if (std::getline(is_line, value)) {
                config[key] = value;
            }
        }
    }
    return config;
}
