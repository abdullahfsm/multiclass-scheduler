import os, sys

os.system("cat /var/emulab/boot/ifmap > ifmapinfo.dat")
fd = open("ifmapinfo.dat",'r')
ft = fd.readlines()
fd.close()
os.system("rm ifmapinfo.dat")

interface = ft[0].split(' ')[0]
serverIP = ft[0].split(' ')[1]
seqIP = serverIP
os.system("tc qdisc delete dev %s root" % interface)
