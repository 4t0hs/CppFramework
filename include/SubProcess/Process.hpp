#pragma once

#include <unistd.h>
#include <wait.h>
#include <sys/resource.h>
#include <chrono>
#include <fstream>
#include <thread>

#include "Templates/Property.hpp"

#include "SubProcess/StartInfo.hpp"

namespace Framework::SubProcess {
	class Process {
	public:
		static constexpr int PROCESS_FAILED = -255;
	private:
		StartInfo _startInfo;
		pid_t _id {-1};
		int32_t _status {-1};
		rusage _usage;

	public:
		Process() = default;
		Process(const StartInfo &startInfo) : _startInfo(startInfo) {}

		static Process StartAsync(const StartInfo &startInfo) {
			Process process{ startInfo };
			process.StartAsync();
			return process;
		}
		
		static Process Start(const StartInfo &startInfo) {
			Process process = StartAsync(startInfo);
			process.Wait();
			return process;
		}

		void Wait() {
			if (HasExited()) {
				return;
			}
			wait4(_id, &_status, 0, &_usage);
		}

		bool Wait(std::chrono::milliseconds milliSeconds) {
			static constexpr auto INTERVAL = std::chrono::milliseconds(10).count();
			if (HasExited()) {
				return true;
			}
			uint64_t countDown;
			if (milliSeconds.count() % INTERVAL == 0) {
				countDown = milliSeconds.count() / INTERVAL;
			} else {
				countDown = milliSeconds.count() / INTERVAL + 1;
			}
			while (countDown--) {
				if (wait4(_id, &_status, 0, &_usage) > 0) {
					return true;
				}
				usleep(INTERVAL);
			}
			return false;
		}

		void StartAsync() {
			// メモリ空間を共有した子プロセスを生成
			// process spawnerクラスが必要
		}

		void Start() {
			StartAsync();
			Wait();
		}

		void Kill() {}

		// properties

		int ExitCode();

		bool HasExited();

		pid_t Id();

		auto StartTime();

		auto ExitTime();

		std::ifstream StandardOutput();

		std::ifstream StandardError();

		auto TotalProcessorTime();

		auto UserProcessorTime();
		// events
		
		// Templates::ReferenceProperty::FunctionSetter<ExitedEventHandler> Exited { _exitedHandler };
	private:
		static int ChildProcess(StartInfo *info) {
			// リダイレクトの設定
			// コマンドの実行
			// ILauncherのインターフェースを用意して、実装はshellと普通のに分ける
			return PROCESS_FAILED;
		}

		bool _Wait(int options) {
			int status {-1};
			rusage usage {0};

			if (_id == wait4(_id, &status, options, &usage)) {
				_status = status;
				_usage = usage;
				return true;
			}
			return false;
		}
	};
} // namespace Framework::SubProcess
