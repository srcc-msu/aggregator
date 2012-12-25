#!/usr/bin/env ruby

require 'rubygems'
require 'socket'
require 'avro'
#require 'tempfile'
require 'e'
#!require 'simple_uuid'
require 'thread'
require 'logger'

require 'yaml'

$logger = Logger.new('_hoplang_proxy.log', 10, 1024000)

require 'ruby_aggregator'

$agg = Aggregator.new

def Init
  config = YAML.load(File.open('./aggregator.conf', "r"))
  
  puts config.inspect
  
  $agg.InitAggregator(config["server"]["address"])

  config["agents"]["list"].each do |address|
    $agg.InitAgent(address[1])
  end

  $agg.SetDelta(config["server"]["queue"]["delta"])
  $agg.SetInterval(config["server"]["queue"]["interval"])

  Thread.new do
    while true
      $agg.Process()
      sleep(0.01)
    end
  end
end

Init()

class HoplangProxy <E
  charset 'utf-8'
  cache_control :public, :must_revalidate, :max_age => 600
  format :html

  map '/'

  error 500 do | exception |
      "Error 500\nInternal Error: #{ exception } (#{exception.backtrace.join("\n")})"
  end

  error 404 do |msg|
    "Error 404\nNot found! #{msg}"
    
  end

  def session
    "NIY"
  end

  def exit
    abort("exit")
  end

  def stream
    data = $agg.GetAllData()

    $logger.warn "Queue fetched: " + data.length.to_s

    str = ""

    $agg.GetAllData().each do |num|
      str += '[["sb","sensor"],{"ip":'+num[0].to_s + 
      ',"sensor_id":[' + num[5].to_s+
      ',UNDEF!!], "value":' + num[6].to_s + 
      ',"gts":' + num[3].to_s + 
      '.' + num[4].to_s + '}]' + "\n"
    end

    str
  end

end

