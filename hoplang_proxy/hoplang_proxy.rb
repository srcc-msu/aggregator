#!/usr/bin/env ruby

require 'rubygems'
require 'socket'

require 'reel'
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

  $agg.SetDelta(config["server"]["queue"]["delta"])
  $logger.warn "delta set to #{config["server"]["queue"]["delta"]}"

  $agg.SetInterval(config["server"]["queue"]["interval"])
  $logger.warn "max interval set to #{config["server"]["queue"]["interval"]}"

  sensors = config["server"]["buffer"]["sensors"]

  $logger.warn "#{sensors} id will be collected in buffer"

  sensors.each do |num|
    $agg.BufferAllowId(num)
  end
$agg.QueueBlacklistId(101)
$agg.QueueBlacklistId(110)
$agg.QueueBlacklistId(111)
$agg.QueueBlacklistId(112)
$agg.QueueBlacklistId(1010)
$agg.QueueBlacklistId(1018)
$agg.QueueBlacklistId(1020)
$agg.QueueBlacklistId(1030)
$agg.QueueBlacklistId(1051)
$agg.QueueBlacklistId(1052)
$agg.QueueBlacklistId(1053)
$agg.QueueBlacklistId(1054)
$agg.QueueBlacklistId(1055)
$agg.QueueBlacklistId(1056)
$agg.QueueBlacklistId(1080)
$agg.QueueBlacklistId(1130)
$agg.QueueBlacklistId(1210)
$agg.QueueBlacklistId(1211)
$agg.QueueBlacklistId(1220)
$agg.QueueBlacklistId(1225)
$agg.QueueBlacklistId(1230)
$agg.QueueBlacklistId(1231)
$agg.QueueBlacklistId(1232)
$agg.QueueBlacklistId(1233)
$agg.QueueBlacklistId(1234)
$agg.QueueBlacklistId(1235)
$agg.QueueBlacklistId(1236)
$agg.QueueBlacklistId(1237)
$agg.QueueBlacklistId(1238)
$agg.QueueBlacklistId(1239)
$agg.QueueBlacklistId(1240)
$agg.QueueBlacklistId(1241)
$agg.QueueBlacklistId(1255)
$agg.QueueBlacklistId(1256)
$agg.QueueBlacklistId(1257)
$agg.QueueBlacklistId(1258)
$agg.QueueBlacklistId(1270)
$agg.QueueBlacklistId(1271)
$agg.QueueBlacklistId(1272)
$agg.QueueBlacklistId(1280)
$agg.QueueBlacklistId(1281)
$agg.QueueBlacklistId(1290)
$agg.QueueBlacklistId(1291)
$agg.QueueBlacklistId(1292)
$agg.QueueBlacklistId(1300)
$agg.QueueBlacklistId(1303)
$agg.QueueBlacklistId(1310)
$agg.QueueBlacklistId(1314)
$agg.QueueBlacklistId(1320)
$agg.QueueBlacklistId(1330)
$agg.QueueBlacklistId(1410)
$agg.QueueBlacklistId(1411)
$agg.QueueBlacklistId(1412)
$agg.QueueBlacklistId(2100)
$agg.QueueBlacklistId(2101)
$agg.QueueBlacklistId(2105)
$agg.QueueBlacklistId(2200)
$agg.QueueBlacklistId(2201)
$agg.QueueBlacklistId(2202)
$agg.QueueBlacklistId(2203)
$agg.QueueBlacklistId(2204)
$agg.QueueBlacklistId(2205)
$agg.QueueBlacklistId(2206)
$agg.QueueBlacklistId(2207)
$agg.QueueBlacklistId(2212)
$agg.QueueBlacklistId(2213)
$agg.QueueBlacklistId(2214)
$agg.QueueBlacklistId(2215)
$agg.QueueBlacklistId(2216)
$agg.QueueBlacklistId(2561)
$agg.QueueBlacklistId(2565)
$agg.QueueBlacklistId(2567)
$agg.QueueBlacklistId(2570)
$agg.QueueBlacklistId(2754)
$agg.QueueBlacklistId(3001)
$agg.QueueBlacklistId(3002)
$agg.QueueBlacklistId(3003)
$agg.QueueBlacklistId(3004)
$agg.QueueBlacklistId(3005)
$agg.QueueBlacklistId(3006)
$agg.QueueBlacklistId(3007)
$agg.QueueBlacklistId(3008)
$agg.QueueBlacklistId(3009)
$agg.QueueBlacklistId(3010)
$agg.QueueBlacklistId(3011)
$agg.QueueBlacklistId(3012)
$agg.QueueBlacklistId(3013)
$agg.QueueBlacklistId(3014)
$agg.QueueBlacklistId(3015)
$agg.QueueBlacklistId(3016)
$agg.QueueBlacklistId(3017)
$agg.QueueBlacklistId(3018)
$agg.QueueBlacklistId(3019)
$agg.QueueBlacklistId(3020)
$agg.QueueBlacklistId(3021)
$agg.QueueBlacklistId(3022)
$agg.QueueBlacklistId(3023)
$agg.QueueBlacklistId(3024)
$agg.QueueBlacklistId(3025)
$agg.QueueBlacklistId(3026)
$agg.QueueBlacklistId(3027)
$agg.QueueBlacklistId(3028)
$agg.QueueBlacklistId(3029)
$agg.QueueBlacklistId(3030)
$agg.QueueBlacklistId(5001)
$agg.QueueBlacklistId(5002)
$agg.QueueBlacklistId(5003)
$agg.QueueBlacklistId(5004)
$agg.QueueBlacklistId(5005)
$agg.QueueBlacklistId(5006)
$agg.QueueBlacklistId(5007)
$agg.QueueBlacklistId(5008)
$agg.QueueBlacklistId(5009)
$agg.QueueBlacklistId(5010)
$agg.QueueBlacklistId(5011)
$agg.QueueBlacklistId(5012)
$agg.QueueBlacklistId(5013)
$agg.QueueBlacklistId(5014)
$agg.QueueBlacklistId(5015)
$agg.QueueBlacklistId(5016)
$agg.QueueBlacklistId(5017)
$agg.QueueBlacklistId(5018)
$agg.QueueBlacklistId(5019)
$agg.QueueBlacklistId(5020)
$agg.QueueBlacklistId(5501)
$agg.QueueBlacklistId(5502)
$agg.QueueBlacklistId(5503)
$agg.QueueBlacklistId(5504)
$agg.QueueBlacklistId(5505)
$agg.QueueBlacklistId(5506)
$agg.QueueBlacklistId(5507)
$agg.QueueBlacklistId(28000)
$agg.QueueBlacklistId(28001)
$agg.QueueBlacklistId(28002)
$agg.QueueBlacklistId(28003)
$agg.QueueBlacklistId(28004)
$agg.QueueBlacklistId(28005)
$agg.QueueBlacklistId(28006)
$agg.QueueBlacklistId(28007)
$agg.QueueBlacklistId(28008)
$agg.QueueBlacklistId(28009)
$agg.QueueBlacklistId(28010)
$agg.QueueBlacklistId(28011)
$agg.QueueBlacklistId(28012)
$agg.QueueBlacklistId(28013)
$agg.QueueBlacklistId(28014)
$agg.QueueBlacklistId(28015)
$agg.QueueBlacklistId(28016)
$agg.QueueBlacklistId(28017)
$agg.QueueBlacklistId(28018)
$agg.QueueBlacklistId(28019)
$agg.QueueBlacklistId(28020)
$agg.QueueBlacklistId(28021)
$agg.QueueBlacklistId(28022)
$agg.QueueBlacklistId(28023)
$agg.QueueBlacklistId(28024)
$agg.QueueBlacklistId(28025)
$agg.QueueBlacklistId(28026)
$agg.QueueBlacklistId(28027)
$agg.QueueBlacklistId(28028)
$agg.QueueBlacklistId(28029)
$agg.QueueBlacklistId(32768)
$agg.QueueBlacklistId(32770)
$agg.QueueBlacklistId(32772)
$agg.QueueBlacklistId(32774)
$agg.QueueBlacklistId(32776)
$agg.QueueBlacklistId(32778)
$agg.QueueBlacklistId(32780)
$agg.QueueBlacklistId(40960)
$agg.QueueBlacklistId(40962)
$agg.QueueBlacklistId(40964)
$agg.QueueBlacklistId(41220)
$agg.QueueBlacklistId(41222)
$agg.QueueBlacklistId(41224)
$agg.QueueBlacklistId(41226)
$agg.QueueBlacklistId(41228)
$agg.QueueBlacklistId(41230)
$agg.QueueBlacklistId(64001)
$agg.QueueBlacklistId(64002)
$agg.QueueBlacklistId(64003)
$agg.QueueBlacklistId(64004)
$agg.QueueBlacklistId(64005)
$agg.QueueBlacklistId(64006)
$agg.QueueBlacklistId(64007)
$agg.QueueBlacklistId(64008)
$agg.QueueBlacklistId(64009)
$agg.QueueBlacklistId(64010)
$agg.QueueBlacklistId(64011)
$agg.QueueBlacklistId(64012)
$agg.QueueBlacklistId(64016)
$agg.QueueBlacklistId(64032)
$agg.QueueBlacklistId(64033)
$agg.QueueBlacklistId(64034)
$agg.QueueBlacklistId(64054)
$agg.QueueBlacklistId(64064)
$agg.QueueBlacklistId(64068)
$agg.QueueBlacklistId(64050)
$agg.QueueBlacklistId(64051)
$agg.QueueBlacklistId(64052)
$agg.QueueBlacklistId(64055)
$agg.QueueBlacklistId(64059)
# start backgounrd processing of all packets
  $agg.BackgroundProcess()

  Thread.new do
    config["agents"]["list"].each do |address|
      $agg.InitAgent(address[1])
      $logger.warn "#{address[0]} aka #{address[1]} agent added"
      sleep 1.0/166.0
    end
  end

  $logger.warn "Aggregator started"

# fetch the queue and add it to all subscribers
  Thread.new do
    while true
      data = $agg.GetAllData()

      $logger.warn "fetched #{data.size}"

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
            "#{num[7].to_s}" + (index != data.length - 1 ? ",\n" : "")
          end
        }
        body << "}}\n"
        body.finish
      end
      [200, {}, body]
    }
  end
end.to_app

Init()
$logger.warn "Webserver started"

Rack::Handler::Reel.run app, Port: 9293
