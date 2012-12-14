#! /usr/bin/env ruby
require 'mkmf'

extension_name = 'ruby_aggregator'

dir_config(extension_name)

have_library("aggregator", "Process")

create_makefile(extension_name)
