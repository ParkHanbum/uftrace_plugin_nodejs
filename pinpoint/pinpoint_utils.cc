#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <sstream>

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/syscall.h>

#ifdef SYS_gettid
int get_tid()
{
	pid_t tid = syscall(SYS_gettid);
	return tid;
}
#else
int get_tid()
{
	return -1;
}
#endif

long int get_time()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()
			).count();
}

std::string get_stime()
{
	return std::to_string(get_time());
}

std::string get_stid()
{
	return std::to_string(get_tid());
}

int get_pid()
{
	return getpid();
}

std::string get_spid()
{
	return std::to_string(get_pid());
}
