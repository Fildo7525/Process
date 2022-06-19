//
// Created by Filip Lobpreis on 06/19/2022.
//
#pragma once

#ifndef __gnu_linux__
#error "The software is defined for unix/linux type systems"
#endif

#if __cplusplus < 201402L
#error "The cpp standard must be c++17 and higher"
#endif

#include "ipc/pipe.hpp"

#include <functional>
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
class process
{
	/**
	 * @brief Function for sharing general data.
	 *
	 * @tparam sharedData_t Type of shared data.
	 * @param data Data to be send to other process.
	 */
	template<typename sharedData_t> void
	share_data(sharedData_t data);

public:
	/// Constructor
	explicit process();

	/**
	 * @brief Constructor that immediatelly runs the inputed callable in other process with supplied parameters.
	 *
	 * @tparam Callable Type of the callable.
	 * @tparam Child_args Type of child arguments.
	 * @param childProcessFunction Callable passed to child process.
	 * @param functionParameters Arguments to be passed to child_proc_fun.
	 */
	template <typename Callable, typename... Child_args>
	process(Callable&& childProccessFunction, Child_args&&... functionParameters);

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
	template <typename _ret> _ret
	readChildMemory();

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
	pid_t getChildPid();

	/// Desturctor
	~process();

private:
	// TODO: m_pipe will be some interface like IPCMemoryDelegator
	/// Pipe for sharing data by IPC
	Pipe m_pipe;

	/// Child process ID
	pid_t m_childPid;
};

// TODO: send will be overriden function
template<typename T> void
process::share_data(T data)
{
	m_pipe.send(data);
}

inline process::process()
: m_pipe()
{
}

template <typename T, typename... Child_arg>
process::process(T&& child_proc_fun, Child_arg&&... child_param)
: m_pipe()
{
	run(child_proc_fun, child_param...);
}

template <typename T, typename... Child_arg>
void process::run(T&& child_proc_fun, Child_arg&&... child_param)
{
	if((m_childPid = fork()) < 0)
		throw std::runtime_error("Fork terminated with error");
	else if( m_childPid == 0) {	// indication of child process
		m_pipe.closeSide(PipeSides::parent);
		share_data(std::invoke(std::forward<T>(child_proc_fun), std::forward<Child_arg>(child_param)...));
		exit(0);	// exit state of 0
	}
	m_pipe.closeSide(PipeSides::child);
	// exit(0);
}

// TODO: m_pipe should be done by some interface => read will be overriden function
template <typename _ret> _ret
process::readChildMemory()
{
	return m_pipe.read<_ret>();
}

inline void process::changeProcess(const std::string& path, const std::string& cmd)
{
	this->run(execl, path.c_str(),cmd.c_str(),NULL);
}

inline pid_t process::getChildPid()
{
	return m_childPid;
}

inline process::~process()
{
	m_pipe.~Pipe();
}

