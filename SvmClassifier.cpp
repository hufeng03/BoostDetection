/**
 * @file   SvmClassifier.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:12:45 2010
 * 
 * @brief  implementation of SvmClassifiers
 * 
 * 
 */
#include "SvmClassifier.h"
#include <iostream>

//do not print during liblinear training
extern void (*liblinear_print_string) (const char*);
static void print_null(const char *s){}

int* SvmClassifier::m_target = NULL;
parameter* SvmClassifier::m_param = NULL;
problem* SvmClassifier::m_prob = NULL;
float* SvmClassifier::m_f = NULL;
float* SvmClassifier::m_v = NULL;
feature_node *SvmClassifier::m_x = NULL;

SvmClassifier::SvmClassifier(Features* F):WeakClassifier(F)
{
	m_model = NULL;
}

SvmClassifier::SvmClassifier()
{
	m_model = NULL;
}

SvmClassifier::~SvmClassifier()
{
	Clear();
}

SvmClassifier& SvmClassifier::operator=(const SvmClassifier & sc)
{
	WeakClassifier::operator=(sc);
	m_thresh = sc.m_thresh;
	m_a = sc.m_a;
	m_b = sc.m_b;
	save_model("./svm.txt",sc.m_model);
	m_model = load_model("./svm.txt");
	return *this;
}

float SvmClassifier::GetValue(float* fv)
{
	int i;
	double r;

	for(i=0;i<m_feature_size;i++)
	{
		m_x[i].index = i+1;
		m_x[i].value = fv[m_feature_index*m_feature_size+i];
	}
	m_x[m_feature_size].index = -1;
	predict_values(m_model,m_x,&r);

	return float(m_a*r+m_b);
}

void SvmClassifier::ReadFromFile(std::ifstream& f)
{
     static bool flag_init = false;
     f>>m_feature_index>>m_feature_size>>m_thresh>>m_a>>m_b;
     f.ignore(256,'\n');
     if(!flag_init)
     {
	  m_f = new float[m_feature_size];
	  m_x = new feature_node[m_feature_size+1];
	  flag_init = true;
     }
     std::ofstream fout;
     fout.open("./svm.txt");
     char fileline[1000];
     while(1)
     {
	  f.getline(fileline,999);
	  if(fileline[0] == '/' && fileline[1]=='/' && fileline[2]=='/')
	  {
	       break;
	  }
	  fout<<fileline<<std::endl;
     }
     fout.close();
     m_model = load_model("./svm.txt");
}

void SvmClassifier::WriteToFile(std::ofstream& f) const
{
	f<<m_feature_index<<" "<<m_feature_size<<" "<<m_thresh<<" "<<m_a<<" "<<m_b<<std::endl;
	char filename[100];
	save_model("./svm.txt",m_model);
	std::ifstream fin;
	char fileline[1000];
	fin.open("./svm.txt");
	while(!fin.eof())
	{
		fin.getline(fileline,999);
		f<<fileline<<std::endl;
	}
	f<<"//////////"<<std::endl;
	fin.close();
}

void SvmClassifier::Clear()
{
	if(m_model!=NULL)
	{
		destroy_model(m_model);
	}
}
/*
void SvmClassifier::InitSvm(DataSet* trainset)
{
	int i,j;
	m_param = new svm_parameter;
	m_param->svm_type = C_SVC;       //svm type
	m_param->kernel_type = LINEAR;   //kernel type
	m_param->degree = 3;             //not used in LINEAR
	m_param->gamma = 0;              //not used in LINEAR
	m_param->coef0 = 0;              //not used in LINEAR
	m_param->nu = 0.5;               //not used in C_SVC
	m_param->cache_size = 100;
	m_param->C = 1;                  //default 1
	m_param->eps = 0.001;            //default 0.001
	m_param->p = 0.1;                //not used in C-SVC
	m_param->shrinking = 1;          //use shrinking heuristics
	m_param->probability = 0;        //not to train a svc model for probability estimates
	m_param->nr_weight = 0;          //not biased between different classes
	m_param->weight_label = NULL;
	m_param->weight = NULL;
	
	m_prob = new svm_problem;
	m_prob->l = trainset->m_sample_num;
	m_prob->y = new double[trainset->m_sample_num];
	m_prob->x = new svm_node*[trainset->m_sample_num];
	m_prob->W = new double[trainset->m_sample_num];
	for(i=0;i<trainset->m_positive_num;i++)
	{
		m_prob->y[i] = 1;
	}
	for(i=trainset->m_positive_num;i<trainset->m_sample_num;i++)
	{
		m_prob->y[i] = -1;
	}
	for(i=0;i<trainset->m_sample_num;i++)
	{
		m_prob->x[i] = (svm_node*)malloc((trainset->m_feature_size+1)*sizeof(svm_node));
		for(j=0;j<trainset->m_feature_size;j++)
		{
			m_prob->x[i][j].index = j+1;
		}
		m_prob->x[i][trainset->m_feature_size].index = -1;
	}
}
*/

