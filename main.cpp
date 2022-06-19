#include "process.hpp"
#include <iostream>
#include <unistd.h>

struct testData
{
	int i;
	short s;
	long l;
	char c;
	const char *cstr;

	// WARNING: str will not be send in good form
	// WARNING: will run but with error due to end of scope destruction
	// TIP: use char * instead
	std::string str;
};

testData testFunction(int i, short s, long l, char c, const char *cstr, std::string str);

int main(int argc, char *argv[])
{
	process p;
	std::cout << "Hello World!\n";
	p.run(testFunction, 10, 3, 50, 'f', "cstr", std::string("str"));

	sleep(2);

	auto returnedData = p.readChildMemory<testData>();
	printf("Returned data are:\n\t%d\n\t%d\n\t%ld\n\t%c\n\t%s\n\t%s\n",
		returnedData.i,
		returnedData.s,
		returnedData.l,
		returnedData.c,
		returnedData.cstr,
		returnedData.str.c_str()
	);

	return 0;
}

testData testFunction(int i, short s, long l, char c, const char *cstr, std::string str)
{
	testData data = {i, s, l, c, cstr, str};
	printf("Test data are returned from other process with values:\n\t%d\n\t%d\n\t%ld\n\t%c\n\t%s\n\t%s\n",
		data.i,
		data.s,
		data.l,
		data.c,
		data.cstr,
		data.str.c_str()
	);
	return data;
}

