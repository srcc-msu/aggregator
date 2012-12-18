#include <iostream>
#include <sstream>

using std :: cout;
using std :: endl;
using std :: stringstream;

#include "aggregator_api.h"
#include "connection.h"

int main(int argc, char** argv)
{
	int agg_id = InitAggregator("192.168.254.248");

	InitAgent(agg_id, "192.168.254.248");

	for(int i = 0; i < 5; i++)
		Process(agg_id);

	size_t count = 0;
	SPacket* l = GetAllData(agg_id, &count);

	cout << "Got " << count << endl;

	cout << "First " << l[0].address << endl;

	l = GetInterval(agg_id, "192.168.254.248", 2061, 1);

	printf("%x\n", (int)l);
	
	if(l != nullptr)
		for(int i = 0; i < 1; i++)
			cout << i << " " << l[i].address << endl;

	return 0;
}

