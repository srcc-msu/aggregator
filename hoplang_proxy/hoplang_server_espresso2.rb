#!/usr/bin/env ruby

require 'rubygems'
require 'socket'
require 'avro'
#require 'tempfile'
require 'e'
#!require 'simple_uuid'
require 'thread'
require 'logger'
require 'oj'

require 'yaml'



PROTOCOL_JSON = File.read('queryserver.avpr')
PROTOCOL = Avro::Protocol.parse(PROTOCOL_JSON)

HOP_PATH='/home/hopsa/hoplang-server/hoplang'
MAX_LA=15

$logger = Logger.new('hopserver.log', 10, 1024000)

class GlobalCounter
  @@counter=0
  @@counter_mutex=Mutex.new

  def self.plus
    @@counter_mutex.synchronize do
      @@counter+=1
    end
  end

  def self.minus
    @@counter_mutex.synchronize do
      @@counter-=1
    end
  end

  def self.get
    ret=0
    @@counter_mutex.synchronize do
      ret=@@counter
    end
    return ret
  end
end

class HoplangProcessor

  TMP_PREFIX='/tmp'
  QUERY_TMP_PREFIX="#{TMP_PREFIX}/hopquery-"
  HEX_DIGITS='0123456789abcdef'


  def initialize(text)
    @query_path = rnd_file QUERY_TMP_PREFIX
    File.open @query_path, 'w' do |query_file|
      query_file.write text
    end
  end
  
  def process
    `cd #{HOP_PATH}; ruby ./hpl #{@query_path}`
  end

  # creates a randomly named file/directory, and returns the name
  def rnd_name(prefix,dir)
    file_created = false
    rnd_name = ''
    while !file_created
      # read random data
      rnd_bytes = ''
      File.open '/dev/urandom' do |rnd_src|
        rnd_bytes = rnd_src.read 6
      end
      # get random hex name
      rnd_name = prefix
      rnd_bytes.each_byte do |b|
        rnd_name += HEX_DIGITS[b / 16, 1];
        rnd_name += HEX_DIGITS[b % 16, 1];
      end
      # puts "random name = #{rnd_name}"
      # try to create random file or directory
      begin
        if dir
          Dir.mkdir rnd_name, 0700
        else
          f = File.new rnd_name, File::CREAT|File::EXCL|File::RDWR, 0600
          f.close
        end
        file_created = true
      rescue SystemCallError
        # just try once more
        # TODO: 
      end
    end # !created
    return rnd_name
  end #rnd_name

  def rnd_file(prefix)
    rnd_name prefix, false
  end

  def rnd_dir(prefix)
    rnd_name prefix, true
  end
  
end

class QueryServerResponder < Avro::IPC::Responder
  def initialize
    super(PROTOCOL)
  end


  #RES_TMP_PREFIX="#{TMP_PREFIX}/hopres-"
  MAX_LA=15

  def call(message, request)
    case message.name 
    when 'doQuery'
      la=`w|head -n 1` # 14:29:53 up  3:01,  4 users,  load average: 2.22, 2.01, 1.95
      la.match /average: ([1234567890.]+)/
      la=$~[1].to_i
      if la > MAX_LA
        $logger.warn "LA=#{la}, so skip"
        return "--\nToo high load: #{la}. Try request later"
      end
      query = request['query']
      # query_path = rnd_file QUERY_TMP_PREFIX
      # File.open query_path, 'w' do |query_file|
      #   query_file.write query
      # end
      processor=HoplangProcessor.new(query)

      count=GlobalCounter.get
      if count>MAX_LA
        $logger.warn "Request count=#{count}, so skip"
        return "--\nToo many requests: #{count}. Try request later"
      end
      GlobalCounter.plus
      $logger.warn "starting request handling (#{query_path}) at "+Time.now.to_s
      $logger.warn query

      #result=`cd hoplang; ruby ./hpl #{query_path}`
      start_time=Time.now
      result = processor.process #`cd #{HOP_PATH}; ruby ./hpl #{query_path}`

      # return result
      end_time=Time.now
      $logger.warn "finished request handling (#{query_path}) at #{end_time.to_s} (#{end_time-start_time} sec)"
      $logger.warn "Code: #{$?.to_s}; bytes: #{result.length}"
      GlobalCounter.minus
      if result[0,2] == '--'
        return result
      elsif $?.to_i != 0
        return "--\nError (#{$!}).\n #{result}"
      end
      return result
    else
      $logger.warn 'unknown message'
      $logger.warn message.name
      $logger.warn message.inspect
      return ''
    end
  end
