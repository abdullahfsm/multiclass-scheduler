import os, sys

def makecdf(length):
	t = 0
	cdf=[]

	for i in range(length):
		t += 1.0/length
		cdf.append(t)
	return cdf

def cond(entry):
	mybool = 0 or int(entry.split(' ')[0]) == 0
	mybool = mybool or int(entry.split(' ')[1]) == 0
	# mybool = mybool or float(entry.split(' ')[2]) < 400.0
	return mybool

def avg(l):
	if len(l) == 0:
		return -1
	return sum(l)/len(l)

files = ["2d_fct", "fifo_fct", "ps_fct"]
cdfpair=[]
N=0
i=0
for file in files:

		# tfile = file.split('.')[0]+str(run)+'.txt'
	tf = open(file,'r')
	ft = tf.readlines()
	tf.close()
	N = len(ft)
	cdfpair.append(sorted(map(lambda x: float(x.split(' ')[1])/1000.0,ft)))

	i+=1

cdfpair.append(makecdf(N))


fd = open("output",'w')
for i in range(N):
	print >> fd, "%0.5f %0.5f %0.5f %0.5f" % (cdfpair[0][i],cdfpair[1][i],cdfpair[2][i],cdfpair[3][i])
fd.close()

os.system("gnuplot cdfplot.plt")