template <typename T>
class MemoryDelegator
{
public:
	virtual void init(void *args) = 0;
	virtual void send(const T *toSend) = 0;
	virtual T read() = 0;
	virtual void deinit(void *args) = 0;
};

