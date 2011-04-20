
/**
 * @file   BoostClassifier.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:06:30 2010
 * 
 * @brief  implementation of BoostClassifier
 * 
 * 
 */
#include "BoostClassifier.h"
#include <math.h>
#include <iostream>
#include <typeinfo>
#include "CartClassifier.h"
#include "SvmClassifier.h"

std::vector<WeakClassifier*> BoostClassifier::m_wc_pool;
int BoostClassifier::m_boost_type = GENTLE;
int BoostClassifier::m_weak_type = CART;

BoostClassifier::BoostClassifier()
{
	m_count = 0;
	m_thresh = 0;

	m_falsepositive_goal = 0.7;  //init for training
	m_detection_goal = 0.9975;     //init for training


	m_feature_indexes = NULL;
	m_feature_values = NULL;
}

BoostClassifier::~BoostClassifier()
{
     ;
}

float BoostClassifier::GetValue(float* fv)
{
	int i;
	float val;
	val = 0.0;
	switch(m_boost_type)
	{
	case GENTLE:
		for(i=0;i<m_count;i++)
		{
			val += m_alphas[i]*m_wcs[i]->GetValue(fv);
		}
		break;
	case REAL:

	case LOGIT:

	case DISCRETE:
	default:
		for(i=0;i<m_count;i++)
		{
			val += m_alphas[i]*m_wcs[i]->Test(fv);
		}
	}

	return val;
}

int BoostClassifier::Test(float* fv)
{
  return (GetValue(fv)>m_thresh?1:-1);
}

int BoostClassifier::Test(Features* extractor,int y,int x)
{
	float val;
	int i;
	val = 0.0;
	extractor->Extract(m_feature_values,y,x,m_feature_indexes,m_count);
	
	switch(m_boost_type)
	{
	case GENTLE:
		for(i=0;i<m_count;i++)
		{
			val += m_wcs[i]->GetValue_fast(m_feature_values+m_feature_size*i);
		}
		break;
	case REAL:
		
	case LOGIT:
		
	case DISCRETE:
	default:
		for(i=0;i<m_count;i++)
		{
			val += m_alphas[i]*m_wcs[i]->Test_fast(m_feature_values+m_feature_size*i);
		}
	}

	return (val>m_thresh?1:-1);
}


void BoostClassifier::Clear()
{
  if(m_count!=0)
    {
      for(int i=0;i<m_count;i++)
	{
	  m_wcs[i]->Clear();
	}
    }
  m_count = 0;
  m_thresh = 0.0;
}


void BoostClassifier::ReadFromFile(std::ifstream& f)
{
	int i;
	float alpha;
  Clear();
  char boost_type[50];
  f>>boost_type;
  if(!strcmp(boost_type, "REAL"))
       SetBoostType(REAL);
  else if(!strcmp(boost_type, "DISCRETE"))
       SetBoostType(DISCRETE);
  else if(!strcmp(boost_type, "LOGIT"))
       SetBoostType(LOGIT);
  else if(!strcmp(boost_type, "GENTLE"))
       SetBoostType(GENTLE);
  else
       ;
  
  char weak_type[50];
  f>>weak_type;
  if(!strcmp(weak_type, "SVM"))
       SetWeakType(SVM);
  else if(!strcmp(weak_type, "CART"))
       SetWeakType(CART);

  f>>m_count;
  f.ignore(256,'\n');
	
  f>>m_thresh; 
  f.ignore(256,'\n');
  m_alphas.clear();
  for(i=0;i<m_count;i++) 
    {
      f>>alpha;
	  m_alphas.push_back(alpha);
    }
  f.ignore(256,'\n');

  m_wcs.clear();
  if(m_weak_type == SVM)
  {
	  SvmClassifier* sc;
	  try{
		  for(i=0;i<m_count;i++)
		  {
			  sc = new SvmClassifier;
			  m_wcs.push_back(sc);
		  }
	  }catch(std::bad_alloc &ba)
	{
		 std::cerr<<ba.what()<<"at the "<<i<<" ReadFromFile!"<<std::endl;
		 exit(0);
	}
  }
  else if(m_weak_type == CART)
  {
	  CartClassifier* sc;
	  try{
		  for(i=0;i<m_count;i++)
		  {
			  sc = new CartClassifier;
			  m_wcs.push_back(sc);
		  }
	  }catch(std::bad_alloc &ba)
	  {
		  std::cerr<<ba.what()<<"at the "<<i<<" ReadFromFile!"<<std::endl;
		  exit(0);
	  }
  }
  else
  {
	  std::cout<<"error!no match of "<<weak_type<<std::endl;
  }

  for(i=0;i<m_count;i++) 
    {
      m_wcs[i]->ReadFromFile(f);
    }
  f.ignore(256,'\n');

  m_feature_size = m_wcs[0]->GetFeatureSize();
  m_feature_indexes = new int[m_count];
  m_feature_values = new float[m_count*m_feature_size];

  for(i=0;i<m_count;i++) 
    {
	  m_feature_indexes[i] = m_wcs[i]->GetFeatureIndex();
    }
}

