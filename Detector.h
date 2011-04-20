/**
 * @file   Detector.h
 * @author feng <huf07@mails.tsinghua.edu.cn>
 * @date   Tue Sep  9 10:58:12 2008
 * 
 * @brief  header file of Detector.cpp
 * 
 * 
 */

#ifndef _detector_h_
#define _detector_h_

#include "DataSet.h"
#include "Classifier.h"
#include "Features.h"

class Detector{
 public:
  Detector();
  ~Detector();

  void SetClassifier(Classifier* p){m_classifier = p;};
  void SetExtractor(Features* p){m_extractor = p;};

  Classifier* GetClassifier(){return m_classifier;};
  Features* GetExtractor(){return m_extractor;};

  void Check(IplImage*,std::vector<CvRect>&);

  void DrawResults(IplImage*, const std::vector<CvRect>&);
  void ShowResults(IplImage*, const std::vector<CvRect>&);

  void Train(DataSet*);
  int Test(IplImage*, int y=0, int x=0);

  void ReadFromFile(std::ifstream& ifile);
  void WriteToFile(std::ofstream& ofile);

  void DrawFeatures(IplImage*, int);

 private:
  CvRect IntersectRect(const CvRect& i, const CvRect& j);
  CvRect UnionRect(const CvRect& i, const CvRect& j);
  inline int SizeOfRect(const CvRect& rect);
  void PostProcess(std::vector<CvRect>&, int);
  Classifier* m_classifier;
  Features* m_extractor;
};

#endif
