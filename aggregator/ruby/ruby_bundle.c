#include <ruby.h>

#include "aggregator_c_api.h"

#include "stdio.h"

VALUE rb_InitAggregator(VALUE self, VALUE rb_port)
{
	int port = FIX2INT(rb_port);

	printf("init aggregator on port %d\n", port);

	InitAggregator(port);

	return INT2NUM(0);
}

VALUE rb_Process(VALUE self)
{	
	Process();

	return INT2NUM(0);
}

VALUE rb_GetAllData(VALUE self)
{
	
	size_t count = 0;

	struct SPacket* packets = GetAllData(&count);

	printf("count %d\n", count);

	VALUE arr = rb_ary_new2(count);

	int i = 0;
	for(i = 0; i < count; i++)
	{
		VALUE sub_arr = rb_ary_new2(6);
		
		rb_ary_store(sub_arr, 0, UINT2NUM(packets[i.address));
		rb_ary_store(sub_arr, 1, UINT2NUM(packets[i].agent_timestamp));
		rb_ary_store(sub_arr, 2, UINT2NUM(packets[i].agent_usec));
		rb_ary_store(sub_arr, 1, UINT2NUM(packets[i].server_timestamp));
		rb_ary_store(sub_arr, 2, UINT2NUM(packets[i].server_usec));
		rb_ary_store(sub_arr, 3, UINT2NUM(packets[i].sensor_id));
		rb_ary_store(sub_arr, 4, rb_str_new(packets[i].data_string, MAX_LEN));
		rb_ary_store(arr, i, sub_arr);
	}

	return arr;
}



void Init_ruby_aggregator()
{
	rb_define_method(rb_cObject, "InitAggregator", rb_InitAggregator, 1);
	rb_define_method(rb_cObject, "Process", rb_Process, 0);
	rb_define_method(rb_cObject, "GetAllData", rb_GetAllData, 0);

	printf("aggregator loaded\n");
}