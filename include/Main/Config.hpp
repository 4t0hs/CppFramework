#pragma once

#include <string_view>
#include <filesystem>
#include <mutex>
#include <map>

namespace Framework::Configuration {
	using Path = std::filesystem::path;

	struct Name {
		static constexpr std::string_view ROOT{ "framework" };
		static constexpr std::string_view TASK{ "task" };
	};

	class Address {
		static constexpr std::string_view _ROOT_ADDRESS{ "/tmp/framework" };
	public:
		static Path Root() {
			return Path{ _ROOT_ADDRESS };
		}
		static Path Task() {
			Path root = Root();
			return std::move(root.append(Name::TASK));
		}
	};
} // namespace Framework::Configuration
