#pragma once

namespace utils {
	namespace file {
		bool write_binary(void* binary, int size, const char* name);
		void* read_binary(const char* filename, size_t* file_size);
	}
}