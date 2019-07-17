#!/bin/bash


all_nodes=(n0 n1 n2 n3 n4 n5 n6 n7 n8 n9 n10 n11 n12 n13 n14 n15 n16 n17 n18 n19 n20 n21 n22 n23 n24 n25 n26)

NUMCLIENTS=15

for ((i=0;i<$NUMCLIENTS;i++)); do
    n=${all_nodes[$i]}
    ssh  -o "StrictHostKeyChecking no" $n "sudo python /tmp/2D/setup/rm_qdisc.py"
done;
