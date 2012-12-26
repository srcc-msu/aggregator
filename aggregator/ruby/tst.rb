#! /usr/bin/env ruby

require_relative 'ruby_aggregator'

agg = Aggregator.new

# Init aggregator
agg.InitAggregator("127.0.0.1")


# Configure agent to send data
agg.InitAgent("127.0.0.1")

$i = 1100
$num = 3000

while $i < $num  do
	agg.QueueBlacklistId($i)
	$i += 1
end

agg.QueueBlacklistId(1052)
agg.SetDeltaA("127.0.0.1", 1.3)

# his sensor will not be connected

# Messages from this address will be ignored
agg.GlobalBlacklistAddress("123.255.255.255")

# Collect all data
t1 = Thread.new do
	while true
		agg.Process()
	end
end


# Get collected data
t2 = Thread.new do
	while true
		agg.GetAllData().each do |num|
			num.each do |field|
		    	print field, " "
		    end
		  	puts
		end
	end
end



t1.join
t2.join

puts "end"
