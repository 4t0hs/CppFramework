#pragma once

#include <memory>
#include <thread>
#include <future>
#include <vector>
#include <string>

#include "Templates/EnumBitset.hpp"

#include "Task/EventRequest.hpp"
#include "Task/interface/IMessageTask.hpp"
#include "Task/interface/IEventAggregator.hpp"

#include "Message/IMessageQueue.hpp"
#include "Message/MessageQueueFactory.hpp"

namespace Framework::Task {
	using namespace Framework;

	template <typename T = EventRequest<>::Command>
	class MessageEventArgs {
		const EventRequest<T> *_content{ nullptr };
	public:
		MessageEventArgs(const EventRequest<T> *content) : _content(content) {}

		const EventRequest<T> &GetRequest() const { return *_content; }
	};

	template <typename T = EventRequest<>::Command>
	class EventTaskBase : public IEventTask<T>, TaskBase {
	public:
		using EventAggregator =
			IEventAggregator<typename EventRequest<T>::Command, const MessageEventArgs<T> &>;
	private:
		using _EventRequest = EventRequest<T>;
		using Command = _EventRequest::Command;
		static constexpr std::chrono::milliseconds WAIT_FOREVER = IEventTask<T>::WAIT_FOREVER;

		class Attribute final {
		public:
			enum Type : uint8_t {
				NONE = 0,
				INTERNAL,
				EXTERNAL,
			};
		private:
			Type _type{ NONE };
		public:
			Attribute() = default;
			Attribute(Type flags) : _type(flags) {}
			bool IsInternal() const { return _type == INTERNAL; }
			bool IsExternal() const { return _type == EXTERNAL; }
		};

		class Response final {
			std::promise<bool> _response;
		public:
			std::future<bool> GetFuture() { return _response.get_future(); }

			void Set(bool response) { _response.set_value(response); }
			void HandleException() { _response.set_exception(std::current_exception()); }
		};

		class InternalCommands final {
		public:
			static constexpr EventRequest<>::Command START = 0;
			static constexpr EventRequest<>::Command STOP = 1;
		};

		class MessageContent {
			Attribute _attribute{};
			_EventRequest _request{};
			std::shared_ptr<Response> _response{ nullptr };
		public:
			MessageContent() = default;
			MessageContent(Attribute attribute, const _EventRequest &request, std::shared_ptr<Response> response = nullptr) :
				_attribute(attribute), _request(request), _response(response) {}
			MessageContent(Attribute attribute, _EventRequest &&request, std::shared_ptr<Response> response = nullptr) :
				_attribute(attribute), _request(std::move(request)), _response(response) {}

			const auto &GetAttribute() const { return _attribute; }
			const auto &GetRequest() const { return _request; }
			auto &GetResponseBuffer() const { return _response; }
			bool IsResponseRequired() const { return _response != nullptr; }
		};

		using MessageQueue = Message::IMessageQueue<MessageContent>;

		class Sender {
			std::weak_ptr<MessageQueue> _messageQueue;
			std::shared_ptr<Response> _response{ nullptr };
			bool sent{ false };
		public:
			Sender(const std::shared_ptr<MessageQueue> &messageQueue, bool needResponse = false) :
				_messageQueue(messageQueue), _response(needResponse ? std::make_shared<Response>() : nullptr) {}

			void Send(Attribute attribute, const _EventRequest &request) {
				if (auto messageQueue = _messageQueue.lock()) {
					messageQueue->Send({ attribute, request, _response });
					sent = true;
				}
			}

			bool WaitForResponse(std::chrono::milliseconds timeoutMsec = WAIT_FOREVER) {
				if (!_response || !sent) {
					return false;
				}
				std::future<bool> future = _response->GetFuture();
				if (timeoutMsec == WAIT_FOREVER) {
					future.wait();
				} else {
					if (future.wait_for(timeoutMsec) == std::future_status::timeout) {
						return false;
					}
				}
				return future.get();
			}
		private:
		};

		EventAggregator *const _eventAggregator{ nullptr };
		std::shared_ptr<MessageQueue> _messageQueue;

		std::function<void()> _onStart;
		std::function<void()> _onFinish;
		bool stop = false;
	public:
		EventTaskBase(TaskType type, const std::string &name,
			EventAggregator *eventAggregator) :
			TaskBase(type, name), _eventAggregator(eventAggregator),
			_messageQueue(Message::MessageQueueFactory::Create<MessageContent>()) {

			_thread = std::thread([this]() {
				_Mainloop();
			});
		}

		~EventTaskBase() {
			Stop();
		}

		void Start() override {
			Sender sender{ _messageQueue, true };
			sender.Send(Attribute::INTERNAL, _EventRequest{
				"", static_cast<T>(InternalCommands::START) });
			sender.WaitForResponse();
		}

		void Stop() override {
			if (!IsRunning()) {
				return;
			}
			Sender sender{ _messageQueue, true };
			sender.Send(Attribute::INTERNAL, _EventRequest{
				"", static_cast<T>(InternalCommands::STOP) });
			sender.WaitForResponse();
			_thread.join();
		}

		void SendEvent(_EventRequest &&request) override {
			Sender sender(_messageQueue, false);
			sender.Send(Attribute::EXTERNAL, std::move(request));
		}

		void SendEvent(const _EventRequest &request) override {
			Sender sender(_messageQueue, false);
			sender.Send(Attribute::EXTERNAL, request);
		}

		bool RpcEvent(_EventRequest &&request, std::chrono::milliseconds timeoutMsec = WAIT_FOREVER) override {
			Sender sender(_messageQueue, true);
			sender.Send(Attribute::EXTERNAL, std::move(request));
			return sender.WaitForResponse(timeoutMsec);
		}

		bool RpcEvent(const _EventRequest &request, std::chrono::milliseconds timeoutMsec = WAIT_FOREVER) override {
			Sender sender(_messageQueue, true);
			sender.Send(Attribute::EXTERNAL, request);
			return sender.WaitForResponse(timeoutMsec);
		}

		void SetOnStart(const std::function<void()> &onStart) {
			_onStart = onStart;
		}

		void SetOnFinish(const std::function<void()> &onFinish) {
			_onFinish = onFinish;
		}

		bool IsRunning() const noexcept {
			return _thread.joinable();
		}
	private:
		void _Mainloop() {
			while (!stop) {
				MessageContent content = _messageQueue->Receive();
				try {
					bool responseValue = true;
					if (content.GetAttribute().IsInternal()) {
						_ProcessInternalCommand(content.GetRequest());
					} else {
						responseValue = _ProcessEvent(content.GetRequest());
					}
					if (content.IsResponseRequired()) {
						content.GetResponseBuffer()->Set(responseValue);
					}
				} catch (...) {
					if (content.IsResponseRequired()) {
						content.GetResponseBuffer()->HandleException();
					}
				}
			}
			if (_onFinish) _onFinish();
		}

		void _ProcessInternalCommand(const _EventRequest &request) {
			EventRequest<>::Command command =
				static_cast<EventRequest<>::Command>(request.GetCommand());
			switch (command) {
			case InternalCommands::START:
				if (_onStart) _onStart();
				break;
			case InternalCommands::STOP:
				stop = true;
				break;
			default:
				break;
			}
		}

		bool _ProcessEvent(const _EventRequest &request) {
			if (__Likely(_eventAggregator)) {
				return _eventAggregator->Publish(request.GetCommand(), MessageEventArgs(&request));
			}
			return false;
		}
	};
} // namespace Framework::Task
