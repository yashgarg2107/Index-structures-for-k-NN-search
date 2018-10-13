#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <queue>
#include <vector>
#include <cmath>
#include <fstream>
#include <iterator>
#include <stack>
#include <time.h>
#include <cstdlib>
#include <queue>
#include <unordered_set>  // compile with -std=c++11 flag
#include <unordered_map>

#define vect vector
#define add push_back 

using namespace std;

int dim = 0, num = 0, knn=0;						
int H=20, L=61, m=75, bins=100, mL;			// the parameters of lsh model	
int ran = 12;
//double p1=0.7, p2=0.3;						

vect<pair<vect<double>, double> > points;					
vect<double> query;

vect<vect<int> > hamming_bin;								// which bin it hashed to in respective dim dimensions
vect<unordered_map<int, vect<int> > > final_proj;			// stores indexes of point which hash to this bucket
vect<vect<int> > hash_codes;								// making these global, to avoid passing them to every query


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

void find_knn(FILE *fp)
{
	int bin = bins-1; 				// for mathematical use
	vect<int> temp(dim);

	for(int i=0;i<dim;i++)
		temp[i] = query[i]*bins;	// hamming bin projection

	int quot, rem, buck=0, flag=0;
	unordered_set<int> pts;			// ml+K collided points
	int count = 0;

	for(int i=0;i<L;i++)
	{
		buck = 0;
		for(int k=0;k<H;k++)
		{
			quot = hash_codes[i][k]/bin;
			rem = hash_codes[i][k]%bin;

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

	pair<vect<double>, double>*v;
	stack<pair<vect<double>, double>* > st;				

	while(!pq.empty())		// push to stack, to get least distant point at the top
	{
		st.push(pq.top());
		pq.pop();
	}


	while(!st.empty())
	{
		v = st.top();
		st.pop();

		fprintf(fp,"%lf",v->first[0]);		// done for evaluation by python script
		for(int i=1;i<dim;i++)
		{
			fprintf(fp," %lf",v->first[i]);	// to remove last space in line
		}
		fprintf(fp,"\n");
	}   
}

void perform_search(string query_file)
{
	FILE *fp;
	int quer;
	
	fp = fopen(query_file.c_str(), "r");
	int ret = fscanf(fp,"%d %d",&dim,&quer);

	vect<vect<double> > queries;
	vect<double> q(dim);

	for(int i=0;i<quer;i++)
	{
		for(int j=0;j<dim;j++)
			ret = fscanf(fp,"%lf",&q[j]);
		queries.add(q);
	}

	fclose(fp);

	char result_file[] = "results_lsh.txt";
	fp=fopen(result_file, "w+");

	for(int i=0;i<quer;i++)
	{
		query = queries[i];
		find_knn(fp);
	}

	fclose(fp);
}

void final_projection()
{
	int buck = 0, quot, rem;
	int bin = bins-1;
	
	for(int i=0;i<L;i++)
	{
		unordered_map<int, vect<int> > final_temp;	// each table Li will have a hashmap (not vector as hdim can be very high)
													// won't work if defined outside, as new values wil be written over prev values(here used add intead of assignment)
		for(int j=0;j<num;j++)
		{
			buck = 0;
			for(int k=0;k<H;k++)
			{
				quot = hash_codes[i][k]/bin;	// converting to hash bits in runtime - efficiency
				rem = hash_codes[i][k]%bin;	// quot and rem decide from where to take the hashcode

				if(hamming_bin[j][quot] > rem)	// converting to binary number to index the bucket
					buck = buck*2+1;
				else
					buck = buck*2;
			}

			final_temp[buck].add(j);
		}

		final_proj.add(final_temp);
	}
}

void project_hamming()
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
}

void store_points(string dataset_file)
{
	FILE *fp;
	fp = fopen(dataset_file.c_str(), "r");

	int ret = fscanf(fp,"%d %d",&dim,&num);

	vect<double> temp(dim);		// single declaration outside loop, 
								// prevents overhead of making and collecting(freeing space) vectors 
	for(int i=0;i<num;i++)
	{
		for(int j=0;j<dim;j++)
		{
			ret = fscanf(fp,"%lf",&temp[j]);
		}

		points.add(make_pair(temp,0));	// index stored for sorting use later
	}

	fclose(fp);
}

int main(int argc, char* argv[])
{
	string dataset_file = argv[1];
	// ran = 1531078570;
	// H=20;
	// if(atoi(argv[2])==1)
	// {
	// 	H=23;
	// 	L=360;
	// 	m=60;
	// }
	// else if(atoi(argv[2])==2)
	// {
	// 	L=98;
	// 	m=98;
	// }

	srand(time(0)); // for randomness
	store_points(dataset_file);

	//double rho = log2(p1)/log2(p2);	
	//cerr<<num<<" "<<rho<<" ";	
	//H = log2(num)/log2(1/p2);				// H = no. of hash codes;
	//L = pow(num, rho);					// L = no. of tables;	

	//H = 20;
	//L = 200;
	
	int hbits = (bins-1)*dim;

	project_hamming();
	//print_points();

	vect<int> temp(H);

	for(int i=0;i<L;i++)		// choosing H random bits for L tables
	{
		for(int j=0;j<H;j++)
			temp[j] = rand()%hbits;
		hash_codes.add(temp);
	}
	
	final_projection();		// constructing table (storing the data points in buckets)
	cout<<0<<endl;						// printing 0 for flag for python eval file

	string query_file;
	cin>>query_file;
	cin>>knn;

	mL = m*L;

	perform_search(query_file);
	cout<<1<<endl;

	return 0;
}
