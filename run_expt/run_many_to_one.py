import os, sys

# Precomputed seeds
seeds=[2193,4591,5467,9174,8120,3926,8612,6321,9492,100634,45610,1205,18522,18,23588,138484,21848,215498,38413,98453,8883,499213,8879,6482,96345,94382,45682]

pload=80.0
link_rate=10000
num_flows=100000

directory="/users/abdffsm/2D"

# Client IPs
clients=["n0","n1","n2","n3","n4"]

# Server IP
servers=["n5"]

# Sequencer IP
sequencers=["n5"]


num_clients=len(clients)
load = int((link_rate*pload)/(num_clients*100.0))

print "pload: %.1f" % pload
print "load: %.1fMbps" % load
print "num_flows: %d" % num_flows

print "Refreshing servers+sequencers.."

for server in servers:
	os.system("ssh -o StrictHostKeyChecking=no %s sudo killall server" % server)
	os.system("ssh -o StrictHostKeyChecking=no %s sudo nohup %s/bin/server &> /dev/null &" % (server, directory))

for seq in sequencers:
	os.system("ssh -o StrictHostKeyChecking=no %s sudo killall sequencer" % seq)
	os.system("ssh -o StrictHostKeyChecking=no %s sudo nohup %s/bin/sequencer &> /tmp/sequencer_logs &" % (seq, directory))
	
os.system("sleep 1")

i=0
for client in clients:
	os.system("ssh -o StrictHostKeyChecking=no %s killall client" % client)
	os.system("ssh -o StrictHostKeyChecking=no %s screen -d -m %s/bin/client -b %d -c %s/conf/client_config.txt -n %d -l %s/results/%s_fct -s %d"%(client, directory, load, directory, num_flows, directory ,client, seeds[i]))