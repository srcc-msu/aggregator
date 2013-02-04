#!/bin/bash

cd jsoncpp/
scons platform=linux-gcc
cp libs/linux-gcc-4.4.6/libjson_linux-gcc-4.4.6_libmt.so ../../proxy/bin/libjson.so
cp libs/linux-gcc-4.4.6/libjson_linux-gcc-4.4.6_libmt.so ../../csv_writer/bin/libjson.so
