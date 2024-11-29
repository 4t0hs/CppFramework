#pragma once

#include <stdexcept>
#include <string>
#include "Error.hpp"

namespace Framework {
	class Exception : public std::runtime_error {
		Error::Code _code;

		static std::string _BuildErrorMessage(const std::string &message, Error::Code code) {
			return message + " (Error Code: " + std::to_string(static_cast<int>(code)) + ")";
		}

	public:
		Exception(const std::string &message, Error::Code code = Error::Code::Unknown)
			: std::runtime_error(_BuildErrorMessage(message, code)), _code(code) {}

		Exception(const char *message, Error::Code code = Error::Code::Unknown)
			: std::runtime_error(_BuildErrorMessage(message, code)), _code(code) {}

		Error::Code GetCode() const { return _code; }
	};
} // namespace Framework