void SvmClassifier::InitSvm(DataSet* trainset)
{
     static bool flag_init = false;
     if(flag_init)
	  return;
     else
	  flag_init = true;
	if(trainset!=NULL)
	{
	m_param = new parameter;
	m_param->solver_type= L2R_L2LOSS_SVC_DUAL;
	m_param->C = 512;
	m_param->eps = 0.001;
	m_param->nr_weight = 0;
	m_param->weight_label = NULL;
	m_param->weight = NULL;

	m_prob = new problem;
	m_prob->l = trainset->m_sample_num;
	m_prob->n = trainset->m_feature_size;//36=4*9
	m_prob->y = new int[trainset->m_sample_num];
	m_prob->x = new feature_node*[trainset->m_sample_num];
	for(int i=0;i<trainset->m_sample_num;i++)
	{
	  m_prob->x[i] = (feature_node*)malloc((trainset->m_feature_size+1)*sizeof(feature_node));
	  for(int j=0;j<trainset->m_feature_size;j++)
	  {
	    m_prob->x[i][j].index = j+1;
	  }
	  m_prob->x[i][trainset->m_feature_size].index = -1;
        }
	m_prob->W = new double[trainset->m_sample_num];

	m_prob->bias = -1;

	m_target = new int[m_prob->l];
	}

	m_f = new float[trainset->m_feature_size];
	m_x = new feature_node[trainset->m_feature_size+1];
	m_v = new float[trainset->m_sample_num];

	liblinear_print_string =  &print_null;
}


int SvmClassifier::Test(float* fv)
{
  return GetValue(fv)>m_thresh?1:-1;
}

int SvmClassifier::Test(Features* extractor,int y,int x)
{
	extractor->Extract(m_f,y,x,&m_feature_index,1);
	return GetValue_fast(m_f)>m_thresh?1:-1;
}

int SvmClassifier::Test_fast(float* fv)
{
  return GetValue_fast(fv)>m_thresh?1:-1;
}

float SvmClassifier::GetValue_fast(float* fv)
{
	int i;
	double r;

	for(i=0;i<m_feature_size;i++)
	{
		m_x[i].index = i+1;
		m_x[i].value = fv[i];
	}
	m_x[m_feature_size].index = -1;
	predict_values(m_model,m_x,&r);

	return float(m_a*r+m_b);
}

void SvmClassifier::GetBestPenaltyByCrossValidation(DataSet* trainset)
{
	int i,j;
	float error,min_error=1.0;
	double C = 0.0625, best_C=1.0, r;
	for(i=0;i<15;i++)
	{
		error = 0.0;
		C *= 2;
		m_param->C = C;
		cross_validation(m_prob,m_param,3,m_target);
		for(j=0;j<m_prob->l;j++)
			if(m_target[j] != m_prob->y[j])
				error+=trainset->m_weights[j];
		if(error<min_error)
		{
			min_error = error;
			best_C = C;
		}
	}
	m_param->C = best_C;
	std::clog<<"...error is "<<min_error<<", C="<<best_C<<std::endl;
}

