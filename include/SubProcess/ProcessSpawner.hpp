#pragma once

#include <unistd.h>
#include <functional>

namespace Framework::SubProcess {

	class ProcessSpawner {
	public:
		ProcessSpawner() = delete;
		~ProcessSpawner() = delete;

		template <typename T>
		static pid_t Spawn(int (*function)(T*), T *argument) {
			pid_t pid = fork();
			if (pid == 0) {
				int ret = function(argument);
				_exit(ret);
			}
			return pid;
		}
	};
} // namespace Framework::SubProcess
