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
require 'time'

$logger = Logger.new('_hoplang_proxy.log', 10, 1024000)

require_relative 'ruby_aggregator'

$agg = Aggregator.new
$subscribers = {}

MAX_QUEUE=100 # max unfetched request in queue for each stream

GC_SLEEP=60 # seconds to sleep untill next GC

def Init
  config = YAML.load(File.open('./aggregator.conf', "r"))
  
  $agg.InitAggregator(config["server"]["address"])
  
  $logger.warn ""
  $logger.warn ""

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

# start backgounrd processing of all packets
  $agg.BackgroundProcess()

  $logger.warn "Aggregator started"

# fetch the queue and add it to all subscribers
  Thread.new do
    while true
      data = $agg.GetAllData()
      
      $subscribers.each do |key, queue|
        queue << data
        
        if queue.length > MAX_QUEUE
          queue.clear()
        end

      end
      sleep(0.1)
    end
  end

# garbage collector thread.. /facepalm
  Thread.new do
    while true
      sleep(GC_SLEEP)
      GC.start
    end
  end

  $logger.warn "Data collection started"
end

#--------------------------------------------

semaphore = Mutex.new

app = Rack::Builder.new do
  map '/session/new' do
    run lambda { |env|
      query = {}
      
      env["QUERY_STRING"].split('&').each do |param|
        pair = param.split('=')
        query[pair[0]] = pair[1]
      end

      sessid = query["sessid"]

      $logger.warn "New subscirber: " + sessid

      $subscribers[sessid] = Queue.new

      [200, {}, []]
    }
  end

  map '/stream' do
    run lambda { |env|
      body = Reel::Stream.new do |body|
        query = {}
        
        env["QUERY_STRING"].split('&').each do |param|
          pair = param.split('=')
          query[pair[0]] = pair[1]
        end

        sessid = query["sessid"]

        $logger.warn "Subscriber " + sessid + " reading stream"

        while $subscribers[sessid]
          data = $subscribers[sessid].pop

          $logger.warn "Queue fetched for " + sessid + " : " + data.length.to_s

          data.each do |num|
            tmp = 
            "[[\"sb\",\"sensor\"],{\"ip\":" + 
            "\"#{num[0]%256}.#{num[0]/256%256}.#{num[0]/256/256%256}.#{num[0]/256/256/256%256}\"" +
            ",\"sensor_id\":[#{num[5].to_s}" +
            ",#{(num[6]+1).to_s}],\"value\":#{num[7].to_s}" +
            ",\"gts\":#{num[3].to_s}" +
            ".#{num[4].to_s}}]\n"
            body << tmp
          end

          sleep(0.1)
        end
        body.finish
      end
      [200, {}, body]
    }
  end

  map '/sensorstats' do
    run lambda { |env|
      body = Reel::Stream.new do |body|

        query = {}
        
        env["QUERY_STRING"].split('&').each do |param|
          pair = param.split('=')
          query[pair[0]] = pair[1]
        end

        tmp = query["sensor"].split('.')
          sensor_id = Integer(tmp[0])

        sensor_num = (tmp.length == 2 ? Integer(tmp[1]) : 1)

        sensor_num_c = sensor_num - 1

        address = query["ip"]
        
        from = query["from"]
        upto = query["upto"] # NIY

        convert = lambda { |val|
          if not val
            0
          elsif val[-1] == 'm'
            int_val = Integer(val[0..-2]) * 60
          else
            int_val = Integer(val)
          end
        }
        
        from = convert.call(from)
        upto = convert.call(upto)

        $logger.warn "Request #{sensor_id}.#{sensor_num} from #{address} from #{from} up to #{upto}" 

        semaphore.synchronize {
          data = $agg.GetInterval(address, sensor_id, sensor_num_c, from, upto)

          $logger.warn "Buffer fetched: " + data.length.to_s

          body << "\{\"#{sensor_id}.#{sensor_num}\":\{\n";

          data.each_with_index do |num, index|
            next if sensor_num_c != num[6]
            iso_time = DateTime.strptime(num[3].to_s, '%s').iso8601(6) 
            iso_time.slice!(29)
            body << "\"#{iso_time}\":" +
            "#{num[7].to_s}" + (index != data.length - 1 ? ",\n" : "}}\n")
          end
        }
     
        body.finish
      end
      [200, {}, body]
    }
  end
end.to_app

Init()
$logger.warn "Webserver started"

Rack::Handler::Reel.run app, Port: 9293
