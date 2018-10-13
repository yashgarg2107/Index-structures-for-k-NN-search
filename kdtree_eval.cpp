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

vect<pair<int,vect<double> > > points;		// pair with int is used to store only 
											// indexes after sorting wrt different dimensions
vect<double> query; 						// query point // made global, to avoid passing many times
//double arr[100] = {0};

struct node
{
	int split_dim;
	double edist, mdist;	 	// euclidean and mbr distance to query pt
	vect<double> point;		// stores the coordinates of point
	vect<double> min, max;	// to store diagonal points of mbr of pt
	node *left, *right;

	node(vect<double> pt, int d)
	{
		point = pt;
		split_dim = d;
	}

	void set_mbr(pair<vect<double>, vect<double> > &pr)
	{
		min = pr.first;
		max = pr.second;
	}
};

double eucl_dist(vect<double> &a)
{
	double dist = 0.0;
	for(int i=0;i<dim;i++)
		dist += (a[i]-query[i])*(a[i]-query[i]);	
	
	return sqrt(dist);
}

double mbr_dist(vect<double> &min, vect<double> &max)
{
	double dist = 0.0;
	double x;
	for(int i=0;i<dim;i++)
	{
		if(query[i]<min[i])
		{
			x = min[i]-query[i];
			dist += x*x;
		}
		else if(query[i]>max[i])
		{
			x = query[i]-max[i];
			dist += x*x;		// else dist same
		}
	}
	return sqrt(dist);	// minimum dist to bounding rectangle of a pt
}


class sort_compare	// comparator class made to pass parameters like sorting dim
{
	int sd; 		// sorting dimension
public:
	sort_compare(int d) : sd(d) {}	// alternate syntax for constructor

	bool operator()(pair<int, vect<double> > &a, pair<int,vect<double> >&b)
	{
		vect<double> *temp1 = &(a.second);
		vect<double> *temp2 = &(b.second);

		for(int j=0;j<dim;j++)
		{
			int i = (sd+j)%dim;		// actuall next index

			double x = (*temp1)[i];
			double y = (*temp2)[i];

			if(x<y)
				return true;
			else if(x>y)
				return false;	
		}
		return true;
	}
};

struct eucl_compare						// for priority queue of answer set
{
	bool operator()(node *a, node *b)	// need to use * and ->, for operator function (cannot use & to pass by ref)	
	{
		if(a->edist < b->edist)			// need to keep max distant point at the top of answer set
			return true;				// max heap logic comparator (to remember - think of normal sorting - looking from right)
		return false;
	}
};

struct mbr_compare						// for priority queue of candidate mbrs
{
	bool operator()(node *a, node *b)
	{
		if(a->mdist > b->mdist)			// need to keep min distant mbr at the top of candidate set
			return true;				// min heap logic comparator
		return false;
	}
};


class kdtree
{
	node* root;
	vect<vect<int> > sorted;	// will store the indices of pre-sorted 
								// points arrangement wrt different dimensions
public:
	kdtree()
	{
		vect<int> temp(num);

		for(int i=0;i<dim;i++)
			sorted.add(temp);

		pre_sort();
		root = construct_tree(0,sorted);
		sorted.clear();

		//print_tree(root);
	}

	void pre_sort()
	{
		vect<pair<int, vect<double> > > temp;
		copy(points.begin(), points.end(), back_inserter(temp));	
		// back_inserter present in iterator header, copy in algorithm header

		for(int i=0;i<dim;i++)					// sort wrt each dimension
		{
			sort(temp.begin(), temp.end(), sort_compare(i));

			for(int j=0;j<num;j++)				// store the sorted indices in sorted vector
			{
				sorted[i][j] = temp[j].first;
			}
		}

		// for(int i=0;i<dim;i++)
		// {
		// 	for(int j=0;j<num;j++)
		// 	{
		// 		cout<<sorted[i][j]<<" ";
		// 	}
		// 	cout<<endl;
		// }
	}

	pair<vect<double>, vect<double> > find_mbr(vect<int> &pts)
	{
		vect<double> min_vec, max_vec;
		double min, max, temp; 

		for(int i=0;i<dim;i++)					// for all dim calculate min and max coordinate
		{
			min = points[pts[0]].second[i];
			max = min;							// initialising for pts first point for respective dim
			
			for(int j=1;j<pts.size();j++)
			{
				temp = points[pts[j]].second[i];
				if(temp > max)
					max = temp;
				if(temp < min)
					min = temp;
			}
			min_vec.add(min);
			max_vec.add(max);
		}
		return make_pair(min_vec, max_vec);
	}

	node* construct_tree(int sd, vect<vect<int> >&div_pts)
	{
		if(div_pts[0].size()==0)		// even when nothing added(last left/right), .size()=2, 
			return NULL;				// so use [0].size to get correct base case
			
		sd = (sd % dim);
		int idx = div_pts[sd][div_pts[0].size()/2];		// median index point
		double val = points[idx].second[sd];			// value in splitting dim to compare with
		
		node* div = new node(points[idx].second, sd);					// node with respective median point
		pair<vect<double>, vect<double> > pr = find_mbr(div_pts[0]);	// find mbr bound, can give any dim (only indexes needed)
		div->set_mbr(pr);								

		vect<vect<int> > left_pts, right_pts;			// need to make 2d vectors to store indices across all dims

		for(int i=0;i<dim;i++)			// eg: x axis: 2 4 0 3 1; y axis: 1 2 0 4 3; and split across x axis gives 0 idx
		{								// then need to have left_pts = {{2,4}{2,4}} and left_pts = {{3,1}{1,3}}
			vect<int> left, right;		// basically have to take sorted indices(across all dims) down in tree construction
			int flag = 0;

			for(int j=0;j<div_pts[0].size();j++)
			{
				int a = div_pts[i][j];
				if(a == idx && flag==0)	// flag used to avoid sending median index to a child
					flag=1;
				else
				{
					if(points[a].second[sd] <= val)
						left.add(a);
					else
						right.add(a);
				}
			}

			left_pts.add(left);			// adding left and right indices across
			right_pts.add(right);		// one dim to successive div_points 2d vectors
		}

		div->left = construct_tree(sd+1, left_pts);		// recurively construct
		div->right = construct_tree(sd+1, right_pts);		// left & right trees
		
		left_pts.clear();				// free up memory - O(nd) memory extra needed only
		right_pts.clear();
										// total complexity of a step is O(dim*num(div_points))
		return div;						// total complexity of construction is O(d*n*log(n))	
	}

