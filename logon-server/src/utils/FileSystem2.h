#pragma once

#include <BaseApp.h>

#include <string>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>

/**
 * @file FileSystem2.h
 * @brief A cross-platform utility namespace for file operations, including JSON and .env file support.
 * @author Oxygen
 * @date 2025-07-12
 */

namespace FileSystem2 {

	/**
	 * @brief Reads the content of a file as a string.
	 * @param filePath The path to the file.
	 * @return std::string The content of the file.
	 * @throws std::runtime_error If the file cannot be opened or read.
	 */
	std::string readFile(const std::string& filePath);

	/**
	 * @brief Writes content to a file.
	 * @param filePath The path to the file.
	 * @param content The content to write.
	 * @param append If true, appends to the file; if false, overwrites it.
	 * @throws std::runtime_error If the file cannot be opened or written.
	 */
	void writeFile(const std::string& filePath, const std::string& content, bool append = false);

	/**
	 * @brief Reads a JSON file and parses it into a JSON object.
	 * @param filePath The path to the JSON file.
	 * @return nlohmann::json The parsed JSON object.
	 * @throws std::runtime_error If the file cannot be read or is invalid JSON.
	 */
	nlohmann::json readJson(const std::string& filePath);

	/**
	 * @brief Writes a JSON object to a file.
	 * @param filePath The path to the JSON file.
	 * @param jsonData The JSON object to write.
	 * @param indent The indentation level for pretty-printing (-1 for compact).
	 * @throws std::runtime_error If the file cannot be written.
	 */
	void writeJson(const std::string& filePath, const nlohmann::json& jsonData, int indent = 4);

	/**
	 * @brief Reads a .env file and parses it into a key-value map.
	 * @param filePath The path to the .env file.
	 * @return std::unordered_map<std::string, std::string> A map of key-value pairs.
	 * @throws std::runtime_error If the file cannot be read or parsed.
	 */
	std::unordered_map<std::string, std::string> readEnv(const std::string& filePath);

	/**
	 * @brief Writes key-value pairs to a .env file.
	 * @param filePath The path to the .env file.
	 * @param envData The key-value pairs to write.
	 * @param append If true, appends to the file; if false, overwrites it.
	 * @throws std::runtime_error If the file cannot be written.
	 */
	void writeEnv(const std::string& filePath, const std::unordered_map<std::string, std::string>& envData, bool append = false);

	int countFiles(const std::string& path);
	int countAllEntries(const std::string& path);
}