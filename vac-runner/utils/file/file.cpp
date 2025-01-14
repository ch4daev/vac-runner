#include "file.hpp"

#include <fstream>

namespace utils {
	namespace file {
		bool write_binary(void* binary, int size, const char* filename) {
			std::fstream stream;

			stream.open(filename, std::fstream::binary | std::fstream::out);

			if (stream.fail()) {
				return false;
			}

			stream.write(reinterpret_cast<char*>(binary), size);

			if (stream.fail()) {
				return false;
			}

			return true;
		}
		void* read_binary(const char* filename, size_t* file_size) {
			if (file_size == nullptr) {
				return nullptr;
			}

			std::ifstream stream(filename, std::ios::binary);

			if (!stream) {
				return nullptr;
			}

			stream.seekg(0, std::ios::end);

			*file_size = stream.tellg();

			stream.seekg(0, std::ios::beg);

			char *buffer = new char[*file_size];

			stream.read(buffer, *file_size);

			if (stream.fail()) {
				return nullptr;
			}

			return buffer;
		}
	}
}