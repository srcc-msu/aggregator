all : aggregator proxy csv_writer

aggregator : 
	$(MAKE) -C aggregator

proxy :
	$(MAKE) -C proxy

csv_writer : 
	$(MAKE) -C csv_writer

clean :
	$(MAKE) -C aggregator_ruby_bundle clean
	$(MAKE) -C aggregator clean
	$(MAKE) -C hoplang_proxy clean
