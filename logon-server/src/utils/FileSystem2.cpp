#include "FileSystem2.h"

namespace FileSystem2 {

    namespace {
        // Trims whitespace from the beginning and end of a string.
        // @param str The string to trim.
        // @return The trimmed string.
        std::string trim(const std::string& str) {
            std::string result = str;
            result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](unsigned char c) {
                return !std::isspace(c);
                }));
            result.erase(std::find_if(result.rbegin(), result.rend(), [](unsigned char c) {
                return !std::isspace(c);
                }).base(), result.end());
            return result;
        }
    } // anonymous namespace

    std::string readFile(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + filePath);
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }

    void writeFile(const std::string& filePath, const std::string& content, bool append) {
        try {
            std::filesystem::path path = std::filesystem::weakly_canonical(filePath); // canonicalisasi, tapi toleran path belum ada

            // Validasi keamanan path
            const std::vector<std::string> forbidden_roots = {
    #ifdef _WIN32
                "C:\\Windows", "C:\\Program Files", "C:\\ProgramData", "C:\\Users\\Default"
    #else
                "/etc", "/bin", "/boot", "/dev", "/lib", "/proc", "/root", "/sys", "/usr", "/var"
    #endif
            };

            for (const auto& root : forbidden_roots) {
                std::string path_s = path.string();
                if (path_s.starts_with(root)) {
                    throw std::runtime_error("writeFile blocked: Attempt to write to restricted path: " + path.string());
                }
            }

            // Jangan izinkan menulis langsung ke root dir (misalnya "/file.txt" atau "C:\\file.txt")
            if (path.parent_path() == path.root_path()) {
                throw std::runtime_error("writeFile blocked: Writing directly to root directory is not allowed.");
            }

            std::filesystem::create_directories(path.parent_path()); // Buat folder kalau belum ada

            std::ofstream file(path, append ? std::ios::app : std::ios::out | std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot write to file: " + path.string());
            }

            file << content;
            file.close();
        }
        catch (const std::exception& e) {
            throw std::runtime_error("writeFile failed: " + std::string(e.what()));
        }
    }

    nlohmann::json readJson(const std::string& filePath) {
        try {
            std::string content = readFile(filePath);
            return nlohmann::json::parse(content);
        }
        catch (const nlohmann::json::exception& e) {
            throw std::runtime_error("Invalid JSON in file: " + filePath + " (" + e.what() + ")");
        }
        catch (const std::runtime_error& e) {
            throw std::runtime_error("readJson failed: " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            throw std::runtime_error("readJson failed: " + std::string(e.what()));
        }
    }

    void writeJson(const std::string& filePath, const nlohmann::json& jsonData, int indent) {
        try {
            std::string content = jsonData.dump(indent);
            writeFile(filePath, content);
        }
        catch (const nlohmann::json::exception& e) {
            throw std::runtime_error("Failed to serialize JSON to file: " + filePath + " (" + e.what() + ")");
        }
        catch (const std::runtime_error& e) {
            throw std::runtime_error("writeJson failed: " + std::string(e.what()));
        }
        catch (const std::exception& e) {
            throw std::runtime_error("writeJson failed: " + std::string(e.what()));
        }
    }

    std::unordered_map<std::string, std::string> readEnv(const std::string& filePath) {
        std::unordered_map<std::string, std::string> envData;
        std::string content = readFile(filePath);
        std::stringstream ss(content);
        std::string line;

        while (std::getline(ss, line)) {
            // Skip empty lines or comments
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }

            // Find the first '=' to split key and value
            auto pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                if (!key.empty()) {
                    envData[key] = value;
                }
            }
        }

        return envData;
    }

    void writeEnv(const std::string& filePath, const std::unordered_map<std::string, std::string>& envData, bool append) {
        std::stringstream buffer;
        for (const auto& pair : envData) {
            buffer << pair.first << "=" << pair.second << "\n";
        }
        writeFile(filePath, buffer.str(), append);
    }

} // namespace FileSystem2