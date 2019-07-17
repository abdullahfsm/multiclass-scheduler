import os, sys
directory="/users/abdffsm/2D/run_expt"
clients=["n0","n1","n2"]
servers=["n3"]
sequencers=["n3"]

print "Refreshing servers+sequencers.."

for server in servers:
    os.system("ssh -o StrictHostKeyChecking=no %s sudo killall server" % server)
    os.system("ssh -o StrictHostKeyChecking=no %s sudo nohup %s/server &> /dev/null &" % (server, directory))

for seq in sequencers:
    os.system("ssh -o StrictHostKeyChecking=no %s sudo killall sequencer" % seq)
    os.system("ssh -o StrictHostKeyChecking=no %s sudo nohup %s/sequencer &> /dev/null &" % (seq, directory))
