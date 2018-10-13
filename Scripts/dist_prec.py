import numpy as np 

lsh = np.loadtxt("results_lsh.txt",delimiter=' ')
seq = np.loadtxt("results_seq.txt",delimiter=' ')

with open("20_dim_query.txt","r") as fp:
	fp.readline()
	query = np.loadtxt(fp, delimiter=' ')

dist_lsh = 0
dist_seq = 0

for i in range(100):
	for j in range(100):
		dist_lsh+=np.sqrt(np.sum(np.square(query[i]-lsh[i*100+j])))
		dist_seq+=np.sqrt(np.sum(np.square(query[i]-seq[i*100+j])))

print(dist_lsh/100)
print(dist_seq/100)

print()
print(100-100*(dist_lsh-dist_seq)/dist_seq)