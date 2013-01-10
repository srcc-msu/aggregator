cp ../aggregator/bin/libaggregator.so src/

cd src
./extconf.rb
make
cd -

rm src/libaggregator.so
mv src/ruby_aggregator.so bin/
