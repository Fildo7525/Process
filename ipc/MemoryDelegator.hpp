class MemoryDelegator
{
public:
	virtual void init(void *args) = 0;
	virtual void send(const void *toSend) = 0;
	virtual void *read() = 0;
	virtual void deinit(void *args) = 0;

	virtual ~MemoryDelegator();
};

