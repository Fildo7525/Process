#include "MemoryDelegator.hpp"

#include "../debug.hpp"

#include <cstdint>
#include <cstdio>
#include <sstream>
#include <unistd.h>

enum class PipeSides
: uint8_t
{
	parent,
	child,
	none
};

// TODO: Add init() and deinit()
template <typename T>
class Pipe
	: public MemoryDelegator<T>
{
public:
	typedef uint8_t pipe_end;

	/**
	 * @brief Constructor
	 */
	Pipe();

	/**
	 * @brief Closes the selected side of the pipe.
	 *
	 * @param endpoint Side of a pipe.
	 */
	void closeSide(PipeSides endpoint);

	void init() override;

	/**
	 * @brief Function for sharing data between processes.
	 *
	 * @tparam T Type of data to be send.
	 * @param toSend The sending data.
	 */
	void send(const T *toSend) override;

	/**
	 * @brief Reading data from pipe from other process.
	 *
	 * @tparam T Type of send data. Should be the same as the send data.
	 * @return Returns the recieved data.
	 */
	T read() override;

	void deinit() override;
	/**
	 * @brief Destrucotr closes the opened side of the pipe.
	 */
	~Pipe();

private:
	int m_fd[2];
	PipeSides m_opened;
};

template <typename T>
inline Pipe<T>::Pipe()
: m_opened(PipeSides::none)
{
	if (pipe(m_fd) < 0) {
		ELOG("Pipe creation failed");
	}
}

template <typename T>
inline void Pipe<T>::closeSide(PipeSides endpoint)
{
	ILOG("Closing one side of the pipe");
	close(static_cast<int>(m_opened));
	ILOG(static_cast<int>(m_opened) + '0')
	m_opened = (endpoint == PipeSides::child ? PipeSides::parent : PipeSides::child);
}

// TODO: Implementation of the init and deinit funcitons -> some input method
template <typename T>
inline void Pipe<T>::init()
{
	ILOG("");
	ILOG("PipeSide created");
	closeSide(PipeSides::child);
}

template <typename T>
void Pipe<T>::send(const T *toSend)
{
	write(m_fd[static_cast<int>(m_opened)], reinterpret_cast<const T*>(&toSend), sizeof(toSend));
}

template <typename T>
T Pipe<T>::read()
{
	T buff = {0,0};
	//std::cout << "BUFF size: " << sizeof(buff) << std::endl;
	::read(m_fd[static_cast<int>(m_opened)], reinterpret_cast<T*>(&buff), sizeof(buff));
	return buff;
}

template <typename T>
inline void Pipe<T>::deinit()
{
	closeSide(m_opened);
}

template <typename T>
inline Pipe<T>::~Pipe()
{
	close(static_cast<int>(m_opened));
}

