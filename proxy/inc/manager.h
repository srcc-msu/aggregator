#ifndef MANAGER_H
#define MANAGER_H

#include <unordered_map>
#include <memory>
#include <cstdio>

#include "aggregator.h"

#include "packet.h"

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
		std::string avg_address;
		int avg_port;

		aggregator = ConfigAggregator(config_fname, id_to_name, avg_address, avg_port);
		aggregator->BackgroundProcess();

		BackgroundDispatch();
		InitAvgStream(avg_address, avg_port);
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
			std::vector<SPacket> packets;
			aggregator->QueueAggregator().GetData(packets);

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


#define MAX_MESSAGE 128

	void AvgStreamHelper(const std::string& address, int port)
	{
		CDynSleeper sleeper;

		while(1)
		{
			shared_ptr<CSocket> socket = make_shared<CDGRAMSocket>(address, port);

			std::vector<NodeEntry> packets;

			aggregator->GetAvgData(packets);

			sleeper.Sleep(packets.size() == 0); // sleep more

			for(NodeEntry& entry : packets)
			{
				char msg[MAX_MESSAGE];
				snprintf(msg, MAX_MESSAGE, "%u.%u.%u.%u;%u;%u;%s;%s;%s;%.3f;%.3f;%.3f"
					, entry.ip.b1[0], entry.ip.b1[1], entry.ip.b1[2], entry.ip.b1[3]
					, entry.sensor, entry.timer
					, UValueToString(entry.min, entry.type)
					, UValueToString(entry.max, entry.type)
					, UValueToString(MultValue(entry.sum, entry.type, 1.0 / entry.count), entry.type)
					, entry.min_speed
					, entry.max_speed
					, entry.sum_speed / entry.count);

				socket->Write(msg, strlen(msg));
			}
		}
	}

	void InitAvgStream(const std::string& avg_address, int avg_port)
	{
		static bool started = false;

		if(started == true)
			return;

		started = true;

		std::thread thread(&CProxyManager :: AvgStreamHelper, this, avg_address, avg_port);

		thread.detach();
	}

};

#endif
