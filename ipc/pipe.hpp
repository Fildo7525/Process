#include <cstdint>
#include <unistd.h>

enum class PipeSides : uint8_t {
	parent,
	child,
	none
};

class Pipe{
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

	/**
	 * @brief Function for sharing data between processes.
	 *
	 * @tparam T Type of data to be send.
	 * @param toSend The sending data.
	 */
	template <typename T>
	void send(const T toSend);

	/**
	 * @brief Reading data from pipe from other process.
	 *
	 * @tparam T Type of send data. Should be the same as the send data.
	 * @return Returns the recieved data.
	 */
	template <typename T>
	T read();

	/**
	 * @brief Destrucotr closes the opened side of the pipe.
	 */
	~Pipe();

private:
	int m_fd[2];
	PipeSides m_opened;
};

inline Pipe::Pipe(){
	pipe(m_fd);
	m_opened = PipeSides::none;
}

inline void Pipe::closeSide(PipeSides endpoint) {
	close(static_cast<int>(m_opened));
	m_opened = (endpoint == PipeSides::child ? PipeSides::parent : PipeSides::child);
}

template <typename T>
void Pipe::send(const T toSend) {
	//std::cout << "Sneding:\n\ta: " << toSend.a << "\n\tb: " << toSend.b << std::endl;
	write(m_fd[static_cast<int>(m_opened)], reinterpret_cast<const T*>(&toSend), sizeof(toSend));
}

template <typename T>
T Pipe::read(){
	T buff = {0,0};
	//std::cout << "BUFF size: " << sizeof(buff) << std::endl;
	::read(m_fd[static_cast<int>(m_opened)], reinterpret_cast<T*>(&buff), sizeof(buff));
	return buff;
}

inline Pipe::~Pipe(){
	close(static_cast<int>(m_opened));
}

