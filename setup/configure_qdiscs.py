import os, sys

def readargs(args):
	global link_rate, ratios, tos

	i=0

	while(i<len(args)):
		if(args[i]=='-l'):
			link_rate = int(args[i+1])
		elif(args[i]=='-r'):
			ratios=args[i+1]
			ratios = map(lambda x: float(x),ratios.split(','))
		elif(args[i]=='-t'):
			tos=args[i+1]
			tos = map(lambda x: int(x),tos.split(','))
		i+=2

os.system("cat /var/emulab/boot/ifmap > ifmapinfo.dat")
fd = open("ifmapinfo.dat",'r')
ft = fd.readlines()
fd.close()
os.system("rm ifmapinfo.dat")

interface = ft[0].split(' ')[0]

link_rate = None
ratios = None
tos = None

readargs(sys.argv[1:])

print "PRINTING DEBUG"
print "Interface: %s" % interface
print "link rate: %d Mbps" % link_rate
print "ratios: %s" % str(ratios)
print "tos map: %s" % str(tos)

sratio = int(round(sum(ratios)))
num_classes = len(ratios)
assert sratio == 1
assert len(ratios) == len(tos)


# setting up filters
print "Setting up qdiscs..."
os.system("tc qdisc delete dev %s root" % interface)
os.system("tc qdisc add dev %s root handle 1: htb default 1%d" % (interface,num_classes-1))
os.system("tc class add dev %s parent 1: classid 1:1 htb rate %dmbit ceil %dmbit" % (interface,link_rate,link_rate))

for r in range(len(ratios)):
	os.system("tc class add dev %s parent 1:1 classid 1:1%d htb rate %dmbit ceil %dmbit" % \
		(interface,r,int(ratios[r]*link_rate), link_rate))
	os.system("tc filter add dev %s protocol ip parent 1:0 prio 1 u32 match ip tos %d 0xff flowid 1:1%d" % \
		(interface,tos[r],r))
	print "Class:%d Tos:%d Rate:%d" % (r,tos[r],int(ratios[r]*link_rate))