end


# class RequestHandler
#   def initialize(address, port)
#     @ip_address = address
#     @port = port
#   end

#   def run
#     server = TCPServer.new(@ip_address, @port)
#     while (session = server.accept)
#       $logger.warn "accepting session\n"
#       handle(session)
#       session.close
#       $logger.warn "session closed\n"
#     end
#   end
# end

# class QueryServerHandler < RequestHandler
#   def handle(request)
#     responder = QueryServerResponder.new
#     transport = Avro::IPC::SocketTransport.new(request)
#     $logger.warn "created responder for request\n"
#     str = transport.read_framed_message
#     $logger.warn "read message\n"
#     $logger.warn str.inspect
#     response = responder.respond str
#     $logger.warn "obtained response message\n"
#     transport.write_framed_message response
#     $logger.warn "written response\n"
#   end
# end

# class HttpQueryServerHandler < WEBrick::HTTPServlet::AbstractServlet
#   def do_POST(req, resp)
#     $logger.warn "AVRO REQUEST"
#     responder = QueryServerResponder.new
#     call_request = Avro::IPC::FramedReader.new(StringIO.new(req.body)).read_framed_message
#     unframed_resp = responder.respond(call_request)
#     writer = Avro::IPC::FramedWriter.new(StringIO.new)
#     writer.write_framed_message(unframed_resp)
#     resp.body = writer.to_s
#   end
# end

class HopLangHandler < E

  # QUERY_TMP_PREFIX='/tmp/hopquery-'

  # def do_POST(request, response)
  #   start=Time.now
  #   $logger.warn ">> #{request.path}; #{request.query};"
  #   if request.path =~ %r{^/html/(?<p>.*)}
  #     path=p
  #     text=request.query["request"]
  #   else
  #     text=request.body
  #     path=request.path
  #   end

  #   $logger.warn "Start processing #{p} at #{start} :\n#{text}\n--------------------------"
  #   case path
  #   # when /test/
  #   #   status, content_type, body = 200, 'html/text', "counter = #{@@counter}"
  #   when 'hoplang'
  #     status, content_type, body = do_hoplang(text)
  #   when /^control/
  #     status, content_type, body = do_control(path,text)
  #   when /^auth/
  #     status, content_type, body = do_auth(path,text)
  #   when /^info/
  #     status, content_type, body = do_info(path,text)
  #   else
  #     status=500
  #     content_type='text/html'
  #     body="<html><body><b>Error: invalid request path: #{request.path}.</body></html>"
  #   end


  #   response.status = status
  #   response['Content-Type'] = content_type
  #   response.body = body

  #   duration=Time.now-start
  #   $logger.warn "End processing started at #{start}. Duration: #{duration}"
  # end

# #  control/reload_server - обновить конфигурацию сервера
# #  control/stop_server - остановить сервер
# #
#   def do_control(req,text)
#     if req.match /reload_server/
#       return 200, 'text/html', 'Server config reloaded'
#     elsif req.match /stop_server/
#     end
#   end

#  auth/user - аутентифицировать пользователя (параметры name, password)
  def do_auth(user,text)
    return 200, 'text/html', "User #{user} authenticated"
  end

