#include <map>
#include <utility>

struct NodeEntry
{
	UIP ip;
	uint16_t sensor;
	uint32_t timer;

	size_t count;

	UValue min;
	UValue max;
	UValue sum;

	E_VAL_TYPE type;

	double min_speed;
	double max_speed;
	double sum_speed;


	NodeEntry()
	{}

	NodeEntry(UIP _ip, uint16_t _sensor):
		ip(_ip),
		sensor(_sensor)
	{
		timer = 0;
	}
};

class CAvgAggregator
{
	std::map<uint32_t, std::map<uint16_t, NodeEntry>> data;

	std::vector<NodeEntry> output;

	size_t filter_size;

public:
	void Add(const SPacket& packet)
	{
		if(data.find(packet.address.b4[0]) == data.end())
		{
			data[packet.address.b4[0]] = std::map<uint16_t, NodeEntry>();
		}

		if(data[packet.address.b4[0]].find(packet.sensor_id) == data[packet.address.b4[0]].end())
		{
			data[packet.address.b4[0]][packet.sensor_id] = NodeEntry(packet.address, packet.sensor_id);
		}

		auto entry = data[packet.address.b4[0]][packet.sensor_id];

		if(entry.timer == 0)
		{
			entry.min = packet.value;
			entry.max = packet.value;
			entry.sum = packet.value;

			entry.min_speed = packet.speed;
			entry.max_speed = packet.speed;
			entry.sum_speed = packet.speed;

			entry.type = packet.type;

			entry.timer = packet.server_timestamp / filter_size * filter_size;
		}
		else
		{
			if(GetDiff(packet.value, entry.min, entry.type) < 0) entry.min = packet.value;
			if(GetDiff(packet.value, entry.max, entry.type) > 0) entry.max = packet.value;

			entry.sum = GetSum(entry.sum, packet.value, entry.type);

			if(packet.speed < entry.min_speed) entry.min_speed = packet.speed;
			if(packet.speed > entry.max_speed) entry.max_speed = packet.speed;
			
			entry.sum_speed = entry.sum_speed + packet.speed;
		}

		entry.count++;

		if(packet.server_timestamp - entry.timer > filter_size)
		{
			output.push_back(entry);
			data[packet.address.b4[0]].erase(packet.sensor_id);
		}
	}

	std::vector<NodeEntry> GetAvgData()
	{
		std::vector<NodeEntry> tmp;
		tmp.swap(output);

		return tmp;
	}

	CAvgAggregator(size_t _filter_size):
		filter_size(_filter_size)
	{}
};
