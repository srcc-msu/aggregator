#!/usr/bin/env ruby

require 'rubygems'
require 'socket'
#require 'tempfile'

require 'bundler/setup'
require 'reel'
#!require 'simple_uuid'
require 'thread'
require 'logger'

require 'yaml'

$logger = Logger.new('_hoplang_proxy.log', 10, 1024000)

require_relative 'ruby_aggregator'

$agg = Aggregator.new

def Init
  config = YAML.load(File.open('./aggregator.conf', "r"))
  
  puts config.inspect
  
  $agg.InitAggregator(config["server"]["address"])
  $logger.warn ""
  $logger.warn "Aggregator started"

  config["agents"]["list"].each do |address|
    $agg.InitAgent(address[1])
    $logger.warn "#{address[0]} aka #{address[1]} agent added"
  end

  $agg.SetDelta(config["server"]["queue"]["delta"])
  $logger.warn "delta set to #{config["server"]["queue"]["delta"]}"

  $agg.SetInterval(config["server"]["queue"]["interval"])
  $logger.warn "max interval set to #{config["server"]["queue"]["interval"]}"

  sensors = config["server"]["buffer"]["sensors"]

  $logger.warn "#{sensors} id will be collected in buffer"

  sensors.each do |num|
    $agg.BufferAllowId(num)
  end

  Thread.new do
    while true
      $agg.Process()
      sleep(0.01)
    end
  end
  
  $logger.warn "Data collection started"
end

Init()

app = Rack::Builder.new do
  map '/stream' do
    run lambda { |env|
      body = Reel::Stream.new do |body|
        while true
          data = $agg.GetAllData()

          $logger.warn "Queue fetched: " + data.length.to_s

          data.each do |num|
            body << 
            "[[\"sb\",\"sensor\"],{\"ip\":#{num[0]%256}.#{num[0]/256%256}.#{num[0]/256/256%256}.#{num[0]/256/256/256%256}" +
            ",\"sensor_id\":[#{num[5].to_s}" +
            ",#{num[6].to_s}],\"value\":#{num[7].to_s}" +
            ",\"gts\":#{num[3].to_s}" +
            ".#{num[4].to_s}}]\n"
          end
        end
        body.finish
      end
      [200, {
        'Transfer-Encoding' => 'identity',
        'Content-Type' => 'text/html'
        }, body]
    }
  end

  map '/sensorstats' do
    run lambda { |env|
      body = Reel::Stream.new do |body|

        sensor_id = 1052
        sensor_num = 0
        address = "127.0.0.1"
        interval = 30

        data = $agg.GetInterval(address, sensor_id, interval)

        $logger.warn "Buffer fetched: " + data.length.to_s

        body << "\{\"#{sensor_id}.#{sensor_num}\":\{\n";

        data.each do |num|
          body << "\"#{num[3].to_s}.#{num[4].to_s}\":" +
          "#{num[6].to_s},\n"
        end

        body << "}}\n"
        
        body.finish
      end
      [200, {
        'Transfer-Encoding' => 'identity',
        'Content-Type' => 'text/html'
        }, body]
    }
  end
end.to_app

Rack::Handler::Reel.run app, Port: 9293
