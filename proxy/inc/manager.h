#ifndef MANAGER_H
#define MANAGER_H

#include <unordered_map>
#include <cstdio>

#include "duplicator.h"
#include "fd_writer.h"
#include "utils.h"

using namespace std;

const int MAX_QUEUE = 16;
const int MEM_CHUNK = 128;

class CProxyManager
{
private:
    int agg_id;
    unordered_map<uint16_t, string> id_to_name;

    CDuplicator<const SPacket*> duplicator;

/**
   service function, to be run in separate thread
   may block if pipe is full
*/
    void BackgroundStreamHelper(shared_ptr<CFdWriter> writer, size_t subscriber_id);

/**
	creates separate thread for every new subscriber
*/
    void BackgroundStream(shared_ptr<CFdWriter> writer, size_t subscriber_id);

/**
	service function, to be run in separe thread, only one call is allowed
	call dispatch forever
*/
    void BackgroundDispatchHelper();

/**
	read config from JSON /config_fname
	JSON is temporary
	TODO: move to YAML, check that all fields are configured
*/
    void Config(const string& config_fname);

public:
/**
	register binary stream
*/
    void AddBinaryStream(shared_ptr<CSocket> socket)
    {
        BackgroundStream(make_shared<CBinaryFdWriter>(socket, MEM_CHUNK)
            , duplicator.Subscribe());
    }

/**
	register json stream NIY
*/
    void AddJsonStream(shared_ptr<CSocket> socket)
    {
        BackgroundStream(make_shared<CJsonFdWriter>(socket), duplicator.Subscribe());
    }

/**
	start background aggregator processing
*/
    void BackgroundProcess()
    {
        ::BackgroundProcess(agg_id);
    }

/**
	get one packet from aggregator and send it to duplicator
*/
    int Dispatch();

/**
	starts background dispatch, call \BackgroundDispatchHelper in separate thread
*/
    void BackgroundDispatch();

    CProxyManager(string config_fname):
       duplicator(MAX_QUEUE)
    {
        Config(config_fname);
    }
};

#endif
