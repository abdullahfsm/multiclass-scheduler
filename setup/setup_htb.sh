#!/bin/bash

echo "Setting up HTB.."
LINKRATE="10000"

#2D HTB
REMOTE_NODE=n1

#VL2
THRESHOLDS="3400,16176,545316,5159030,129372452"
RATIOS="0.71,0.097,0.1,0.045,0.028,0.020"

#UNI
#THRESHOLDS="15000000"
#RATIOS="0.999,0.001"

#CACHE
#THRESHOLDS="3072000"
#RATIOS="0.907,0.093"

#DCTCP
#THRESHOLDS="113154,5671937"
#RATIOS="0.544,0.353,0.103"

#EQUAL
#THRESHOLDS="1,2,3,4,5,6,7,8,9"
#RATIOS="0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1"

#HADOOP
#THRESHOLDS="2640,13332,2783837"
#RATIOS="0.874,0.04,0.048,0.038"

#MIX OF VL2 DCTCP CACHE
#THRESHOLDS="5773,1064257,7566875,106790675"
#RATIOS="0.246,0.521,0.188,0.038,0.007"

#MIX OF VL2 CACHE
#THRESHOLDS="3425,1027717,3134200,35540322"
#RATIOS="0.352,0.44,0.136,0.051,0.021"


#BARAAT HTB
#REMOTE_NODE=n29
#ALL workloads
# THRESHOLDS="1000000000000"
# RATIOS="0.999,0.001"


# ssh  -o "StrictHostKeyChecking no" $REMOTE_NODE "sudo python /tmp/2D/setup/configure_qdiscs.py -l ${LINKRATE} -t ${THRESHOLDS} -r ${RATIOS}"
# scp ${REMOTE_NODE}:/tmp/2D/conf/client_config2.txt ~/TrafficGenerator-master/run_expt/config_baraat.txt

ssh  -o "StrictHostKeyChecking no" $REMOTE_NODE "sudo python /tmp/2D/setup/configure_qdiscs.py -l ${LINKRATE} -t ${THRESHOLDS} -r ${RATIOS}"
scp ${REMOTE_NODE}:/tmp/2D/conf/client_config2.txt ~/TrafficGenerator-master/run_expt/config_2d.txt

#Aalo HTB
#REMOTE_NODE=n28


#ALL workloads
#THRESHOLDS="10,100,1000,10000,100000,1000000,10000000,100000000"
#RATIOS="0.2,0.18,0.16,0.13,0.11,0.09,0.07,0.04,0.02"

#ssh  -o "StrictHostKeyChecking no" $REMOTE_NODE "sudo python /tmp/2D/setup/configure_qdiscs.py -l ${LINKRATE} -t ${THRESHOLDS} -r ${RATIOS}"
#scp ${REMOTE_NODE}:/tmp/2D/conf/client_config2.txt ~/TrafficGenerator-master/run_expt/config_aalo.txt

#BARAAT HTB
#REMOTE_NODE=n29
#ALL workloads
#THRESHOLDS="1000000000000"
#RATIOS="0.999,0.001"

#ssh  -o "StrictHostKeyChecking no" $REMOTE_NODE "sudo python /tmp/2D/setup/configure_qdiscs.py -l ${LINKRATE} -t ${THRESHOLDS} -r ${RATIOS}"
#scp ${REMOTE_NODE}:/tmp/2D/conf/client_config2.txt ~/TrafficGenerator-master/run_expt/config_baraat.txt

#PIAS HTB
#REMOTE_NODE=n26
#VL2 workloads
#THRESHOLDS="1095000,1581180,2067360,20009300,21829920,30842500,41249380"
#RATIOS="1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0"

#DCTCP workloads
#THRESHOLDS="1108140,1652720,2125760,2536020,2934600,3210540,3394500"
#RATIOS="1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0"

#CACHE
#THRESHOLDS="1158323,1381737,1479833,1632172,1800193,1966151,2064874"
#THRESHOLDS="180164561,275708784,658888880,823383858"
#RATIOS="1.0,1.0,1.0,1.0,1.0"


#ssh  -o "StrictHostKeyChecking no" $REMOTE_NODE "sudo python /tmp/2D/setup/pias_configure_qdiscs.py -l ${LINKRATE} -t ${THRESHOLDS} -r ${RATIOS}"
#scp ${REMOTE_NODE}:/tmp/2D/conf/client_config2.txt ~/TrafficGenerator-master/run_expt/config_pias.txt
