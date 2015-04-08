#include "kmeans.h"

bool lessmark(const row_data &s1, const row_data &s2)  
{  
	return s1.row < s2.row;  
};

bool greatermark(const row_data &s1, const row_data &s2)  
{  
	return s1.row > s2.row;  
}; 

void print_Tuple(Tuple tuple)
{
	cout<<"Tuple's number of element = "<<tuple.num_elem<<endl;
	for(int i = 0; i<5;i++)
	{
		cout<<tuple.data[i]<<" ";
	}
	cout<<endl;
	cout<<"Tuple.attr1 = "<<tuple.attr1<<endl;
	cout<<"Tuple.attr2 = "<<tuple.attr2<<endl;
	cout<<"================"<<endl;
}

void init_Tuple(Tuple &tuple, int num_element)
{
	tuple.num_elem = num_element;
	tuple.data = new double[num_element];
	for(int i = 0; i<num_element;i++)
	{
		tuple.data[i] = 0.0;
	}
	tuple.attr1 = 0.0;
	tuple.attr2 = 0.0;
};

void readin_data(Tuple &tuple, int num, CvMat* &Matdata)
{
	tuple.attr1 = 0;
	tuple.attr2 = 0;
	//tuple.num_elem = num;
	//tuple.data = new double[num];

	for(int i = 0; i<num; i++)
	{
		tuple.data[i] = cvGet2D(Matdata,floor(i/Matdata->cols),(i%Matdata->cols)).val[0];
	}

	return;
};

void readout_data(Tuple &tuple, int frameH, int frameW)
{
	IplImage* img = cvCreateImage(cvSize(frameW,frameH),IPL_DEPTH_8U,1);
	int imgwidth = img->widthStep;
	uchar* imgdata = (uchar*) img->imageData;

	for(int i = 0; i<frameH; i++)
	{
		for(int j = 0; j<frameW; j++)
		{
			imgdata[i*imgwidth+j] = max(0,min(255,(int)tuple.data[i*frameW+j]));
		}
	}

	cvNamedWindow("test");
	cvShowImage("test",img);
	cvWaitKey(0);
	cvDestroyWindow("test");
	cvReleaseImage(&img);

	return;
};

void delete_tuple(Tuple &tuple)
{
	if(tuple.num_elem)
	{
		tuple.num_elem = 0;
		delete [] tuple.data;	
	}
	tuple.data = NULL;
};

double getDistXY(Tuple t1, Tuple t2)   
{  
	if(t1.num_elem != t2.num_elem)
	{
		printf("ERROR: The dimensions of two 2 tuples do not agree!\n");
		return 0.0;
	}
	else
	{
		double sum_dist = 0.0;
		for(int i = 0; i<t1.num_elem; i++)
		{
			sum_dist += (t1.data[i] - t2.data[i])*(t1.data[i] - t2.data[i]);
		}
		return sqrt(sum_dist);
	}
};

//根据质心，决定当前元组属于哪个簇  
int clusterOfTuple(Tuple means[],Tuple tuple, int num_cluster)
{  
	double dist=getDistXY(means[0],tuple);  
	double tmp;  
	int label=0;//标示属于哪一个簇  
	for(int i=1;i<num_cluster;i++){  
		tmp=getDistXY(means[i],tuple);  
		if(tmp<dist) {dist=tmp;label=i;}  
	}  
	return label;     
};

//获得给定簇集的平方误差  
double getVar(Tuple* &tuples,Tuple means[], int num_cluster, vector<int>* labels)
{  
	double var = 0;  
	for (int i = 0; i < num_cluster; i++)  
	{  
		vector<int> t = labels[i];  
		for (int j = 0; j< t.size(); j++)  
		{  
			var += getDistXY(tuples[t[j]],means[i]);  
		}  
	}  
	//cout<<"sum:"<<sum<<endl;  
	return var;  

};

//获得当前簇的均值（质心）  
Tuple getMeans(vector<Tuple> cluster)
{  

	int num = cluster.size(); 
	int num_element = cluster[0].num_elem;
	
	double* means = new double[num_element];

	Tuple t;  
	init_Tuple(t,num_element);

	for (int i = 0; i < num_element; i++)  
	{
		means[i] = 0.0;

		for(int j = 0; j<num; j++)
		{
			means[i] += cluster[j].data[i];
		}

		t.data[i] = means[i] / num;
	}  
	t.attr1 = 0;  
	t.attr2 = 0;  
	delete [] means;
	return t;  
	//cout<<"sum:"<<sum<<endl;  
};  

// 计算质心
void cal_mean(Tuple* &tuples, Tuple & mean_tuple, vector<int> labels)
{
	if(labels.size() == 0)
	{
		printf("ERROR: none element in cluster!\n");
		Tuple mean_tuple;
		init_Tuple(mean_tuple,0);
		return;
	}
	else
	{
		//Tuple mean_tuple;
		//init_Tuple(mean_tuple,tuples[0].num_elem);
		for(int j = 0; j<labels.size(); j++)
		{
			mean_tuple.attr1 += tuples[labels[j]].attr1/labels.size();
			mean_tuple.attr2 += tuples[labels[j]].attr2/labels.size();
			for(int i = 0; i<tuples[0].num_elem; i++)
			{
				//cout<<"i = "<<i<<endl;
				mean_tuple.data[i] += tuples[labels[j]].data[i]/labels.size();
			}
		}
		return;
	}
};