	// Best First Search Algorithm
	// Algorithm used:
		// 1) Pop top MBR from Candidate
		// 2) If MBR is further to query than the top node in answer set, return(break)
		// 3) Else
		//		i) If MBR is a data pt and closer to query than the top node in ans_set, insert in ans_set(keeping k nodes)
		//		ii) Insert those children of MBR (mbr distances comparison) that are closer to quer than top node in ans_set
		// 4) Iterate as long as Candidate is not empty or top MBR is candidate is further than the top pt in ans_set

	void knn_query(FILE *fp)					// k nearest neighbours to be found
	{
		int k = knn;
		priority_queue<node*, vect<node*>, eucl_compare> answer_set;		// template < class T, class Container = vector<T>,
		priority_queue<node*, vect<node*>, mbr_compare> candidate_mbrs;	    // class Compare = less<typename Container::value_type> >
																			// template definition for stl priority queue
		candidate_mbrs.push(root);

		node *temp;

		while(answer_set.size()!=k)		// first populate the answer set with k elements
		{
			temp = candidate_mbrs.top();
			candidate_mbrs.pop();

			temp->edist = eucl_dist(temp->point);	// calculate distances as and when required, otherwise
													// if pre-computed mdist and edist, complexity is O(n) here already
			answer_set.push(temp);

			if(temp->left!=NULL)
			{
				temp->left->mdist = mbr_dist(temp->left->min, temp->left->max);
				candidate_mbrs.push(temp->left);
			}

			if(temp->right!=NULL)
			{
				temp->right->mdist = mbr_dist(temp->right->min, temp->right->max);
				candidate_mbrs.push(temp->right);
			}
		}		
		// intentially added k points in the beginning to get good speedup, otherwise

		double dis,x;									// dis and x are used to provide some optimization, 
														// as no need to reference again and again(which may be costly)
		while(!candidate_mbrs.empty())
		{
			temp = candidate_mbrs.top();
			dis = answer_set.top()->edist;
			candidate_mbrs.pop();

			if(temp->mdist > dis)					// as all below it in queue will be even farther
				break;

			else // temp->mbr_dist <= temp2->edist
			{
				x = eucl_dist(temp->point);
				if(x < dis)
				{
					temp->edist = x;
					answer_set.pop();					// answer set should contain k points at any time
					answer_set.push(temp);
				}

				dis = answer_set.top()->edist;

				if(temp->left!=NULL)
				{
					x = mbr_dist(temp->left->min, temp->left->max);
					if(x<dis)
					{
						temp->left->mdist = x;
						candidate_mbrs.push(temp->left);
					}
				}

				if(temp->right!=NULL)
				{
					x = mbr_dist(temp->right->min, temp->right->max);
					if(x<dis)
					{
						temp->right->mdist = x;
						candidate_mbrs.push(temp->right);
					}
				}
			}
		}

		node *v;
		stack<node*> st;

		while(!answer_set.empty())		// push to stack, to get least distant point at the top
		{
			st.push(answer_set.top());
			answer_set.pop();
		}

		int idx = 0;

		while(!st.empty())
		{
			v = st.top();
			st.pop();

			fprintf(fp,"%lf",v->point[0]);
			for(int i=1;i<dim;i++)
			{
				fprintf(fp," %lf",v->point[i]);
			}
			fprintf(fp,"\n");

			//arr[idx++] += (v->edist)/100;
		}
	}

	void print_tree(node* n)	// inorder traversal
	{
		if(n==NULL)
			return;
		
		print_tree(n->left);

		for(int i=0;i<dim;i++)
			cout<<(n->point[i])<<" ";

		cout<<endl;

		print_tree(n->right);
	}
};

void perform_search(string query_file, kdtree &kt)
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

	char result_file[] = "results_kdt.txt";
	fp=fopen(result_file, "w+");

	for(int i=0;i<quer;i++)
	{
		query = queries[i];
		kt.knn_query(fp);
	}

	fclose(fp);
}

void print_points()
{
	cout<<dim<<" "<<num<<endl;

	for(int i=0;i<num;i++)
	{
		for(int j=0;j<dim;j++)
			cout<<points[i].second[j]<<" ";
		
		cout<<endl;
	}
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

		points.add(make_pair(i,temp));	// index stored for sorting use later
	}

	fclose(fp);
}


int main(int argc, char* argv[])
{
	string ab = argv[1];
	store_points(ab);		// stores dataset points in 2-d vectors
	//print_points();	

	kdtree kt;			// constructs kd tree
	cout<<0<<endl;		// printing 0 for flag for python eval file

	string query_file;
	cin>>query_file;
	cin>>knn;

	perform_search(query_file, kt);
	cout<<1<<endl;

	// cerr<<"Distance of 2nd closest = "<<arr[1]<<endl;
	// cerr<<"Distance of 100nd closest = "<<arr[99]<<endl;


	return 0;
}

// python3 parent.py dataset_2_1000000.txt query_2_1000000.txt 100 // for time evaluation