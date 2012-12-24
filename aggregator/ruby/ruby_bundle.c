#include <ruby.h>

#include "aggregator_c_api.h"

#include "stdio.h"

VALUE agg_class;

VALUE rb_InitAggregator(VALUE self, VALUE rb_address)
{
	char* address = STR2CSTR(rb_address);

	printf("init aggregator\n");

	rb_ivar_set(self, rb_intern("agg_id"), INT2NUM(InitAggregator(address)));
}

VALUE rb_InitAgent(VALUE self, VALUE rb_address)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	char* address = STR2CSTR(rb_address);

	InitAgent(agg_id, address);
}

VALUE rb_Process(VALUE self)
{	
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));

	Process(agg_id);

	return INT2NUM(0);
}

VALUE rb_GetAllData(VALUE self)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	
	size_t count = 0;

	struct SPacket* packets = GetAllData(agg_id, &count);

	VALUE arr = rb_ary_new2(count);

	int i = 0;
	for(i = 0; i < count; i++)
	{
		VALUE sub_arr = rb_ary_new2(6);
		
		rb_ary_store(sub_arr, 0, UINT2NUM(packets[i].address));
		rb_ary_store(sub_arr, 1, UINT2NUM(packets[i].agent_timestamp));
		rb_ary_store(sub_arr, 2, UINT2NUM(packets[i].agent_usec));
		rb_ary_store(sub_arr, 3, UINT2NUM(packets[i].server_timestamp));
		rb_ary_store(sub_arr, 4, UINT2NUM(packets[i].server_usec));
		rb_ary_store(sub_arr, 5, UINT2NUM(packets[i].sensor_id));
		rb_ary_store(sub_arr, 6, rb_str_new2(packets[i].data_string));
		rb_ary_store(arr, i, sub_arr);
	}

	return arr;
}

VALUE rb_GetInterval(VALUE self, VALUE rb_address, VALUE rb_sensor_id, VALUE rb_seconds)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));

	char* address = STR2CSTR(rb_address);
	int sensor_id = FIX2INT(rb_sensor_id);
	int seconds = FIX2INT(rb_seconds);

	struct SPacket* packets = GetInterval(agg_id, address, sensor_id, seconds);

	VALUE arr = rb_ary_new2(seconds);

	if(packets == NULL)
		return arr;

	int i = 0;
	for(i = 0; i < seconds; i++)
	{
		VALUE sub_arr = rb_ary_new2(6);
		
		rb_ary_store(sub_arr, 0, UINT2NUM(packets[i].address));
		rb_ary_store(sub_arr, 1, UINT2NUM(packets[i].agent_timestamp));
		rb_ary_store(sub_arr, 2, UINT2NUM(packets[i].agent_usec));
		rb_ary_store(sub_arr, 3, UINT2NUM(packets[i].server_timestamp));
		rb_ary_store(sub_arr, 4, UINT2NUM(packets[i].server_usec));
		rb_ary_store(sub_arr, 5, UINT2NUM(packets[i].sensor_id));
		rb_ary_store(sub_arr, 6, rb_str_new2(packets[i].data_string));
		rb_ary_store(arr, i, sub_arr);
	}

	return arr;
}

VALUE rb_GlobalBlacklistAddress(VALUE self, VALUE rb_address)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	char* address = STR2CSTR(rb_address);

	GlobalBlacklistAddress(agg_id, address);
}

// -----------

VALUE rb_QueueBlacklistId(VALUE self, VALUE rb_sensor_id)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	uint16_t sensor_id = FIX2INT(rb_sensor_id);

	QueueBlacklistId(agg_id, sensor_id);
}

VALUE rb_QueueUnblacklistId(VALUE self, VALUE rb_sensor_id)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	uint16_t sensor_id = FIX2INT(rb_sensor_id);

	QueueUnblacklistId(agg_id, sensor_id);
}

// ------

VALUE rb_BufferAllowId(VALUE self, VALUE rb_sensor_id)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	uint16_t sensor_id = FIX2INT(rb_sensor_id);

	BufferAllowId(agg_id, sensor_id);
}

VALUE rb_BufferDisallowId(VALUE self, VALUE rb_sensor_id)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	uint16_t sensor_id = FIX2INT(rb_sensor_id);

	BufferDisallowId(agg_id, sensor_id);
}