void BoostClassifier::WriteToFile(std::ofstream& f) const
{
     switch(m_boost_type)
     {
     case DISCRETE:
	  f<<"DISCRETE "; break;
     case REAL:
	  f<<"REAL "; break;
     case LOGIT:
	  f<<"LOGIT "; break;
     case GENTLE:
	  f<<"GENTLE "; break;
     }
       
  switch(m_weak_type)
  {
  case SVM:
       f<<"SVM"<<std::endl; break;
  case CART:
       f<<"CART"<<std::endl; break;
  }

  f<<m_count<<std::endl;
  f<<m_thresh<<std::endl;
  for(int i=0;i<m_count;i++)
    {
      f<<m_alphas[i]<<" ";
    }
  f<<std::endl;

  for(int i=0;i<m_count;i++)
    {
      m_wcs[i]->WriteToFile(f);
    }
  f<<std::endl;
}

void BoostClassifier::DrawFeatures(IplImage*img, Features* extractor, int n)
{
     if(n>m_count)
	  n = m_count;
     for(int i=0;i<n;i++)
     {
	  extractor->DrawFeature(img, m_feature_indexes[i], CV_RGB(255,0,0));	  
     }
}

void BoostClassifier::DrawFeatures(IplImage* img, Features* extractor)
{
     for(int i=0;i<m_count;i++)
     {
	  extractor->DrawFeature(img, m_feature_indexes[i], CV_RGB(255,0,0));
     }
}

void BoostClassifier::SetTrainGoal(float falsepositive_rate, float detection_rate)
{
	m_falsepositive_goal = falsepositive_rate;
	m_detection_goal = detection_rate;
	return;
}

void BoostClassifier::Train(DataSet* trainset)
{
  int i,j,k;
  bool* used = NULL;
  int* idx;
  int* indexes;
  float* vals;

  Clear();

  m_feature_size = trainset->m_feature_size;

  try{ 

       indexes = new int[trainset->m_sample_num];
       vals = new float[trainset->m_sample_num];
  }catch(std::bad_alloc &ba)
  {
       std::cerr<<ba.what()<<"at boost train!"<<std::endl;
       exit(0);
  }
  for(i=0;i<trainset->m_sample_num;i++)
  {
       vals[i] = 0.0f;
       indexes[i] = i;
  }

  trainset->InitializeWeights();//初始化训练样本权值

  do
   {
      std::clog<<"Start to find the "<<m_count<<"th weak classifier..."<<std::endl;
  	/*Strategy used by Zhu Qiang
  	select 0.05% weakclassifier.
       */
      trainset->RandomSelect();
      
      AddOneWeakClassifier(m_count, trainset, used);
      std::clog<<"Find weak classifier with:"<<" alpha = "<<m_alphas[m_count-1]<<", feature_index =  "<<trainset->m_selected_feature_idx[m_wcs[m_count-1]->GetFeatureIndex()]<<", error rate = "<<m_wcs[m_count-1]->m_error<<". ";


	float _error = 0.0f;
    //find suitable threshold, and compute false positive
	  for(i=0;i <trainset->m_sample_num; i++)
	  {
		  switch(m_boost_type)
		  {
		  case GENTLE:
			  vals[i] += m_wcs[m_count-1]->GetValue(trainset->FetchSampleFeatures(indexes[i]));
			  break;
		  case REAL:

		  case LOGIT:

		  case DISCRETE:
		  default:
			  vals[i] += m_alphas[m_count-1]*m_wcs[m_count-1]->Test(trainset->FetchSampleFeatures(indexes[i]));
		  }

		  if(trainset->m_labels[indexes[i]]==1 && vals[i]<=0)
		    _error+=0.5/trainset->m_positive_num;
		  if(trainset->m_labels[indexes[i]]==-1 && vals[i]>0)
		    _error+=0.5/trainset->m_negative_num;
	  }
	  std::clog<<"Boost error rate = "<<_error<<". ";

	  //retrieve the real index in the whole feature dataset
  	  i = trainset->m_selected_feature_idx[m_wcs[m_count-1]->GetFeatureIndex()];
  	  m_wcs[m_count-1]->SetFeatureIndex(i);

	  int pos_num=0,neg_num=0;

	  if(m_count==1)
	  {
		  for(i=0;i <trainset->m_sample_num; i++)
		  {
			  if(vals[i]>0)
			  {
				  if(trainset->m_labels[indexes[i]]==1)
					pos_num++;
				  else
					neg_num++;
			  }
		  }
	  }
	  else
	  {
		  QuickSort(vals, indexes, 0, trainset->m_sample_num-1);	  
		  for(i=trainset->m_sample_num-1;i>=0; i--)
		  {
			  if(vals[i] == vals[0])
			{
				m_thresh = vals[i];
				break;
			}
			  if(trainset->m_labels[indexes[i]] == 1)
				  pos_num++;
			  else
				  neg_num++;
			  if(pos_num>=m_detection_goal*trainset->m_positive_num)
			  {
				  //m_thresh = vals[i];
				  j = i;
				  while(vals[--j]==vals[i])
				 {
						  if(trainset->m_labels[indexes[j]]==1)
							  pos_num++;
						  else
							  neg_num++;
				  }
				  m_thresh = vals[j];
				  break;
			  }
		  }
		  
	  }

	  m_detection = (float)pos_num/trainset->m_positive_num;
	  m_falsepositive = (float)neg_num/trainset->m_negative_num;

      std::clog<<"Threshold is adjusted to be "<<m_thresh<<", detection rate = "<<m_detection<<", false positive rate = "<<m_falsepositive<<std::endl;
   }while(m_detection<m_detection_goal || m_falsepositive>m_falsepositive_goal);


  //Reset Feature Index
  if(m_feature_indexes != NULL)
    delete[] m_feature_indexes;
  
  if(m_feature_values != NULL)
    delete[] m_feature_values;

   m_feature_indexes = new int[m_count];
   m_feature_values = new float[m_count*trainset->m_feature_size];
  for(int i=0;i<m_count;i++) 
  {
	  m_feature_indexes[i] = m_wcs[i]->GetFeatureIndex();
  }
  free(vals);
  free(indexes);
  //  delete[] used; used=NULL;
}

