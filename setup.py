import os, sys

def arr2str(arr):
	arr = map(lambda x: str(x), arr)
	arr = ','.join(arr)
	return arr

# constant #
tos=[4,32,40,56,72,128,152,184,192,224]

directory="/users/abdffsm/2D"
workload = "VL2_CDF.txt"
link_rate="10000"
serverIP = ["10.1.1.7"]
serverPort = ["5001"]
sequencerIP = ["10.1.1.7"]
sequencerPort = ["6001"]

mpl=[1,1,1,1,1,1,1,1,1,1]
thresholds=[3400,16176,545316,5159030,129372452,129372452]
ratios=[0.71,0.097,0.1,0.045,0.028,0.020]

num_classes = len(thresholds)

tos = tos[:num_classes]
mpl=mpl[:num_classes]

assert len(ratios) == len(thresholds)
assert len(tos) == len(thresholds)
assert len(mpl) == len(thresholds)

# setup qdisc at server nodes
for server in serverIP:
	os.system("ssh -o StrictHostKeyChecking=no %s sudo python %s/setup/configure_qdiscs.py -l %s -r %s -t %s"%(server, directory, link_rate, arr2str(ratios), arr2str(tos)))
	print "Installed qdiscs"

# creating config files
print "Creating config file..."
fd = open('%s/conf/client_config.txt'%directory,'w')
for i in range(len(serverIP)):
	print >> fd , "server %s %s" % (serverIP[i],serverPort[i])
for i in range(len(sequencerIP)):
	print >> fd , "sequencer %s %s" % (sequencerIP[i],sequencerPort[i])
print >> fd , "req_size_dist %s/run_expt/%s" % (directory, workload)
print >> fd , "rate %sMbps 100" % link_rate
for t in range(len(thresholds)):
	print >> fd , "threshold %d tos %d mpl %d" % (thresholds[t], tos[t], mpl[t])
fd.close()

# Make latest
os.system("sudo make")

# Copying latest files to run_expt
os.system("cp %s/bin/client %s/run_expt/" % (directory, directory))
os.system("cp %s/bin/sequencer %s/run_expt/" % (directory, directory))
os.system("cp %s/bin/server %s/run_expt/" % (directory, directory))
os.system("cp %s/conf/client_config.txt %s/run_expt/" % (directory, directory))
os.system("cp %s/conf/%s %s/run_expt/" % (directory, workload, directory))
os.system("cp %s/conf/%s %s/run_expt/" % (directory, workload, directory))
