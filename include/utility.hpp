#pragma once

#include <fstream>

namespace Framework::Utility {
	bool CreateFile(const std::string &name) {
		std::ofstream file(name);
		bool opened = file.is_open();
		file.close();
		return opened;
	}
} // namespace Framework::Utility

namespace Framework {
#define __Likely(x) __builtin_expect(!!(x), 1)
#define __Unlikely(x) __builtin_expect(!!(x), 0)
} // namespace Framework
