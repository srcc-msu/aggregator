#!/bin/bash

cd jsoncpp/
scons platform=linux-gcc
cp libs/linux-gcc*/libjson_linux*_libmt.so ../../proxy/bin/libjson.so
cp libs/linux-gcc*/libjson_linux*_libmt.so ../../csv_writer/bin/libjson.so
