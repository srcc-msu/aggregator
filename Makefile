
all : agg bundle proxy

agg : 
	$(MAKE) -C aggregator

bundle : agg
	$(MAKE) -C aggregator_ruby_bundle

proxy : bundle
	$(MAKE) -C hoplang_proxy

clean :
	$(MAKE) -C aggregator_ruby_bundle clean
	$(MAKE) -C aggregator clean
	$(MAKE) -C hoplang_proxy clean
