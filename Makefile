all: aggregator proxy csv_writer hr_dumper jsoncpp

aggregator:
	$(MAKE) -C aggregator

proxy:
	$(MAKE) -C proxy

hr_dumper:
	$(MAKE) -C hr_dumper

csv_writer:
	$(MAKE) -C csv_writer

jsoncpp:
	$(MAKE) -C side_libs

.PHONY: clean aggregator proxy csv_writer hr_dumper jsoncpp

clean :
	$(MAKE) -C aggregator clean
	$(MAKE) -C proxy clean
	$(MAKE) -C csv_writer clean
	$(MAKE) -C hr_dumper clean
