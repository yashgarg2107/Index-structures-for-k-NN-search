import numpy as np 
import matplotlib.pyplot as plt 

with open('dist_ratio.txt','r') as inp:
	inp.readline()
	dist = np.loadtxt(inp, delimiter=',', dtype=np.float)
	print(dist.shape)

selected = list([1,2,4,9,14,19])
plt.plot((1+np.array(range(25)))[selected],(dist[:,0]/dist[:,1])[selected],'m^-')
plt.title("Ratio (of 2nd and 100th closest pt) vs Dimension")
plt.xlabel('Dimension')
plt.ylabel('Ratio of average distances (2nd closestt/ 100th closest)')
# plt.xlim([0,21])
# plt.ylim([0,0.85])
plt.grid()
plt.show()