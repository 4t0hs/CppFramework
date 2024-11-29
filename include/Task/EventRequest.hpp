#pragma once

#include <string>
#include <any>
#include "Exception/Exception.hpp"

namespace Framework::Task {

	template <typename T = int64_t>
	class EventRequest {
	public:
		using Command = T;
	private:
		std::string _from;
		Command _command { 0 };
		std::any _payload;
	public:
		EventRequest() = default;
		EventRequest(const std::string &from, Command command, const std::any &payload) :
			_from(from), _command(command), _payload(payload) {}
		EventRequest(const std::string &from, Command command, std::any &&payload) :
			_from(from), _command(command), _payload(std::move(payload)) {}
		EventRequest(const std::string &from, Command command) :
			_from(from), _command(command) {}
		EventRequest(const EventRequest &other) :
			_from(other._from), _command(other._command) {
			if (other.HasPayload())
				_payload = other._payload;
		}
		EventRequest(EventRequest &&other) :
			_from(std::move(other._from)), _command(other._command), _payload(std::move(other._payload)) {}

		EventRequest &operator=(const EventRequest &other) {
			if (this != &other) {
				_from = other._from;
				_command = other._command;
				if (other.HasPayload())
					_payload = other._payload;
			}
			return *this;
		}

		const std::string &GetFrom() const { return _from; }

		Command GetCommand() const { return _command; }

		bool HasPayload() const { return _payload.has_value(); }
		const std::any &GetPayload() const { return _payload; }
		template <typename U>
		const U &GetPayloadAs() const {
			return std::any_cast<const U&>(_payload);
		}
	};
} // namespace Framework::Task