# #  info/version - информация по поддерживаемым версиям протокола L-Control
# #  info/server - информация по состоянию сервера
# #  info/nodes  - информация по узлам, с которых собирается информация
# #  info/sensors - информация по сенсорам
# #  info/status - информация по состоянию комплекса
# #  info/users  - список пользователей
# #  info/user  - получить информацию по пользователю (параметр name) 
#   def do_info(req,text)
#     case req
#     when 'version'
#       return 200, 'text/html', "Name: hoplang\nVersion: 0.1\nSupported: 0.1\nDialects: ver1\nControl versions: 1.0"
#     when 'server'
#       return 200, 'text/html', "Server is running. Active requests: #{GlobalCounter.get}"
#     when 'nodes'
#       return 200, 'text/html', `uname -n`
#     when 'sensors'
#       return 500, 'text/html', "Not implemented yet"
#     when 'status'
#       return 200, 'text/html', "OK"
#     when 'users'
#       return 200, 'text/html', "Not implemented yet"
#     when 'user'
#       return 200, 'text/html', "Not implemented yet"
#     else 
#       return 404,  'text/html', 'Bad command'
#     end
#   end


#   def do_hoplang(text)
# #    content = "<p>some text #{request.header};#{request.body};#{request.query};#{request.path}</p>"
#     count=GlobalCounter.get
#     if count>MAX_LA
#       $logger.warn "COUNTER=#{count}, so skip"
#       return 500, ''
#     else
#       GlobalCounter.plus
#     end

#     query_path = rnd_file QUERY_TMP_PREFIX
#     File.open query_path, 'w' do |query_file|
#       query_file.write text
#     end

#     $logger.warn "=================================>"
#     content = `cd #{HOP_PATH}; ruby ./hpl #{query_path}`
#     File.delete query_path
#     $logger.warn "<================================="

#     GlobalCounter.minus

#     if content[0,3] == "--\n"
#       return 500, 'text/html', content
#     end
#     return 200, 'text/csv', content
#   end

  # def rnd_file(prefix)
  #   loop {
  #     uuid=SimpleUUID::UUID.new.to_guid
  #     path=prefix+uuid
  #     return path unless File.exist? path
  #   }
  # end
  
end

class HopServer <E


  # TMP_PREFIX='/tmp'
  # QUERY_TMP_PREFIX="#{TMP_PREFIX}/hopquery-"
  #RES_TMP_PREFIX="#{TMP_PREFIX}/hopres-"
  MAX_LA=15

  charset 'utf-8'
  cache_control :public, :must_revalidate, :max_age => 600
  format :json, :html

  setup '.csv' do
    format :csv
    content_type '.csv'
  end
  setup '.json' do
    format :json
  end

  map '/'

  error 500 do | exception |
      "Error 500\nInternal Error: #{ exception } (#{exception.backtrace.join("\n")})"
  end

  error 404 do |msg|
    "Error 404\nNot found! #{msg}"
    
  end

  #
  #  process POST request to /
  #  AVRO request by default
  #
  def post_index
    # text=env['rack.input'].read
    # "POST #{text}\n#{env.inspect}"
    $logger.warn "AVRO REQUEST"
    req=nil
    responder = QueryServerResponder.new
    call_request = Avro::IPC::FramedReader.new(env['rack.input']).read_framed_message
    unframed_resp = responder.respond(call_request)
    writer = Avro::IPC::FramedWriter.new(StringIO.new)
    writer.write_framed_message(unframed_resp)

    return writer.to_s
  end

  def get_index *args
    "GET\n"
  end


  def post_test *args
    @out={'counter' => GlobalCounter.get}
#    if (accept =~ /json/) or args[0] == 'json'
    if args[0] == 'json'
      content_type! '.json'
      render {Oj.dump @out}
    else
      "TEST: #{@out}\nARGS: #{args.inspect}\nENV: " + env.to_s
    end
  end

#  control/reload_server - обновить конфигурацию сервера
#  control/stop_server - остановить сервер
#
  def get_control req
    case req
    when 'reload_server'
      "Server config reloaded\n"
    when 'stop_server'
      "Are you sure???!!!\n"
    end
  end

  def get_info *args
    @out={}
    case args[0]
    when 'version'
      @out={'Name' => 'hoplang',
            'Version' => '0.1',
            'Supported' => '0.1',
            'Dialects' => 'hopsa_1',
            'Control versions' => '1.0'}
    when 'server'
      @out={'state' => "Server is running. Active requests: #{GlobalCounter.get}"}
    when 'nodes'
      name=`uname -n`
      @out={'nodes' => [name.chomp]}
    when 'sensors'
      #halt 500, "Not implemented yet\n"
      @hopsa_conf=get_hopsa_conf
      @out=[]
      @hopsa_conf['varmap'].each_pair do |name,h|

