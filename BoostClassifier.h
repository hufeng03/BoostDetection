/**
 * @file   BoostClassifier.h
 * @author feng <huf07@mails.tsinghua.edu.cn>
 * @date   Tue Sep  9 10:58:12 2008
 * 
 * @brief  header file of adaboostclassifier.cpp
 * 
 * 
 */

#ifndef _boostclassifier_h_
#define _boostclassifier_h_

#include "DataSet.h"
#include "Classifier.h"
#include "WeakClassifier.h"
#include <vector>

enum { DISCRETE, REAL, LOGIT, GENTLE}; /* Boosting Type */
enum { SVM, CART};/*Weakclassifier Type*/

class BoostClassifier : public Classifier {
public:
     BoostClassifier();
     ~BoostClassifier();
     void Train(DataSet* trainset);
     int Test(float* fv);
     int Test(Features*,int y,int x);
     void WriteToFile(std::ofstream& f) const;
     void ReadFromFile(std::ifstream& f);
     void Clear();

     float GetValue(float* fv);
  		
     void AddOneWeakClassifier(int count, DataSet* trainset, bool* used);

     void SetTrainGoal(float falsepositive_rate, float detection_rate);
     float GetTrainResult();
     
     void SetBoostType(int t){m_boost_type = t;};
     int GetBoostType(){return m_boost_type;};

     void SetWeakType(int t){m_weak_type = t;};
     int GetWeakType(){return m_weak_type;};

     void DrawFeatures(IplImage*, Features*, int n);
     void DrawFeatures(IplImage*, Features*);

     int GetWeakClassifierNumber(){return m_count;};

     static std::vector<WeakClassifier*> m_wc_pool;
     static void InitWeakClassifierPool(Features*,char*,float);
     static int m_boost_type;
     static int m_weak_type;
private:
     std::vector<WeakClassifier*> m_wcs;
     std::vector<float> m_alphas;
     int* m_feature_indexes;
     float* m_feature_values;
     int m_feature_size;
     int m_count;
     float m_thresh;
     //for training
     float m_falsepositive_goal;
     float m_detection_goal;
     float m_detection;
     float m_falsepositive;
};

#endif
