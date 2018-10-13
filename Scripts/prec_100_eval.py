import numpy as np 

kdt = np.loadtxt("results_kdt.txt",delimiter=' ')
lsh = np.loadtxt("results_lsh.txt",delimiter=' ')
seq = np.loadtxt("results_seq.txt",delimiter=' ')

count=0
for i in range(100):
	for j in range(100):
		for k in range(100):
			if(np.array_equal(seq[i*100+j],lsh[i*100+k])):
				count+=1 
				break

print(count/100)