float BoostClassifier::GetTrainResult()
{
	return m_falsepositive;
}

void BoostClassifier::AddOneWeakClassifier(int count, DataSet* trainset, bool* used)
{
  int i;
  float alpha,beta;
  WeakClassifier* minsc;
  float minerror;
  int minindex;

  minerror = float(1.01); 
  minindex  = -1;


  for(i=0;i<trainset->m_selected_feature_num;i++)
    {
//       if(used[i]) 
// 	{
// 	  continue;
// 	}
      m_wc_pool[i]->Train(trainset);
      if(m_wc_pool[i]->m_error<minerror)
	{
	  minerror = m_wc_pool[i]->m_error;
	  minsc = m_wc_pool[i];
	  minindex = i;
	}
    }

//  used[minindex] = true;
  beta = minerror / (float(1.0)-minerror);

  alpha = -log(beta);

  for(i=0;i<trainset->m_sample_num;i++)
    {

		switch(m_boost_type)
		{
		case GENTLE:
			trainset->m_weights[i]*=exp(-minsc->GetValue(trainset->FetchSampleFeatures(i))*trainset->m_labels[i]);
			break;
		case REAL:

		case LOGIT:

		case DISCRETE:
		default:
			if(minsc->Test(trainset->FetchSampleFeatures(i)) != trainset->m_labels[i])
			{
				trainset->m_weights[i] *= exp(alpha);

			}
		}
    }

  trainset->NormalizeWeights();
  /*必须将wc从wc pool里面拷贝出来，因为下次训练时候，pool里面的所有wc的预值会改变*/
 if(typeid(*minsc) == typeid(CartClassifier))
 {
	 CartClassifier* selected;
	 try{
		selected= new CartClassifier;
	 }catch(std::bad_alloc &ba)
	 {
		 std::cerr<<ba.what()<<"at boost addoneweakclassifier!"<<std::endl;
		 exit(0);
	 }
	CartClassifier* min_sc = dynamic_cast<CartClassifier*>(minsc);
	*selected = *min_sc;
	m_wcs.push_back(selected);
 }
 else if(typeid(*minsc) == typeid(SvmClassifier))
 {
	 SvmClassifier* selected;
	 try{
		selected = new SvmClassifier;
	 }catch(std::bad_alloc &ba)
	 {
		 std::cerr<<ba.what()<<"at boost addoneweakclassifier!"<<std::endl;
		 exit(0);
	 }
	SvmClassifier* min_sc = dynamic_cast<SvmClassifier*>(minsc);
	*selected = *min_sc;
	m_wcs.push_back(selected);
 }

 for(i=0;i<trainset->m_sample_num;i++)
 {

	 switch(m_boost_type)
	 {
	 case GENTLE:
		 m_alphas.push_back(1.0);
		 break;
	 case REAL:

	 case LOGIT:

	 case DISCRETE:
	 default:
	     m_alphas.push_back(alpha);
	 }
 }
  
  m_count++;

  return;
}

void BoostClassifier::InitWeakClassifierPool(Features* pExtractor, char* type, float ratio)
{
	int pool_size = pExtractor->GetFeatureNum()*ratio;

	for(int i=0;i<pool_size;i++)
	{
		if(strcmp(type,"SvmClassifier")==0)
			BoostClassifier::m_wc_pool.push_back(new SvmClassifier(pExtractor)); 
		else if(strcmp(type,"CartClassifier")==0)
			BoostClassifier::m_wc_pool.push_back(new CartClassifier(pExtractor));
		else
			std::clog<<"Error in BoostClassifier Init!"<<std::endl;

		BoostClassifier::m_wc_pool[i]->SetFeatureIndex(i);
	}
}
