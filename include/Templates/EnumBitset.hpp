#pragma once

#include <bitset>
#include <cstddef>
#include <string>
#include <type_traits>
#include "utility.hpp"
#include "Templates/EnumCast.hpp"

namespace Framework::Templates {

	template <typename T,
		std::size_t MaxBits = sizeof(std::underlying_type_t<T>) * 8,
		typename std::enable_if_t<std::is_enum_v<T> &&
		MaxBits <= (sizeof(std::underlying_type_t<T>) * 8),
		nullptr_t> = nullptr>
	class EnumBitset : std::bitset<MaxBits> {

		using _base = std::bitset<MaxBits>;
	public:
		using EnumCast = class EnumCast<T>;
		using Type = typename EnumCast::Type;
		using UnderlyingType = typename EnumCast::UnderlyingType;

		static constexpr size_t _MaxBits = MaxBits;

		constexpr EnumBitset() noexcept : _base() {}

		constexpr EnumBitset(std::initializer_list<Type> init) noexcept :
			_base(EnumCast::ToUnderlying(OrAll(init))) {}

		constexpr EnumBitset(Type value) noexcept : EnumBitset({ value }) {}

		EnumBitset &operator&=(const EnumBitset &other) noexcept {
			_base::operator&=(other);
			return *this;
		}

		EnumBitset &operator|=(const EnumBitset &other) noexcept {
			_base::operator|=(other);
			return *this;
		}

		EnumBitset &operator^=(const EnumBitset &other) noexcept {
			_base::operator^=(other);
			return *this;
		}

		EnumBitset &Set() noexcept {
			_base::set();
			return *this;
		}

		EnumBitset &Set(Type _enum, bool value = true) {
			if (__Unlikely(EnumCast::ToUnderlying(_enum) == 0)) {
				return *this;
			}
			_base::set(_ToBitPosition(_enum), value);
			return *this;
		}

		EnumBitset &Reset() noexcept {
			_base::reset();
			return *this;
		}

		EnumBitset &Reset(Type _value) {
			if (__Unlikely(EnumCast::ToUnderlying(_value) == 0)) {
				return *this;
			}
			_base::reset(_ToBitPosition(_value));
			return *this;
		}

		EnumBitset operator~() const noexcept {
			return _base::operator~();
		}

		EnumBitset &Flip() noexcept {
			_base::flip();
			return *this;
		}

		EnumBitset &Flip(Type value) {
			if (__Unlikely(EnumCast::ToUnderlying(value) == 0)) {
				return *this;
			}
			_base::flip(_ToBitPosition(value));
			return *this;
		}

		bool operator[](UnderlyingType position) const {
			return _base::operator[](position);
		}

		size_t Count() const noexcept {
			return _base::count();
		}

		bool Test(Type value) const {
			if (__Unlikely(EnumCast::ToUnderlying(value) == 0)) {
				return false;
			}
			return Test(_ToBitPosition(value));
		}

		bool Test(UnderlyingType position) const {
			return _base::test(position);
		}

		bool All() const noexcept {
			return _base::all();
		}

		bool Any() const noexcept {
			return _base::any();
		}

		bool None() const noexcept {
			return _base::none();
		}

		bool operator==(const EnumBitset &other) const noexcept {
			return _base::operator==(other);
		}

		bool operator!=(const EnumBitset &other) const noexcept {
			return !_base::operator==(other);
		}

		inline UnderlyingType ToUnderlying() const noexcept {
			return _base::to_ullong();
		}

		inline Type ToEnum() const {
			return EnumCast::ToEnum(ToUnderlying());
		}

		std::string ToString() const {
			return _base::to_string();
		}

		static constexpr Type OrAll(std::initializer_list<Type> init) noexcept {
			UnderlyingType result = EnumCast::ToUnderlying(Type(0));
			for (Type value : init) {
				UnderlyingType underlying = EnumCast::ToUnderlying(value);
				result |= underlying;
			}
			return EnumCast::ToEnum(result);
		}
	private:
		inline static constexpr UnderlyingType _ToBitPosition(Type value) {
			return __builtin_ctzll(EnumCast::ToUnderlying(value));
		}
	};
} // namespace Framework::Templates
