/**
* @file   CartClassifier.cpp
* @author feng <feng@feng-desktop>
* @date   Sun Jan 10 16:12:24 2010
* 
* @brief  implementation of CartClassifier.
* 
* 
*/
#include "CartClassifier.h"
#include <iostream>

CartClassifier::CartClassifier(Features* F):WeakClassifier(F)
{
	m_error = 0.0;
	m_feature_index = -1;
}

CartClassifier::CartClassifier()
{
	m_error = 0.0;
	m_feature_index = -1;
}

CartClassifier::~CartClassifier()
{
	;
}

CartClassifier& CartClassifier::operator=(const CartClassifier & sc)
{
	WeakClassifier::operator=(sc);
	m_thresh = sc.m_thresh;
	m_a = sc.m_a;
	m_b = sc.m_b;
	m_parity = sc.m_parity;
	return *this;
}

int CartClassifier::Test(float* fv)
{
	return GetValue(fv)>0?1:-1;
}

int CartClassifier::Test(Features* extractor,int y,int x)
{
	float f;
	extractor->Extract(&f,y,x,&m_feature_index,1);
	return GetValue_fast(&f)>0?1:-1;
}

int CartClassifier::Test_fast(float* fv)
{
	return GetValue_fast(fv)>0?1:-1;
}

float CartClassifier::GetValue(float* fv)
{
	if((m_parity==1 && fv[m_feature_index]<m_thresh) || (m_parity==-1 && fv[m_feature_index]>m_thresh))
		return m_a;
	else
		return m_b;
}

float CartClassifier::GetValue_fast(float* fv)
{
/*	if((m_parity==1 && (*fv)<m_thresh) || (m_parity==-1 && (*fv)>m_thresh))
		return m_a;
	else
		return m_b;
*/
	if(m_parity>0)
	{
	     if(*fv<m_thresh)
		  return m_a;
	     else
		  return m_b;
	}
	else
	{
	     if(*fv>m_thresh)
		  return m_a;
	     else
		  return m_b;
	}
}

void CartClassifier::ReadFromFile(std::ifstream& f)
{
	f>>m_parity>>m_thresh>>m_a>>m_b>>m_feature_index;
	f.ignore(256,'\n');
}

void CartClassifier::WriteToFile(std::ofstream& f) const
{
	f<<m_parity<<" "<<m_thresh<<" "<<m_a<<" "<<m_b<<" "<<m_feature_index<<" "<<std::endl;
}

void CartClassifier::Clear()
{

}


void CartClassifier::Train(DataSet* trainset)
{
	int i;
	float min1, min2;
	float t1,t2,e1,e2;
	int pos1,pos2;
	int* indexes;
	float* features;
	float* fv;

	try{
		indexes = new int[trainset->m_sample_num];
		features = new float[trainset->m_sample_num];
	}catch(std::bad_alloc &ba)
	{
		std::cerr<<ba.what()<<"at the CartClassfier::Train!"<<std::endl;
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
	e2 = 1.0-e1;
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
	m_a = 0.0f;m_b= 0.0f;t1=0.0f;t2=0.0f;
	for(i=0;i<trainset->m_sample_num;i++)
	{
		if(m_parity==1)
		{
			if(features[i]<=m_thresh) 
			{
				t1 += trainset->m_weights[indexes[i]];
				 m_a += trainset->m_labels[indexes[i]]*trainset->m_weights[indexes[i]];
			}
			else 
			{	
				t2 += trainset->m_weights[indexes[i]];
				 m_b += trainset->m_labels[indexes[i]]*trainset->m_weights[indexes[i]];
			}
		}
		else
		{
			if(features[i]>m_thresh)
			{
				t1+=trainset->m_weights[indexes[i]];
				m_a += trainset->m_labels[indexes[i]]*trainset->m_weights[indexes[i]];
			}
			else
			{
				t2+=trainset->m_weights[indexes[i]];
				m_b += trainset->m_labels[indexes[i]]*trainset->m_weights[indexes[i]];
			}
		}
	}
	m_a = m_a/t1;
	m_b = m_b/t2;

	delete []features;
	delete []indexes;
}

