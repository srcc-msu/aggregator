all : bin/ruby_aggregator.so
		
bin/ruby_aggregator.so : src/ruby_bundle.c src/aggregator_c_api.h
	./.make.sh

clean :
	rm -f bin/*
	rm -f src/*.o
	rm -f src/Makefile
	rm -f src/mkmf.log
