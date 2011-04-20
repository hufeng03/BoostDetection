/**
 * @file   Classifier.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:07:43 2010
 * 
 * @brief  implementation of Classifier
 * 
 * 
 */
#include "Classifier.h"
#include "Features.h"

//Classifier::Classifier(Features& F)
//{
//	m_feature_size = F.GetFeatureSize();
//}

Classifier::Classifier()
{
//	m_feature_size = 1;
	m_true_positive = 1.0;
	m_false_positive = 1.0;
}


Classifier::~Classifier()
{
  ;
}

void Classifier::QuickSort(float* values,int* labels,const int lo,const int hi)
{
	int i=lo, j=hi;
	float v; int l;
	float x = values[(lo+hi)/2];

	do
	{    
		while (values[i]<x) i++; 
		while (values[j]>x) j--;
		if (i<=j)
		{
			v=values[i]; values[i]=values[j]; values[j]=v;
			l=labels[i]; labels[i]=labels[j]; labels[j]=l;
			i++; j--;
		}
	} while (i<=j);

	if (lo<j) QuickSort(values,labels,lo, j);
	if (i<hi) QuickSort(values,labels,i, hi);
}