void SvmClassifier::Train(DataSet* trainset)
{
	int i,j,k,label;
	double r;
	float* fv;
	float t1,t2;	

	Clear();
	
	//	float w_min = 1.f;
	//for(i=0;i<trainset->m_sample_num;i++)
	//{
	//  if(trainset->m_weights[i]<w_min && trainset->m_weights[i]>0)
	//  {
	//    w_min = trainset->m_weights[i];
	//  }
	//}

//     	if(trainset->m_randomselect_flag)
//	{
	//	if(m_sample_total!=0)
	//	{
	//		delete []m_prob->y;
	//		for(i=0;i<m_sample_total;i++)
	//		  delete [](m_prob->x[i]);
	//		delete []m_prob->x;
	//		delete []m_prob->W;
	//	}
		//		double w_min=1.0;
		//for(i=0;i<trainset->m_sample_num;i++)
		//{
		//	if(trainset->m_weights[i]<w_min && trainset->m_weights[i]>0.0)
		//		w_min = trainset->m_weights[i];
		//}
		//m_sample_total = 0;
		//for(i=0;i<trainset->m_sample_num;i++)
		//{
		//	m_sample_copy[i] = int(trainset->m_weights[i]/w_min);
		//	m_sample_total+=m_sample_copy[i];
		//}

		//	m_prob->l = m_sample_total;
		//m_prob->y = new int[m_sample_total];
		//m_prob->x = new feature_node*[m_sample_total];
		//m_prob->W = new double[m_sample_total];
	//	for(i=0;i<trainset->m_feature_selected_num;i++)
	//	{
		  //	m_prob->x[i] = (feature_node*)malloc((trainset->m_feature_size+1)*sizeof(feature_node));
	//		for(j=0;j<trainset->m_feature_size;j++)
	//		{
	//			m_prob->x[i][j].index = j+1;
	//		}
	//		m_prob->x[i][trainset->m_feature_size].index = -1;
	//	}
		//trainset->m_randomselect_flag = false;
//	}

//	int t=0;
//	float w_max = 0.0;
	for(i=0;i<trainset->m_sample_num;i++)
	{
		//m_prob->W[i] = trainset->m_weights[i]/w_min;  //ÉèÖÃÈ¨ÖØ
		fv = trainset->FetchSampleFeature(i,m_feature_index);
		//	for(j=0;j<m_sample_copy[i];j++)
		//{
			for(j=0;j<trainset->m_feature_size;j++)
			{
				m_prob->x[i][j].value = fv[j];
			}
			m_prob->y[i] = trainset->m_labels[i];
			m_prob->W[i] = trainset->m_weights[i];
			//		m_prob->W[i] = trainset->m_weights[i]/w_min ;
			//}
			//			if(m_prob->W[i]>w_max)
			//  w_max = m_prob->W[i];
	}

	/*cross validation to find best C*/
	//GetBestPenaltyByCrossValidation(trainset);

	m_model = train(m_prob,m_param);
	//char filename[100];
	//	sprintf(filename,"%s/linear.txt",dir_prefix);
	//	save_model(filename,m_model);
	//destroy_model(m_model);
	//	m_model = load_model(filename);


	//error
	m_b = 0.0;t1=0.0f;t2=0.0f;
	//	feature_node *x = (feature_node*)malloc((trainset->m_feature_size+1)*sizeof(feature_node));
	for(j=0;j<trainset->m_feature_size;j++)
	{
	  //x[j].index = j+1;
		m_x[j].index = j+1;
	}
	m_x[trainset->m_feature_size].index = -1;
	for(i=0;i<trainset->m_sample_num;i++)
	{
		fv = trainset->FetchSampleFeature(i,m_feature_index);
		for(j=0;j<trainset->m_feature_size;j++)
		{
			m_x[j].value = fv[j];
		}
		predict_values(m_model,m_x, &r);
		m_v[i] = float(r);
		t1 += m_v[i]*trainset->m_labels[i]*trainset->m_weights[i];
		t2 += m_v[i]*m_v[i]*trainset->m_weights[i];
		m_b += trainset->m_labels[i]*trainset->m_weights[i];
	}

	m_a = t1/t2;
	
	m_error = 0.0f;

	int* indexes;
	
	try{
		indexes = new int[trainset->m_sample_num];
	}catch(std::bad_alloc &ba)
	{
		std::cerr<<ba.what()<<"at the SvmClassifier::Train!"<<std::endl;
	}

	for(i=0;i<trainset->m_sample_num;i++)
	{
          m_v[i] = m_a*m_v[i]+m_b;
	  indexes[i] = i;
	}
		
	
	QuickSort(m_v,indexes,0,trainset->m_sample_num-1);
	
	float e1=0.0,min1;
	int pos1;
	for(i=0;i<trainset->m_sample_num;i++)
		if(trainset->m_labels[indexes[i]]==-1)
		{
			t1 = m_v[i]+1.0;
			e1+=trainset->m_weights[indexes[i]]*t1*t1;
		}
		
	min1 = e1;
	pos1 = -1;
	
	for(i=0;i<trainset->m_sample_num;i++)
	{
		if(trainset->m_labels[indexes[i]]==-1)
		{
			t1 = m_v[i]+1.0;
			e1-=trainset->m_weights[indexes[i]]*t1*t1;
		}
		else
		{
			t1 = m_v[i]-1.0;
			e1+=trainset->m_weights[indexes[i]]*t1*t1;
		}
		if(e1<min1)
		{
			min1 = e1;
			pos1 = i;
		}
	}
	
	m_error = min1;
	m_thresh = m_v[pos1];//pos1 can not be -1
        
	std::clog<<m_feature_index<<" "<<m_a<<" "<<m_b<<" "<<m_error<<" "<<std::endl;
	delete []indexes;
}
