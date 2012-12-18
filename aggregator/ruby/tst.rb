#! /usr/bin/env ruby

require 'ruby_aggregator'

agg = Aggregator.new

# Init aggregator
agg.InitAggregator("127.0.0.1")


# Configure agent to send data
agg.InitAgent("127.0.0.1")

# his sensor will not be connected
agg.BlacklistId(2162)

# Messages from this address will be ignored
agg.BlacklistAddress("123.255.255.255")

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