# - Name
#     short_metric_name
# - Id
#     1010 etc, i.e. - number
# - Description
#     human readable description
# - Unit
#      "bytes/sec" etc
# - Type
#     absolute/accumulated
# - Scope
#     {now; sinceLast; untilNext}
# - Reasonable threshold
#     {min;max} - for validation
# - Availability
# {is_working, available}
# - Sources
# {clustrxwatch, ganglia, etc...}
# - granularity
#     in Hz
# - nodes?
#     nodelist
        @hopsa_conf['nodes'] ||= {}
        item= {}
#        @name=name
        item['name'] = name
        item['id'] = h['id'] || 9999
        item['desc']= h['desc'] || 'None'
        item['unit']= h['unit'] || 'items' # %, bytes/sec, bytes, ...
        item['val_type']= h['val_type'] || 'abs' # 'accum'
        item['scope']= h['scope'] || 'sinceLast' # sinceLast / untilNext
        item['supported']= 1 # 0 (is in active configuration)
        item['avail']= 1 # 0 (is now active)
        item['source']= h['source'] || 'clustrxwatch'
        item['granularity']= 1 # Hz
        item['min']= h['min'] || 'None'
        item['max']= h['max'] || 'None'
        item['nodes'] = @hopsa_conf['nodes']['all'] || []
        @out << item
      end
    when 'status'
      @out={'status' => "OK"}
    when 'users'
      halt 500, "Not implemented yet\n"
    when 'user'
      halt 500, "Not implemented yet\n"
    else 
      halt 404, "Bad command: '#{args[0]}'\n"
    end
#    if (accept =~ /json/) or (args[1] == 'json')
    if (args[1] == 'json')
      content_type! '.json'
      render 'get_info.json'
    else
      render
    end
  end

  def get_hopsa_conf
    YAML.load(File.open('./hopsa.conf', "r"))
  end

  #
  #  process POST request to /hoplang{,.csv,.json}
  #  run program and get result
  #
  def post_hoplang request=nil
    count=GlobalCounter.get
    if count>MAX_LA
      $logger.warn "COUNTER=#{count}, so skip"
      halt 500, "Load average is too high (#{count}). Try later.\n"
    else
      GlobalCounter.plus
    end

    # query_path = rnd_file QUERY_TMP_PREFIX
    # File.open query_path, 'w' do |query_file|
    #   query_file.write (request || env['rack.input'].read)
    # end
    processor = HoplangProcessor.new(request || env['rack.input'].read)

    $logger.warn "=================================>"
    content = processor.process #`cd #{HOP_PATH}; ruby ./hpl #{query_path}`
    #File.delete query_path
    $logger.warn "<================================="

    GlobalCounter.minus

    if content[0,3] == "--\n"
      halt 500, content
    end
    content_type! '.csv'
    content
  end

  #
  #  process GET request to /html
  #  just the same as /hoplang, but request is passed into 'request' parameter
  #
  def get_html
    pass :get_hoplang, params['request']
  end

end


Oj.default_options={:mode => :object, :indent => 1, :circular => false}

#!app=HopServer.mount '/'

#!port = 9099

#!$logger.warn "starting query server\n"

#!app.run :server => :Thin, :Port => port, :num_processors => 100


#Dir.chdir '/home/hadoop/pig-server/bin'
# handler = QueryServerHandler.new(host, port)
# handler.run
#server = WEBrick::HTTPServer.new(:Host => host, :Port => port)
#server.mount '/hoplang', HopLangHandler
#server.mount '/html', HopLangHandler
#server.mount '/', HttpQueryServerHandler
#trap("INT") { server.shutdown }
#daemonize
#server.start
#$logger.warn "done\n"
