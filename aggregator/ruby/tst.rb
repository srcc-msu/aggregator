#! /usr/bin/env ruby

require 'ruby_aggregator'

puts InitAggregator(33826)

Process()
Process()

GetAllData().each do |num|
	num.each do |field|
    	puts field
    end
end

puts "end"