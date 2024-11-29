#pragma once
#include <functional>

namespace Framework::Templates {

	class ReferenceProperty {
		template <typename T>
		class Base {
		protected:
			T &_value;
		public:
			Base(T &value) : _value(value) {}
			virtual ~Base() {};
		};
	public:
		template <typename T>
		class DefaultGetter {
		public:
			static const T &Get(const T &value) {
				return value;
			}
		};
		template <typename T>
		class DefaultSetter {
		public:
			static void Set(T &value, const T &newValue) {
				value = newValue;
			}
		};

		template <typename T, typename Getter = DefaultGetter<T>>
		class ReadOnly : public Base<T> {
		public:
			ReadOnly(T &value) : Base<T>(value) {}
			ReadOnly(const T &value) : Base<T>(const_cast<T &>(value)) {}

			operator auto () const {
				return Getter::Get(this->_value);
			}
		};

		template <typename T, typename Getter = DefaultGetter<T>, typename Setter = DefaultSetter<T>>
		class Writable : public ReadOnly<T, Getter> {
		public:
			Writable(T &value) : ReadOnly<T, Getter>(value) {}

			auto &operator=(const T &value) {
				Setter::Set(this->_value, value);
				return *this;
			}
		};

		template <typename T>
		class FunctionSetter : public Base<T> {
		public:
			using Type = T;

			FunctionSetter(Type &value) : Base<Type>(value) {}

			template <typename F>
			auto &operator=(F &&value) {
				this->_value = std::forward<F>(value);
				return *this;
			}
		};

		template <typename Ret, typename... ArgTypes>
		class FunctionSetter<Ret(ArgTypes...)> : public Base<std::function<Ret(ArgTypes...)>> {
		public:
			using Type = std::function<Ret(ArgTypes...)>;

			FunctionSetter(Type &value) : Base<Type>(value) {}

			template <typename F>
			auto &operator=(F &&value) {
				this->_value = std::forward<F>(value);
				return *this;
			}
		};
	};

	template <typename T>
	class Property {
	public:
		Property() = delete;
		~Property() = delete;

		using SetterType = std::function<void(const T&)>;
		using GetterType = std::function<T()>;

		class ReadOnly {
		protected:
			GetterType _getter;
		public:
			ReadOnly(GetterType getter) : _getter(getter) {}

			operator T() const {
				return _getter();
			}
		};

		class Writable : public ReadOnly {
		protected:
			SetterType _setter;
		public:
			Writable(GetterType getter, SetterType setter) : ReadOnly(getter), _setter(setter) {}

			auto &operator=(const T &value) {
				_setter(value);
				return *this;
			}
		};
	};
} // namespace Framework::Templates
