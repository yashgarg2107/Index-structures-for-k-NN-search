import numpy as np 
import matplotlib.pyplot as plt 

with open('query_time.txt','r') as inp:
	inp.readline()
	times = np.loadtxt(inp, delimiter=',')
	print(times.shape)

selected = list([1,2,4,9,14,19])
plt.plot((1+np.array(range(25)))[selected],times[:,0][selected],'r^-',label="Best first search algorithm")
# plt.plot((1+np.array(range(25)))[selected],times[:,1][selected],'bd-',label="Sequential scan algorithm")
plt.title("Average Query Time vs Dimension (kd-Tree)")
plt.xlabel('Dimension')
plt.ylabel('Average Time for 20NN query(in sec)')
# plt.ylim([0,0.01])
plt.legend()
plt.grid()
plt.show()