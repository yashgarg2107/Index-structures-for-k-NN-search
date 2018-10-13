-> Description of the plots : all dimensions = [2:20], selected dimensions = [2,3,5,10,15,20]

1) bfs_time.png : average time per query point for 20NN query with best first search algorithm for kd-tree

2) seq_time.png : average time per query point for 20NN query with sequential scan algorithm

3) lsh95_time.png : average time per query point for 100NN query with locality sensitive hashing with parameters tuned to obtain 95% precision

4) lsh99_time.png : average time per query point for 100NN query with locality sensitive hashing with parameters tuned to obtain 1% average distance deviation between dist of query pt. to exact kNNs to that with approx. kNNs

5) cons_a_b.png : comparison of construction(pre-processing)(indexing) time of method a with method b for 100,000 points for different dimensions

6) quer_a_b.png : comparison of 100NN average query time using method a vs method b for selected dimensions

7) speedup_a.png : ratio of avg query time for 100NN using method a with that by using kd-tree best first search algorithm.

8) speedup_seq_lsh : speedup of querying time comparison between sequential scan and lsh (for 99% dist precision) 

9) dist_ratio_1-2.png : plot of ratios of distance of query point to 2nd NN and 100th NN for selected and all dims. Shows the reason of curse of dimensionality in higher dimension spaces
____________________________________________________________________________________________________________________________

-> Description of the data files

1) query_time.txt : average time per query point for 20NN query for kd tree and sequential scan for all dims

2) dist_ratio.txt : distances of query point to its 2nd NN and 100th NN, when dataset contains 100,000 points for all dims

3) timings.txt : contains important indexing and querying times for sequential scan, kdtree's bfs, and lsh's hamming bit pojection algorithms.  _con => construction time (prep) for 100000 points, _quer => average time for 100NN query per query point, 1 => represent lsh model to obtain 95% precision in identified knn, 2 => represent lsh model to obtain 1% average deviation from query points to the found approx. knn than the actual knn. 'timings.ods' : spreadsheet file for the same
___________________________________________________________________________________________________________________________

-> Competition part - used kd tree's best first search and then used sequential scan kind of approach from 12 dimension onwards to achieve fastest implementaion