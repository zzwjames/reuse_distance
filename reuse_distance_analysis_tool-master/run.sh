#!/bin/sh

PIN=/home/cc/jie/pin-3.6-97554-g31f0a167d-gcc-linux

$PIN/pin -t obj-intel64/rdpersig.so -- $@
