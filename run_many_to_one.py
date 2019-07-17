import os, sys

# Precomputed seeds
seeds=[2193,4591,5467,9174,8120,3926,8612,6321,9492,100634,45610,1205,18522,18,23588,138484,21848,215498,38413,98453,8883,499213,8879,6482,96345,94382,45682]


# Default Offered load (%/100)
pload=80.0

# Default Num flows
num_flows=100000

if len(sys.argv == 2):
	num_flows = int(sys.argv[1])
elif len(sys.argv == 3):
	pload = float(sys.argv[2])

# Client IPs
clients=["n0","n1","n2","n3","n4"]
num_clients=len(clients)
load = int((link_rate*pload)/(num_clients*100.0))


print "pload: %.1f" % pload
print "load: %.1fMbps" % load
print "num_flows: %d" % num_flows

directory=os.getcwd()

tf = open("%s/conf/client_config.txt",'r')
ft = tf.readlines()
tf.close()

servers=[]
sequencers=[]
link_rate=0
for f in ft:
	if "server" in f:
		servers.append(f.split(' ')[1])
	elif "sequencer" in f:
		sequencers.append(f.split(' ')[1])
	elif "link_rate" in f:
		link_rate = int(f.split(' ')[1].split('M')[0])

assert link_rate > 0


print "Refreshing servers+sequencers.."


print "Making results directory.."
os.system("mkdir %s/results"%directory)

print "Starting servers.."
for server in servers:
	os.system("ssh -o StrictHostKeyChecking=no %s sudo killall server" % server)
	os.system("ssh -o StrictHostKeyChecking=no %s sudo nohup %s/bin/server &> /dev/null &" % (server, directory))

print "Starting sequencers.."
for seq in sequencers:
	os.system("ssh -o StrictHostKeyChecking=no %s sudo killall sequencer" % seq)
	os.system("ssh -o StrictHostKeyChecking=no %s sudo nohup %s/bin/sequencer &> /tmp/sequencer_logs &" % (seq, directory))
	
os.system("sleep 1")

print "Starting clients.."
i=0
for client in clients:
	os.system("ssh -o StrictHostKeyChecking=no %s killall client" % client)
	os.system("ssh -o StrictHostKeyChecking=no %s screen -d -m %s/bin/client -b %d -c %s/conf/client_config.txt -n %d -l %s/results/%s_fct -s %d"%(client, directory, load, directory, num_flows, directory ,client, seeds[i]))