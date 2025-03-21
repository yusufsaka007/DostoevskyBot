#ifndef ID_HANDLER_H
#define ID_HANDLER_H

#include <iostream>
#include <string>
#include <fstream>

const const char* FILENAME = "id.txt";

namespace j61sbaazstls3ek { // Id Handler
	bool check_if_exists() {
		std::ifstream file(FILENAME);
		return file.good();
	}
	bool is_empty() {
		std::ifstream file(FILENAME);
		return file.peek() == std::ifstream::traits_type::eof();
	}
	void create_file() {
		std::ofstream newFile(FILENAME);
	}
	void read_id(std::string& id) {
		std::ifstream file(FILENAME);
		if (file) {
			std::getline(file, id);
		}
	}
	void write_id(const std::string& id) {
		std::ofstream file(FILENAME);
		if (file) {
			file << id;
		}
	}
};

#endif // !ID_HANDLER_H
