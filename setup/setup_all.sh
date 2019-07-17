#!/bin/bash

#used to copy files to all nodes
#30 Nodes
#nodes=(n0 n1 n2 n3 n4 n5 n6 n7 n8 n9 n10 n11 n12 n13 n14 n15 n16 n17 n18 n19 n20 n21 n22 n23 n24 n25 n26 n27 n28 n29)
#2 Nodes
nodes=(n0 n1)
#nodes=(n0 n1 n2 n3 n4 n5 n6 n7 n8 n9 n10 n11)

echo "Making Client-Server"
cd ~/TrafficGenerator-master
make clean
make

#File system is updated
#/tmp/DANS/setup/./update_fs.sh

for n in ${nodes[*]}; do
    ssh -o StrictHostKeyChecking=no $n "~/TrafficGenerator-master/setup/./copy_all.sh"
#    ssh -o StrictHostKeyChecking=no $n "~/DB/./dbinstall.sh"
#    ssh -o StrictHostKeyChecking=no $n "~/DB/./dbinstall.sh;~/DB/./patch.sh"
    echo $n" done"
done
