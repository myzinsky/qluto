#!/usr/bin/sh

cd liquid-dsp
./bootstrap.sh
./configure --prefix=`pwd`/../../build/lib/liquid-dsp
make
make install
