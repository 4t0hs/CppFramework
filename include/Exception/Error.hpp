#pragma once

#include <string>

namespace Framework {
	class Error {
	public:
		enum class Code : int32_t {
			Unknown = -1,
			Success = 0,
			OutOfRange,
			InvalidArgument,
			TypeMismatch,
			InvalidOperation,
		};
	};
} // namespace Framework
