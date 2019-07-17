import os, sys

directory="/users/abdffsm/2D"

# ServerIP
servers=["n3"]

# SequencerIP
sequencers=["n3"]

print "Refreshing servers+sequencers.."

for server in servers:
    os.system("ssh -o StrictHostKeyChecking=no %s sudo killall server" % server)
    os.system("ssh -o StrictHostKeyChecking=no %s sudo nohup %s/bin/server &> /dev/null &" % (server, directory))

for seq in sequencers:
    os.system("ssh -o StrictHostKeyChecking=no %s sudo killall sequencer" % seq)
    os.system("ssh -o StrictHostKeyChecking=no %s sudo nohup %s/bin/sequencer &> /dev/null &" % (seq, directory))