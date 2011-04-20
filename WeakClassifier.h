/**
 * @file   WeakClassifier.h
 * @author feng <huf07@mails.tsinghua.edu.cn>
 * @date   Tue Sep  9 10:58:12 2008
 * 
 * @brief  header file of WeakClassifier.cpp
 * 
 * 
 */

#ifndef _weakclassifier_h_
#define _weakclassifier_h_

#include <fstream>
#include "Classifier.h"

class DataSet;
class Features;

class WeakClassifier:public Classifier{
 public:
  WeakClassifier();
  WeakClassifier(Features* F);
  virtual ~WeakClassifier();
  virtual int Test_fast(float* fv) = 0;
  virtual float GetValue_fast(float* fv) = 0;
  virtual float GetValue(float* fv) = 0;
  virtual void SetFeatureSize(int size){m_feature_size = size;};
  virtual int GetFeatureSize(){return m_feature_size;};
  virtual void SetFeatureIndex(int idx){m_feature_index = idx;};
  virtual int GetFeatureIndex(){return m_feature_index;};
  WeakClassifier& operator = (const WeakClassifier &); 

  float m_error;

protected:
  int m_feature_size;
  int m_feature_index;
};

#endif
