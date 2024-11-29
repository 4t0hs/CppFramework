#pragma once

#include <type_traits>

namespace Framework::Templates {

	template <typename T, typename std::enable_if_t<std::is_enum_v<T>, std::nullptr_t> = nullptr>
	class EnumCast {
	public:
		using Type = T;
		using UnderlyingType = std::underlying_type_t<T>;

		inline static constexpr Type ToEnum(UnderlyingType value) {
			return static_cast<Type>(value);
		}

		inline static constexpr UnderlyingType ToUnderlying(Type value) {
			return static_cast<UnderlyingType>(value);
		}
	};
} // namespace Framework::Templates