// -------

VALUE rb_SetDeltaAS(VALUE self, VALUE rb_address, VALUE rb_sensor_id, VALUE rb_delta)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	
	char* address = STR2CSTR(rb_address);
	uint16_t sensor_id = FIX2INT(rb_sensor_id);
	double delta = NUM2DBL(rb_delta);

	SetDeltaAS(agg_id, address, sensor_id, delta);
}


VALUE rb_SetDeltaA(VALUE self, VALUE rb_address, VALUE rb_delta)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	
	char* address = STR2CSTR(rb_address);
	double delta = NUM2DBL(rb_delta);

	SetDeltaA(agg_id, address, delta);
}

VALUE rb_SetDeltaS(VALUE self, VALUE rb_sensor_id, VALUE rb_delta)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	
	uint16_t sensor_id = FIX2INT(rb_sensor_id);
	double delta = NUM2DBL(rb_delta);

	SetDeltaS(agg_id, sensor_id, delta);
}

VALUE rb_SetDelta(VALUE self, VALUE rb_delta)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	
	double delta = NUM2DBL(rb_delta);

	SetDelta(agg_id, delta);
}

// -------

VALUE rb_SetIntervalAS(VALUE self, VALUE rb_address, VALUE rb_sensor_id, VALUE rb_interval)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	
	char* address = STR2CSTR(rb_address);
	uint16_t sensor_id = FIX2INT(rb_sensor_id);
	int interval = FIX2INT(rb_interval);

	SetIntervalAS(agg_id, address, sensor_id, interval);
}


VALUE rb_SetIntervalA(VALUE self, VALUE rb_address, VALUE rb_interval)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	
	char* address = STR2CSTR(rb_address);
	int interval = FIX2INT(rb_interval);

	SetIntervalA(agg_id, address, interval);
}

VALUE rb_SetIntervalS(VALUE self, VALUE rb_sensor_id, VALUE rb_interval)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	
	uint16_t sensor_id = FIX2INT(rb_sensor_id);
	int interval = FIX2INT(rb_interval);

	SetIntervalS(agg_id, sensor_id, interval);
}

VALUE rb_SetInterval(VALUE self, VALUE rb_interval)
{
	int agg_id = FIX2INT(rb_ivar_get(self, rb_intern("agg_id")));
	
	int interval = FIX2INT(rb_interval);

	SetInterval(agg_id, interval);
}

// -------

void Init_ruby_aggregator()
{
	agg_class = rb_define_class("Aggregator", rb_cObject);

	rb_define_method(agg_class, "InitAggregator", rb_InitAggregator, 1);
	rb_define_method(agg_class, "InitAgent", rb_InitAgent, 1);
	rb_define_method(agg_class, "Process", rb_Process, 0);
	rb_define_method(agg_class, "GetAllData", rb_GetAllData, 0);
	rb_define_method(agg_class, "GetInterval", rb_GetInterval, 3);
	
	rb_define_method(agg_class, "GlobalBlacklistAddress", rb_GlobalBlacklistAddress, 1);

	rb_define_method(agg_class, "QueueBlacklistId"  , rb_QueueBlacklistId, 1);
	rb_define_method(agg_class, "QueueUnblacklistId", rb_QueueUnblacklistId, 1);

	rb_define_method(agg_class, "BufferAllowId"   , rb_BufferAllowId, 1);
	rb_define_method(agg_class, "BufferDisallowId", rb_BufferDisallowId, 1);

	rb_define_method(agg_class, "SetIntervalAS", rb_SetInterval, 3);
	rb_define_method(agg_class, "SetIntervalA", rb_SetIntervalA, 2);
	rb_define_method(agg_class, "SetIntervalS", rb_SetIntervalS, 2);
	rb_define_method(agg_class, "SetInterval", rb_SetInterval, 1);

	rb_define_method(agg_class, "SetDeltaAS", rb_SetDeltaAS, 3);
	rb_define_method(agg_class, "SetDeltaA", rb_SetDeltaA, 2);
	rb_define_method(agg_class, "SetDeltaS", rb_SetDeltaS, 2);
	rb_define_method(agg_class, "SetDelta", rb_SetDelta, 1);

	printf("aggregator loaded\n");
}