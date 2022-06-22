//
// Created by Filip Lobpreis on 06/19/2022.
//
#pragma once

#include <unistd.h>
#ifndef __gnu_linux__
#error "The software is defined for unix/linux type systems"
#endif

#if __cplusplus < 201402L
#error "The cpp standard must be c++17 and higher"
#endif

#include "ipc/pipe.hpp"

#include "debug.hpp"

#include <functional>
#include <memory>
#include <cstring>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

/**
 * @class process
 * @brief Class represents a child process and its functionality
 * Usage:
 * 	- if you want to allocate memory outside of processes
 * 	  use constructor with a number
 * 	- to run a separate process use function process::run(function, args)
 * 	- to retrieve returned arguments use function  process::read_child_memory<type_to_read>()
 * 	  this function returns a std::pair<type_to_return, int>
 * 	  - type_to_return - user input
 * 	  - int - child process exit state
 * 	- process::change_process() runs execl with third argument NULL
 */
template<typename T>
class process
{
	/**
	 * @brief Function for sharing general data.
	 *
	 * @tparam sharedData_t Type of shared data.
	 * @param data Data to be send to other process.
	 */
	void share_data(const T data);

public:
	/// Constructor
	explicit process(std::shared_ptr<MemoryDelegator<T>> memoryDelegator = std::make_shared<Pipe<T>>());

	/**
	 * @brief Constructor that immediatelly runs the inputed callable in other process with supplied parameters.
	 *
	 * @tparam Callable Type of the callable.
	 * @tparam Child_args Type of child arguments.
	 * @param childProcessFunction Callable passed to child process.
	 * @param functionParameters Arguments to be passed to child_proc_fun.
	 */
	template <typename Callable, typename... Child_args>
	process(Callable&& childProccessFunction,
		 	Child_args&&... functionParameters,
			std::shared_ptr<MemoryDelegator<T>> memoryDelegator = std::make_shared<Pipe<T>>());

	/**
	 * @brief Function supplying the callable to child process.
	 *
	 * @tparam Callable Type of the callable.
	 * @tparam Child_args Type of child arguments.
	 * @param childProccessFunction Callable passed to child process.
	 * @param functionParameters Arguments to be passed to child_proc_fun.
	 */
	template <typename Callable, typename... Child_args>
	void run(Callable&& childProccessFunction, Child_args&&... functionParameters);

	/**
	 * @brief Function for reading shared data from child process.
	 *
	 * @tparam _ret Return type of the desired output.
	 * @return Shared data, result of IPC.
	 */
	T readChildMemory();

	/**
	 * @brief Change the process execution to specific executable.
	 *
	 * @param path Path to executable.
	 * @param cmd Command of the executable.
	 */
	void changeProcess(const std::string& path, const std::string& cmd);

	/**
	 * @brief Child process ID.
	 */
	pid_t childPid();

	/**
	 * @brief Process ID.
	 *
	 * @return Returns current process id.
	 */
	pid_t pid();

	/// Desturctor
	~process();

private:
	std::shared_ptr<MemoryDelegator<T>> m_memoryDelegator;
	/// Child process ID
	pid_t m_childPid;
};

template<typename T>
void process<T>::share_data(const T data)
{
	m_memoryDelegator->send(&data);
}

template<typename T>
inline process<T>::process(std::shared_ptr<MemoryDelegator<T>> memoryDelegator)
: m_memoryDelegator(memoryDelegator)
{
}

template<typename T>
template <typename Callable, typename... Child_args>
process<T>::process(Callable&& childProcessFunction, Child_args&&... functionParameters, std::shared_ptr<MemoryDelegator<T>> memoryDelegator)
: m_memoryDelegator(memoryDelegator)
{
	run(childProcessFunction, functionParameters...);
}

template<typename T>
template <typename Callable, typename... Child_args>
void process<T>::run(Callable&& childProcessFunction, Child_args&&... functionParameters)
{
	if((m_childPid = fork()) < 0) {
		ELOG("Error while calling fork, quitting the program");
		throw std::runtime_error("Fork terminated with error");
	}
	else if( m_childPid == 0) {	// indication of child process
		std::string msg = "Starting execution of child process number ";
		msg += std::to_string(pid());
		ILOG(msg.c_str());
		m_memoryDelegator->init();
		share_data(std::invoke(std::forward<Callable>(childProcessFunction), std::forward<Child_args>(functionParameters)...));
		exit(0);	// exit state of 0
	}
	m_memoryDelegator->deinit();

	std::string msg = "Parent process continues. Process number ";
	msg += std::to_string(pid());
	ILOG(msg.c_str());
}

template<typename T>
T process<T>::readChildMemory()
{
	return static_cast<T>(m_memoryDelegator->read());
}

template<typename T>
inline void process<T>::changeProcess(const std::string& path, const std::string& cmd)
{
	this->run(execl, path.c_str(), cmd.c_str(), NULL);
}

template<typename T>
inline pid_t process<T>::childPid()
{
	return m_childPid;
}

template<typename T>
inline pid_t process<T>::pid()
{
	return getpid();
}

template<typename T>
inline process<T>::~process()
{
}

