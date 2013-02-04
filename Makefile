all: aggregator proxy csv_writer

aggregator:
	$(MAKE) -C aggregator

proxy:
	$(MAKE) -C proxy

csv_writer:
	$(MAKE) -C csv_writer

jsoncpp:
	$(MAKE) -C side_libs

.PHONY: clean aggregator proxy csv_writer jsoncpp

clean :
	$(MAKE) -C aggregator clean
	$(MAKE) -C proxy clean
	$(MAKE) -C csv_writer clean
