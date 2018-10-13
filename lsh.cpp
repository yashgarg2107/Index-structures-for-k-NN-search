#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <queue>
#include <vector>
#include <cmath>
#include <fstream>
#include <iterator>
#include <stack>
#include <cstdlib>
#include <queue>
#include <unordered_set>  // compile with -std=c++11 flag

#define vect vector
#define add push_back 

using namespace std;

int dim = 0, num = 0;						
int H, L;									// H = no. of hash codes ; L = no. of tables ;

vect<pair<vect<double>, double> > points;					
vect<double> query;

vect<vect<int> > hamming_bin;				// which bin it hashed to in respective dim dimensions
vect<vect<vect<int> > > final_proj;			// stores indexes of point which hash to this bucket


double eucl_dist(vect<double> &point)
{
	double dist = 0;
	
	for(int i=0;i<dim;i++)
		dist += (point[i]-query[i])*(point[i]-query[i]);
	
	return dist;
}

struct dist_compare						// for priority queue of answer set
{
	bool operator()(pair<vect<double>, double> *a, pair<vect<double>, double> *b)	
	{
		if(a->second < b->second)		// need to keep max distant point at the top of answer set
			return true;				// max heap logic comparator 
		return false;
	}
};

void find_knn(vect<vect<int> > &hash_codes, int bins, int hdim, int mL, int knn)
{
	vect<int> temp(dim);

	for(int i=0;i<dim;i++)
		temp[i] = query[i]*bins;	// hamming bin projection

	int quot, rem, buck=0, flag=0;
	unordered_set<int> pts;			// ml+K collided points

	for(int i=0;i<L;i++)
	{
		buck = 0;
		for(int k=0;k<H;k++)
		{
			quot = hash_codes[i][k]/bins;
			rem = hash_codes[i][k]%bins;

			if(temp[quot] > rem)	// converting to binary number to index the bucket
				buck = buck*2+1;
			else
				buck = buck*2;
		}

		for(int j=0;j<final_proj[i][buck].size();j++)
		{
			pts.insert(final_proj[i][buck][j]);		// Adding mL+k distinct points to the search set
			if(pts.size()>mL+knn)
			{
				flag=1;
				break;
			}
		}

		if(flag==1)
			break;
	}

	priority_queue<pair<vect<double>, double>*, vect<pair<vect<double>, double>* >, dist_compare> pq;
	unordered_set<int> :: iterator itr;

	for (itr = pts.begin(); itr != pts.end(); itr++)
	{
		if(pq.size()!=knn)
        {
        	points[*itr].second = eucl_dist(points[*itr].first);
        	pq.push(&points[*itr]);
        }
        else
        {
        	points[*itr].second = eucl_dist(points[*itr].first);
        	if(points[*itr].second < pq.top()->second)
        	{
        		pq.pop();
        		pq.push(&points[*itr]);		// remember, queue of pointers, hence pass address
        	}
        }
	}

	vect<double> *t;
	stack<vect<double> *> st;				

	while(!pq.empty())		// push to stack, to get least distant point at the top
	{
		st.push(&(pq.top()->first));
		pq.pop();
	}

	while(!st.empty())
	{
		t = st.top();
		st.pop();

		for(int i=0;i<dim;i++)
			cout<<(*t)[i]<<" ";
		cout<<endl;
	}       
}

void final_projection(vect<vect<int> > &hash_codes, int bins, int hdim)
{
	int buck = 0, quot, rem;
	
	for(int i=0;i<L;i++)
	{
		vect<vect<int> > final_temp(hdim);		// each table Li will have a 2d vector of length hdim(no. of buckets)
												// won't work if defined outside, as new values wil be written over prev values(here used add intead of assignment)
		for(int j=0;j<num;j++)
		{
			buck = 0;
			for(int k=0;k<H;k++)
			{
				quot = hash_codes[i][k]/bins;	// converting to hash bits in runtime - efficiency
				rem = hash_codes[i][k]%bins;	// quot and rem decide from where to take the hashcode

				if(hamming_bin[j][quot] > rem)	// converting to binary number to index the bucket
					buck = buck*2+1;
				else
					buck = buck*2;
			}

			final_temp[buck].add(j);
		}

		final_proj.add(final_temp);
	}


	for(int i=0;i<L;i++)
	{
		cout<<"-----------"<<endl;
		for(int j=0;j<hdim;j++)
		{
			cout<<j<<" -> ";
			
			for(int k=0;k<final_proj[i][j].size();k++)
				cout<<final_proj[i][j][k]<<" ";
			
			cout<<endl;
		}
		cout<<endl;
	}
}

void project_hamming(int bins)
{
	vect<int> temp(dim);

	for(int i=0;i<num;i++)
	{
		for(int j=0;j<dim;j++)
			temp[j] = points[i].first[j]*bins;		// projects to hamming space bins

		hamming_bin.add(temp);
	}
}

void print_points()
{
	cout<<dim<<" "<<num<<endl;

	for(int i=0;i<num;i++)
	{
		for(int j=0;j<dim;j++)
			cout<<points[i].first[j]<<" "<<hamming_bin[i][j]<<" ";
		
		cout<<endl;
	}
	cout<<endl;
}

void store_points(char dataset_file[])
{
	freopen(dataset_file, "r", stdin);
	cin>>dim>>num;

	vect<double> temp(dim);		// single declaration outside loop, 
								// prevents overhead of making and collecting(freeing space) vectors 
	for(int i=0;i<num;i++)
	{
		for(int j=0;j<dim;j++)
			cin>>temp[j];

		points.add(make_pair(temp,0));	// index stored for sorting use later
	}
}

int main(int argc, char* argv[])
{
	char dataset_file[] = "2_dim_datas.txt";

	store_points(dataset_file);

	int bins = 8;
	int hbits = (bins-1)*dim;

	H = 4;
	L = 4;
	
	int hdim = pow(2,H);

	project_hamming(bins);
	print_points();

	vect<vect<int> > hash_codes(L);
	vect<int> temp(H);

	for(int i=0;i<L;i++)
	{
		for(int j=0;j<H;j++)
		{
			temp[j] = rand()%hbits;
			cout<<temp[j]<<" ";
		}
		cout<<endl;
		hash_codes[i] = temp;
	}
	
	final_projection(hash_codes, bins-1, hdim);

	query.add(0.5);
	query.add(0.5);

	find_knn(hash_codes, bins-1, hdim, 2*L, 4);

	cout<<endl;

	return 0;
}
