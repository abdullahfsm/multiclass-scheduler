import os, sys, random

# constant seed for reproducibility
random.seed(4591)

# Default Offered load (%/100)
pload=80.0

# Default Num flows
num_flows=100000

if len(sys.argv) > 1:
	for i in range(1,len(sys.argv),2):
		if sys.argv[i] == "-n":
			num_flows = int(sys.argv[i+1])
		elif sys.argv[i] == "-p":
			pload = float(sys.argv[i+1])


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
	elif "rate" in f:
		link_rate = int(f.split(' ')[1].split('M')[0])


# Client IPs
clients=["n0","n1","n2","n3","n4"]
num_clients=len(clients)
load = int((link_rate*pload)/(num_clients*100.0))


print "pload: %.1f" % pload
print "load: %.1fMbps" % load
print "num_flows: %d" % num_flows


assert link_rate > 0

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
	seed = random.uniform(1000,10000)
	os.system("ssh -o StrictHostKeyChecking=no %s killall client" % client)
	os.system("ssh -o StrictHostKeyChecking=no %s screen -d -m %s/bin/client -b %d -c %s/conf/client_config.txt -n %d -l %s/results/%s_fct -s %d"%(client, directory, load, directory, num_flows, directory ,client, seed))