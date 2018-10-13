import numpy as np

dims = 1+np.array(range(20))
nums = 100000
quer = 100

for i in dims:
	mat = np.random.uniform(0.0,1.0,(nums,i))
	with open(str(i)+"_dim_data.txt","w+") as out:
		out.write(str(i)+" "+str(nums)+"\n")
		np.savetxt(out, mat, delimiter=" ",fmt='%.10f')

for i in dims:
	mat = np.random.uniform(0.0,1.0,(quer,i))
	with open(str(i)+"_dim_query.txt","w+") as out:
		out.write(str(i)+" "+str(quer)+"\n")
		np.savetxt(out, mat, delimiter=" ",fmt='%.10f')



"""
dims = 1+np.array(range(3))
nums = 20

for i in dims:
	mat = np.random.uniform(0.0,1.0,(nums,i))
	with open(str(i)+"_dim_data.txt","w+") as out:
		out.write(str(i)+" "+str(nums)+"\n")
		np.savetxt(out, mat, delimiter=" ",fmt='%.5f')


quer = 5

for i in dims:
	mat = np.random.uniform(0.0,1.0,(quer,i))
	with open(str(i)+"_dim_query.txt","w+") as out:
		out.write(str(i)+" "+str(quer)+"\n")
		np.savetxt(out, mat, delimiter=" ",fmt='%.5f')
"""