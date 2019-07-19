import os, sys

def arr2str(arr):
	arr = map(lambda x: str(x), arr)
	arr = ','.join(arr)
	return arr

def config_parser(fname):
	global workload, link_rate, serverIP, serverPort, serverInterface
	global sequencerIP, sequencerPort, thresholds, ratios

	tf = open(fname,'r')
	ft = tf.readlines()
	tf.close()


	for f in ft:
		key,val = f.split(' ')

		if key == "workload":
			workload = val
		elif key == "link_rate":
			link_rate = val
		elif key == "serverIP":
			serverIP = val
		elif key == "serverPort":
			serverPort = val
		elif key == "serverInterface":
			serverInterface = val
		elif key == "sequencerIP":
			sequencerIP = val
		elif key == "sequencerPort":
			sequencerPort = val
		elif key == "thresholds":
			thresholds = val
		elif key == "ratios":
			ratios = val




# Full path of main directory
directory=os.getcwd()

# Workload
workload = None
link_rate = None
serverIP = None
serverPort = None
serverInterface = None
sequencerIP = None
sequencerPort = None
thresholds = None
ratios = None

config_parser("setup_config.tr")


tos=[4,32,40,56,72,128,152,184,192,224]
mpl=[1,1,1,1,1,1,1,1,1,1]
use_seq=[1,1,1,1,1,1,1,1,1,1]

num_classes = len(thresholds)
tos = tos[:num_classes]
mpl=mpl[:num_classes]

assert workload is not None
assert link_rate is not None
assert serverIP is not None
assert serverPort is not None
assert serverInterface is not None
assert sequencerIP is not None
assert sequencerPort is not None
assert thresholds is not None
assert ratios is not None

assert len(ratios) == len(thresholds)
assert len(tos) == len(thresholds)
assert len(mpl) == len(thresholds)

# Setup qdisc at server
print "Setting up qdiscs.."
os.system("ssh -o StrictHostKeyChecking=no %s sudo python %s/setup/configure_qdiscs.py -l %s -r %s -t %s -i %s"%(ServerInfo["ip"], directory, link_rate, arr2str(ratios), arr2str(tos), ServerInfo["interface"]))
print "Installed qdiscs"

# creating config files
print "Creating config file..."
fd = open('%s/conf/client_config.txt'%directory,'w')

print >> fd , "server %s %s" % (ServerInfo["ip"],ServerInfo["port"])
print >> fd , "sequencer %s %s" % (SequencerInfo["ip"],SequencerInfo["port"])
print >> fd , "req_size_dist %s/conf/%s" % (directory, workload)
print >> fd , "rate %sMbps 100" % link_rate

for t in range(len(thresholds)):
	# print >> fd , "threshold %d tos %d mpl %d" % (thresholds[t], tos[t], mpl[t])
	print >> fd , "class %d %d %d %d %d" % (t, thresholds[t], tos[t], mpl[t], use_seq[t])
fd.close()

# Make latest
os.system("sudo make")