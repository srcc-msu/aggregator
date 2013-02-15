#ifndef PACKET_H
#define PACKET_H

#include <sys/types.h>
#include <cmath>
#include "sensor_metainf.h"

#define MAX_LEN 40 // TODO check or change it

/**
    union for ipv4 addresses
*/
union UIP
{
    uint32_t b4[1];
    uint8_t b1[4];

    UIP(uint32_t _b4 = 0)
    {
        b4[0] = _b4;
    }
};

/**
    not type safe... hope it will be used correctly :)
*/
#define SENS_UID(sensor_id, sensor_num) ((sensor_id) << 16 | (sensor_num))


/**
    union for easier parsing different types
*/
union UValue
{
    uint64_t b8[1];
    uint32_t b4[2];
    uint16_t b2[4];
    uint8_t b1[8];

    float  f4[2];
    double f8[1];

    UValue()
    {
        b8[0] = 0;
    }
};

#define CURRENT_VERSION 1

enum class E_VAL_TYPE : uint8_t
{
    uint8,
    uint16,
    uint32,
    uint64,
    int8,
    int16,
    int32,
    int64,
    float32,
    float64,
    none
};

/**
    Sensor value, that will be send to upper level
    protocol version 1
*/
struct SPacket
{
    UValue value;
    E_VAL_TYPE type;

    double speed; // ??? TODO: make separate sensor?

    UIP address; // IPv4

    uint32_t server_timestamp;
    uint32_t server_usec;

    uint32_t agent_timestamp;
    uint32_t agent_usec;

    uint16_t sensor_id;
    uint8_t sensor_num;

    uint8_t version;

    SPacket():
        type(E_VAL_TYPE :: none),
        speed(0),
        server_timestamp(0),
        server_usec(0),
        agent_timestamp(0),
        agent_usec(0),
        sensor_id(0),
        sensor_num(0),
        version(CURRENT_VERSION)
    {}

};

/**
    parse UValue from \buffer according to msg_length
    converts network byte order to normal
*/

UValue ParseUValue(unsigned char* buffer, size_t msg_length);

/**
    returns a type of \sensor_id with given msg_length
    uses sensor_metainf to lookup
*/
E_VAL_TYPE GetType(uint16_t sensor_id, e_sens_type type, size_t msg_length);

/**
    calculates and fills all fields in \packet
    that should be a default method to create a packet
*/
void CreatePacket(SPacket& packet, unsigned char* buffer
    , uint16_t sensor_id, uint8_t sensor_num
    , UIP address
    , uint32_t server_timestamp, uint32_t server_usec
    , uint32_t agent_timestamp, uint32_t agent_usec);

/**
    self explaining
*/
double GetDiv(const UValue& v1, const UValue& v2, E_VAL_TYPE type);

/**
    self explaining
*/
double GetDiff(const UValue& v1, const UValue& v2, E_VAL_TYPE type);

/**
    self explaining
*/
UValue GetSum(const UValue& v1, const UValue& v2, E_VAL_TYPE type);

/**
    self explaining
*/
UValue MultValue(const UValue& value, E_VAL_TYPE type, double mult);

#endif