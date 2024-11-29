#pragma once

#include <filesystem>
#include <string_view>
#include <mutex>

namespace Framework::Main {
	using Path = std::filesystem::path;

	struct Name {
		static constexpr std::string_view ROOT{ "framework" };
		static constexpr std::string_view TASK{ "task" };
	};

	class Workspace {
		Path _address;
	public:
		Workspace(const Path &address) : _address{address} {}
		
		const Path &Address() const {
			return _address;
		}

		void Create() {
			if (!std::filesystem::exists(_address)) {
				std::filesystem::create_directory(_address);
			}
		}

		void Remove() {
			if (std::filesystem::exists(_address)) {
				std::filesystem::remove_all(_address);
			}
		}
	};

} // namespace Framework::Main
