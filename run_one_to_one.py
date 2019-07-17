import os, sys

# Default Offered load (%/100)
pload=80.0

# Default Num flows
num_flows=100000

for i in range(0,len(sys.argv),2):
	if sys.argv[i] == "-n":
		num_flows = int(sys.argv[i+1])	
	elif sys.argv[i] == "-p":
		pload = float(sys.argv[i+1])

# Client IPs
load = int((link_rate*pload)/(100.0))

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

print "Starting client.."
os.system("%s/bin/client -b %d -c %s/conf/client_config.txt -n %d -l %s/results/flows_fct"%(directory, load, directory, num_flows, directory))