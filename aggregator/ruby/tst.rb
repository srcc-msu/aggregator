#! /usr/bin/env ruby

require 'ruby_aggregator'

InitAggregator(33826)

t1 = Thread.new do
	while true
		Process()
	end
end

t2 = Thread.new do
	while true
		GetAllData().each do |num|
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