vector<int>* update_clusters(Tuple* &tuples, Tuple means[], int num_cluster, vector<int> *labels)
{
	int i, j, k, N_size, N_size_k;
	row_data row_min, row_current;
	//vector<Tuple>::iterator Iter;
	vector<int>* label_new = new vector<int>[num_cluster];

	for(j = 0; j<num_cluster;j++)
	{
		int delete_num = 0;
		N_size = labels[j].size();
		if(N_size <= 1)
		{
			label_new[j].push_back(labels[j][0]);
			continue;
		}
		else
		{
			vector<int> temp = labels[j];
			for(i = 0; i<N_size; i++)
			{
				for(k = 0; k<num_cluster;k++)
				{
					if(j != k)
					{
						N_size_k = labels[k].size();
						//print_Tuple(temp[i]);
						//print_Tuple(means[k]);
						row_current.row = double(N_size_k)/double(N_size_k+1)*getDistXY(tuples[temp[i]],means[k])*getDistXY(tuples[temp[i]],means[k]);
						row_current.label = k;
						//cout<<"row_current.row = "<<row_current.row<<", N_size_k = "<<N_size_k<<endl;
					}
					else
					{
						N_size_k = labels[k].size();
						//print_Tuple(temp[i]);
						//print_Tuple(means[k]);
						row_current.row = double(N_size_k)/double(N_size_k - 1)*getDistXY(tuples[temp[i]],means[k])*getDistXY(tuples[temp[i]],means[k]);
						row_current.label = k;
						//cout<<"row_current.row = "<<row_current.row<<", N_size_k = "<<N_size_k<<endl;
					}

					if(k == 0)
					{
						row_min.row = row_current.row;
						row_min.label = row_current.label;
					}
					else
					{
						if(row_current.row < row_min.row)
						{
							row_min.row = row_current.row;
							row_min.label = row_current.label;
						}
					}
				}
				if(j != row_min.label) {delete_num++;}
				if(delete_num != N_size)
				{
					label_new[row_min.label].push_back(labels[j][i]);
					//cout<<"cluster "<<j<<", tuple "<<i<<", new label = "<<row_min.label<<endl;
					//print_Tuple(temp[i]);
				}
				else
				{
					label_new[j].push_back(labels[j][i]);
					//cout<<"cluster "<<j<<", tuple "<<i<<", new label = "<<j<<endl;
					//print_Tuple(temp[i]);
				}
			}
		}
	}

	return label_new;
};

vector<Tuple> * reorder_Tuple(vector<Tuple>* clusters, vector<int>* labels, int num_cluster)
{
	vector<Tuple>* new_clusters = new vector<Tuple>[num_cluster];
	int i,j;
	for(i = 0; i<num_cluster; i++)
	{
		for(j = 0; j<clusters[i].size(); j++)
		{
			//cout<<"cluster "<<i<<" label "<<labels[i][j]<<endl;
			new_clusters[labels[i][j]].push_back(clusters[i][j]);
		}
	}

	return new_clusters;
};

// kMeans 算法实现
vector<int>* KMeans(Tuple* &tuples, int num_cluster, double thres_diff, int length)
{  
	vector<int> * labels = new vector<int> [num_cluster];
	vector<int>* labels_new = new vector<int> [num_cluster];
	Tuple* means = new Tuple[num_cluster];  
	int i=0; int j = 0;  

	int lable=0;
	int init_length = floor(length/num_cluster);
	int num_element = tuples[0].num_elem;

	//根据默认的质心给簇赋值  
	for(i=0;i!=length;++i)
	{  
		lable = floor(i/init_length) > (num_cluster-1) ? (num_cluster-1) : floor(i/init_length); 
		labels[lable].push_back(i);
	}   

	//默认一开始将前K个元组的值作为k个簇的质心（均值）  
	for(i=0;i<num_cluster;i++)
	{ 
		init_Tuple(means[i],num_element);
		cal_mean(tuples, means[i],labels[i]);	
		print_Tuple(means[i]);
	}  

	double oldVar=-1;  
	double newVar=getVar(tuples,means,num_cluster,labels);  
	int count = 0;

	//当新旧函数值相差不到1即准则函数值不发生明显变化时，算法终止
	while(abs(newVar - oldVar) >= thres_diff && count < THRES_ITERATION_TIMES)   
	{  
		++count;

		//更新标签
		labels_new = update_clusters(tuples,means,num_cluster,labels);
		cout<<"Iteration times = "<<count<<", new labels: "<<endl;
		for(i = 0; i<num_cluster; i++)
		{
			cout<<"Cluster "<<i<<", size = "<<labels_new[i].size()<<endl;
			//for(j = 0; j<labels_new[i].size(); j++)
			//{
			//	cout<<labels_new[i][j]<<" ";
			//}
			//cout<<endl;
		}

		//重新计算质心
		for(i=0;i<num_cluster;i++)
		{
			delete_tuple(means[i]);
			init_Tuple(means[i],num_element);
			cal_mean(tuples,means[i],labels_new[i]);	
			//print_Tuple(means[i]);
		}  

		oldVar = newVar;  
		newVar = getVar(tuples,means,num_cluster,labels_new); //计算新的准则函数值  

		//更新标签
		labels = labels_new;

		cout<<"Diff = "<<newVar - oldVar<<endl;
	}  

	cout<<"Iteration times = "<<count<<endl;

	for(i = 0; i<num_cluster;i++)
	{
		delete_tuple(means[i]);
	}
	
	//for(i = 0; i<num_cluster;i++)
	//{
	//	labels_new[i].clear();
	//}
	//delete [] labels_new;
	
	return labels;

};