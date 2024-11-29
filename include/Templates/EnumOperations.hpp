#pragma once

#include <type_traits>
#include "Templates/EnumCast.hpp"

namespace Framework::Templates::EnumOperations {

	namespace Concept {
		template <typename T>
		struct HasBitwiseOperators : std::false_type {};
	}

	template <typename T, typename U>
	struct AreEnumLogicSafety : public std::conditional_t<
		std::is_same_v<T, U> &&std::is_enum_v<T>,
		std::true_type,
		std::false_type> {};

	template <typename T, typename U>
	using EnumBitwiseEnabler = std::enable_if<Concept::HasBitwiseOperators<T>::value &&AreEnumLogicSafety<T, U>::value, std::nullptr_t>;

	template <typename T, typename U = T,
		typename EnumBitwiseEnabler<T, U>::type = nullptr>
	inline static constexpr T operator&(T lhs, U rhs) {
		using Cast = EnumCast<T>;
		return Cast::ToEnum(Cast::ToUnderlying(lhs) & Cast::ToUnderlying(rhs));
	}
	template <typename T, typename U = T,
		typename EnumBitwiseEnabler<T, U>::type = nullptr>
	inline static constexpr T &operator&= (T &lhs, U rhs) {
		using Cast = EnumCast<T>;
		lhs = Cast::ToEnum(Cast::ToUnderlying(lhs) & Cast::ToUnderlying(rhs));
		return lhs;
	}

	template <typename T, typename U = T,
		typename EnumBitwiseEnabler<T, U>::type = nullptr>
	inline static constexpr T operator|(T lhs, U rhs) {
		using Cast = EnumCast<T>;
		return Cast::ToEnum(Cast::ToUnderlying(lhs) | Cast::ToUnderlying(rhs));
	}

	template <typename T, typename U = T,
		typename EnumBitwiseEnabler<T, U>::type = nullptr>
	inline static constexpr T &operator|= (T &lhs, U rhs) {
		using Cast = EnumCast<T>;
		lhs = Cast::ToEnum(Cast::ToUnderlying(lhs) | Cast::ToUnderlying(rhs));
		return lhs;
	}

	template <typename T, typename U = T,
		typename EnumBitwiseEnabler<T, U>::type = nullptr>
	inline static constexpr T operator~(T value) {
		using Cast = EnumCast<T>;
		return Cast::ToEnum(~Cast::ToUnderlying(value));
	}

	template <typename T, typename U = T,
		typename EnumBitwiseEnabler<T, U>::type = nullptr>
	inline static constexpr T operator^(T lhs, U rhs) {
		using Cast = EnumCast<T>;
		return Cast::ToEnum(Cast::ToUnderlying(lhs) ^ Cast::ToUnderlying(rhs));
	}

	template <typename T, typename U = T,
		typename EnumBitwiseEnabler<T, U>::type = nullptr>
	inline static constexpr T &operator^= (T &lhs, U rhs) {
		using Cast = EnumCast<T>;
		lhs = Cast::ToEnum(Cast::ToUnderlying(lhs) ^ Cast::ToUnderlying(rhs));
		return lhs;
	}
} // Framework::Templates::EnumOperations
