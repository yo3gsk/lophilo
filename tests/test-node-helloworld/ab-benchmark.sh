#!/bin/sh
TS=`date +%s`
ab -n 1000 -c 10 -g ab-data-$TS.txt http://10.236.10.145:1337/ > ab-output-$TS.txt
