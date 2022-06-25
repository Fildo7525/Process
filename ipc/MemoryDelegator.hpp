template <typename T>
class MemoryDelegator
{
public:
	virtual void init() = 0;
	virtual void send(const T *toSend) = 0;
	virtual T read() = 0;
	virtual void deinit() = 0;
};

