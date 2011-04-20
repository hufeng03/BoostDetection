/**
 * @file   WeakClassifier.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:13:25 2010
 * 
 * @brief  implemantation of WeakClassifier.
 * 
 * 
 */
#include "WeakClassifier.h"
#include "Features.h"

WeakClassifier::WeakClassifier(Features* F)
{
	m_feature_size = F->GetFeatureSize();
}

WeakClassifier::WeakClassifier()
{
	m_feature_size = 1;
}


WeakClassifier::~WeakClassifier()
{
  ;
}

WeakClassifier& WeakClassifier::operator = (const WeakClassifier & wc) 
 {
	m_feature_size = wc.m_feature_size;
	m_feature_index = wc.m_feature_index;
	m_error = wc.m_error;
	return *this;
 }
