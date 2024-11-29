#pragma once

#include <string>
#include <vector>

namespace Framework::SubProcess {
	class StartInfo {
		std::string _command;
		std::vector<std::string> _arguments;
		std::vector<std::string> _environments;
		bool _redirectStandardOutput{ false };
		bool _redirectStandardError{ false };
		bool _useShell{ false };
	public:
		StartInfo() = default;
		StartInfo(const std::string &command) : _command(command) {}

		std::string &Command() {
			return _command;
		}

		std::vector<std::string> Arguments() {
			return _arguments;
		}

		std::vector<std::string> Environments() {
			return _environments;
		}

		bool &RedirectStandardOutput() {
			return _redirectStandardOutput;
		}

		bool &RedirectStandardError() {
			return _redirectStandardError;
		}

		bool &UseShell() {
			return _useShell;
		}

		std::string GetCommandLine() const {
			std::string result = _command;
			for (const auto &argument : _arguments) {
				result += " " + argument;
			}
			return result;
		}

		const char **CStyleEnvironments() {
			const char **result = new const char *[_environments.size() + 1];
			for (size_t i = 0; i < _environments.size(); ++i) {
				result[i] = _environments[i].c_str();
			}
			result[_environments.size()] = nullptr;
			return result;
		}
	};
}; // namespace Framework::SubProcess
