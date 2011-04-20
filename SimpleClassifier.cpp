/**
 * @file   SimpleClassifier.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:12:24 2010
 * 
 * @brief  implementation of SimpleClassifier. This classifier cannot be combined with GentleBoost.
 * 
 * 
 */
#include "SimpleClassifier.h"
#include <iostream>

SimpleClassifier::SimpleClassifier(Features& F):WeakClassifier(F)
{
  m_thresh = 0.5;
  m_parity = 0;
  m_error = 0.0;
  m_feature_index = -1;
}

SimpleClassifier::SimpleClassifier()
{
	m_thresh = 0.5;
	m_parity = 0;
	m_error = 0.0;
	m_feature_index = -1;
}

SimpleClassifier::~SimpleClassifier()
{
  ;
}

SimpleClassifier& SimpleClassifier::operator=(const SimpleClassifier & sc)
{
	WeakClassifier::operator=(sc);
	m_thresh = sc.m_thresh;
	m_parity = sc.m_parity;
	return *this;
}


int SimpleClassifier::Test(float* fv)
{
  if(m_parity == 1)
    return (fv[m_feature_index]>m_thresh)?1:-1;
  else
    return (fv[m_feature_index]>m_thresh)?-1:1;
}

int SimpleClassifier::Test(Features* extractor,int y,int x)
{
	float f;
	extractor->Extract(&f,y,x,&m_feature_index,1);

	if(m_parity == 1)
		return (f>m_thresh)?1:-1;
	else
		return (f>m_thresh)?-1:1;
}

int SimpleClassifier::Test_fast(float* fv)
{
	if(m_parity == 1)
		return ((*fv)>m_thresh)?1:-1;
	else
		return ((*fv)>m_thresh)?-1:1;
}

float SimpleClassifier::GetValue(float* fv)
{
  return fv[m_feature_index];
}

float SimpleClassifier::GetValue_fast(float* fv)
{
	return (*fv);
}

void SimpleClassifier::ReadFromFile(std::ifstream& f)
{
  f>>m_thresh>>m_parity>>m_feature_index;
  f.ignore(256,'\n');
}

void SimpleClassifier::WriteToFile(std::ofstream& f) const
{
  f<<m_thresh<<" "<<m_parity<<" "<<m_feature_index<<" "<<std::endl;
}

void SimpleClassifier::Clear()
{
	
}


void SimpleClassifier::Train(DataSet* trainset)
{
  int i;
  float min1, min2;
  float e1,e2;
  int pos1,pos2;
  int* indexes;
  float* features;
  float* fv;

  try{
	indexes = new int[trainset->m_sample_num];
	features = new float[trainset->m_sample_num];
  }catch(std::bad_alloc &ba)
  {
	  std::cerr<<ba.what()<<"at the SimpleClassfier::Train!"<<std::endl;
	  exit(0);
  }

  for(i=0; i<trainset->m_sample_num; i++)
    {
      features[i] = *(trainset->FetchSampleFeature(i,m_feature_index));
      indexes[i] = i;
    }

  QuickSort(features, indexes, 0, trainset->m_sample_num-1);

  e1 = 0.0;
  e2 = 0.0;
  for(i=0;i<trainset->m_sample_num;i++) 
    {
      if(trainset->m_labels[i]==1) 
		e1+=trainset->m_weights[i];
	  else
 		e2+=trainset->m_weights[i];
    }
  min1 = e1; 
  pos1 = -1;
  min2 = e2; 
  pos2 = -1;
  for(i=0;i<trainset->m_sample_num;i++)
    {
		if(trainset->m_labels[indexes[i]]==1) 
		{
			e1-=trainset->m_weights[indexes[i]];
			e2+=trainset->m_weights[indexes[i]];
		}
		else 
		{
			e1+=trainset->m_weights[indexes[i]];
			e2-=trainset->m_weights[indexes[i]];
		}
		if(e1<min1) 
		{ 
			min1=e1; 
			pos1=i;
		}
		if(e2<min2) 
		{ 
			min2=e2; 
			pos2=i; 
		}
    }

  if(min1<min2)
  {
	  m_parity = -1;
	  m_error = min1;
	  m_thresh = features[pos1];
  }
  else
  {
	  m_parity = 1;
	  m_error = min2;
	  m_thresh = features[pos2];
  }

  delete []features;
  delete []indexes;
}

