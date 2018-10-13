import numpy as np 
import matplotlib.pyplot as plt 

with open('timings.txt','r') as inp:
	inp.readline()
	times = np.loadtxt(inp, delimiter=',')
	print(times.shape)

selected = list([0,1,3,8,13,18])
# plt.plot((2+np.array(range(19))),times[:,0],'r^-',label="Best first search algorithm")
# plt.plot((2+np.array(range(19))),times[:,1],'bd-',label="Sequential scan algorithm")
plt.plot((2+np.array(range(19)))[selected],times[:,7][selected],'bd-',label="Locality Sensitive Hashing (99% dist prec)")
# plt.plot((2+np.array(range(19)))[selected],times[:,1][selected],'bd-',label="Sequential scan algorithm")
plt.title("Average Query Time vs Dimension")
plt.xlabel('Dimension')
plt.ylabel('Average Time for 100NN query(in sec)')
# plt.ylim([0,0.01])
plt.legend()
plt.grid()
plt.show()


# import numpy as np 
# import matplotlib.pyplot as plt 

# with open('timings.txt','r') as inp:
# 	inp.readline()
# 	times = np.loadtxt(inp, delimiter=',')
# 	print(times.shape)

# selected = list([0,1,3,8,13,18])
# # plt.plot((2+np.array(range(19))),times[:,0],'r^-',label="Best first search algorithm")
# # plt.plot((2+np.array(range(19))),times[:,1],'bd-',label="Sequential scan algorithm")
# # plt.plot((2+np.array(range(19)))[selected],(times[:,1]/times[:,7])[selected],'r^-',label="Speedup")
# # plt.plot((2+np.array(range(19)))[selected],times[:,7][selected],'bd-',label="LSH (95% dist_prec)")
# plt.title("Average query time ratio(seq_scan/lsh(99% dist_prec)) vs Dimension")
# plt.xlabel('Dimension')
# plt.ylabel('Ratio of Average times for 100NN query(in sec)')
# # plt.ylim([0,0.01])
# plt.legend()
# plt.grid()
# plt.show()