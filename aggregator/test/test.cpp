#include <iostream>
#include <sstream>

using std :: cout;
using std :: endl;
using std :: stringstream;

#include "aggregator_api.h"

int main(int argc, char** argv)
{
	int port = 0;
	
	if(argc <= 1)
		return 1;
	else
	{
		stringstream s(argv[1]);
		s >> port;
	}
	
	InitAggregator(port);

	for(int i = 0; i < 1; i++)
		Process();

	size_t count = 0;
	SPacket* l = GetAllData(&count);

	cout << "Got " << count << endl;

	cout << "First " << l[0].address << endl;

	return 0;
}