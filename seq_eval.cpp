// derived from code of kdtree and lsh(find_knn method) already implemented 

#include <iostream>
#include <stdlib.h>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <queue>
#include <vector>
#include <string>
#include <fstream>
#include <stack>

#define vect vector
#define add push_back 

using namespace std;

int dim = 0, num = 0, knn = 0;

vect<pair<vect<double>, double> > points;					
vect<double> query;
// double arr[100] = {0};

double eucl_dist(vect<double> &point)
{
	double dist = 0;
	
	for(int i=0;i<dim;i++)
		dist += (point[i]-query[i])*(point[i]-query[i]);
	
	return sqrt(dist);
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
	priority_queue<pair<vect<double>, double>*, vect<pair<vect<double>, double>* >, dist_compare> pq;

	for (int i=0;i<num;i++)
	{
		if(pq.size()!=knn)
        {
        	points[i].second = eucl_dist(points[i].first);
        	pq.push(&points[i]);
        }
        else
        {
        	points[i].second = eucl_dist(points[i].first);
        	if(points[i].second < pq.top()->second)			// have to maintain knn points in answer set
        	{
        		pq.pop();
        		pq.push(&points[i]);		// remember, queue of pointers, hence pass address
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

	int idx = 0;
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
		//arr[idx++] += (v->second)/100; 
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

	char result_file[] = "results_seq.txt";
	fp=fopen(result_file, "w+");

	for(int i=0;i<quer;i++)
	{
		query = queries[i];
		find_knn(fp);
	}

	fclose(fp);
}

void store_points(string dataset_file)
{
	FILE *fp;
	fp = fopen(dataset_file.c_str(), "r");		// fopen takes c_string , so covert to c type string or use char array

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
	string ab = argv[1];
	store_points(ab);		// stores dataset points in 2-d vectors
	//print_points();	

	cout<<0<<endl;		// printing 0 for flag for python eval file

	string query_file;
	cin>>query_file;
	cin>>knn;

	perform_search(query_file);
	cout<<1<<endl;

	// FILE *fp;
	// fp = fopen("dist_ratio.txt", "a");
	// fprintf(fp,"%lf, %lf\n",arr[1], arr[99]);
	// fclose(fp);

	return 0;
}
