#ifndef MANAGER_H
#define MANAGER_H

#include <unordered_map>
#include <memory>
#include <cstdio>

#include "aggregator.h"

#include "config.h"
#include "duplicator.h"
#include "fd_writer.h"
#include "utils.h"

using namespace std;

const int MAX_QUEUE = 16;
const int MEM_CHUNK = 128;

class CProxyManager
{
private:
	std::shared_ptr<CAggregator> aggregator;

	unordered_map<uint16_t, string> id_to_name;

	CDuplicator duplicator;

public:
	CProxyManager(const string& config_fname):
	   duplicator(MAX_QUEUE)
	{
		aggregator = ConfigAggregator(config_fname, id_to_name);
		aggregator->BackgroundProcess();

		BackgroundDispatch();
	}

	void DeleteSubscriber(size_t uid)
	{
		duplicator.DeleteSubscriber(uid);
	}

	size_t AddBinaryStream(shared_ptr<CSocket> socket)
	{
		return duplicator.Subscribe(make_shared<CBinaryFdWriter>(socket));
	}

private:
	void BackgroundDispatchHelper()
	{
		CDynSleeper sleeper;

		while(1)
		{
			std::vector<SPacket> packets = aggregator->QueueAggregator().GetData();

			sleeper.Sleep(packets.size() == 0); // sleep more

			duplicator.Add(packets); 
		}
	}

	void BackgroundDispatch()
	{
		static bool started = false;

		if(started == true)
			return;

		started = true;

		std::thread thread(&CProxyManager :: BackgroundDispatchHelper, this);

		thread.detach();
	}
};

#endif
