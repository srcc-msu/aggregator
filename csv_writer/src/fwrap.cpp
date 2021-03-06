#include "fwrap.h"

#include "packet.h"
#include "error.h"

template <typename T>
void CFWrap<T> :: ReinitQueue()
{
	queue = make_shared<vector<T>>();
	queue->reserve(max_lines+1);
}

//--------------------------------

template <typename T>
void CFWrap<T> :: WriteHelper(shared_ptr<vector<T>> queue_dump)
{
	lock_guard<mutex> lock(CFWrap :: global_write_mutex);

	char fname[PATH_MAX] = "";
	snprintf(fname, PATH_MAX, "%s%u%06u.csv"
		, dir.c_str(), (*queue_dump)[0].server_timestamp
		, (*queue_dump)[0].server_usec);

	FILE* f = fopen(fname, "w");

	if(!f)
	{
		fprintf(stderr, "can not open file: %s\n", fname);

		throw CSyscallException("fopen failed");
	}

	printf("Writing file to disk: %s ... ", fname);

	for(auto& it : *queue_dump)
	{
/*		fprintf(f, "%u%06u;cn%02u;%s;%u;%.3f\n"
			, it.server_timestamp, it.server_usec
			, it.address.b1[3]
			, UValueToString(it.value, it.type)
			, it.sensor_num, it.speed);
*/
		fprintf(f, "%u%06u;node-%02u-%02u;%s;%u;%.3f\n"
			, it.server_timestamp, it.server_usec
			, it.address.b1[2], it.address.b1[3]
			, UValueToString(it.value, it.type)
			, it.sensor_num, it.speed);

	}

	fclose(f);

	printf("done\n");
}

//--------------------------------

template <typename T>
void CFWrap<T> :: Write()
{
	shared_ptr<vector<T>> queue_dump = queue;

	ReinitQueue();

// if disk is busy, number of process will grow.. somethign is wrong
	try
	{
		thread t(&CFWrap :: WriteHelper, this, queue_dump);

		t.detach();
	}
	catch(const std::system_error& e)
	{
		throw CException("disk write overrun");
	}
}

//--------------------------------

template <typename T>
void CFWrap<T> :: AddPacket(const T& p)
{
	queue->push_back(p);

	if(queue->size() >= max_lines)
	{
		printf("%d sizeout %d/%d : \n", p.sensor_id, (int)queue->size()
			, (int)max_lines);

		Write();
	}
	else if(queue->size() > 0
		&& p.server_timestamp - (*queue)[0].server_timestamp > max_time)
	{
		printf("%d timeout %u/%u : \n", p.sensor_id
			, p.server_timestamp - (*queue)[0].server_timestamp, max_time);

		Write();
	}
}

//--------------------------------

template <typename T>
mutex CFWrap<T> :: global_write_mutex;

template class CFWrap<SPacket>;
