/**
 * @file   Features.h
 * @author feng <huf07@mails.tsinghua.edu.cn>
 * @date   Tue Sep  9 10:58:12 2008
 * 
 * @brief  header file of Features.cpp
 * 
 * 
 */
#ifndef _features_h_
#define _features_h_

#include <cv.h>
#include <fstream>


class Features
{
 public:
 Features(int width, int height) : m_rect_width(width),m_rect_height(height),m_feature_num(0){};
     
  virtual ~Features();
        
  virtual int GetWidth(){return m_rect_width;};
     
  virtual int GetHeight(){return m_rect_height;};
		
  virtual int GetFeatureNum(){return m_feature_num;};

  virtual int GetFeatureSize(){return m_feature_size;};

  //you only have 
  virtual void Init(IplImage*) = 0;
//  virtual bool Extract(float* fv, int, int) = 0;
  virtual bool Extract(float* fv, int, int, bool* selected = NULL) = 0;
  virtual bool Extract(float* fv, int, int, int* selected, int num) = 0;
//  virtual bool Extract(float* fv, in, int, int*, int k) = 0;
  virtual void Clear() = 0;
  virtual void DrawFeature(IplImage*, int, CvScalar) = 0;
  virtual void ReadFromFile(std::ifstream& f) = 0;
  virtual void WriteToFile(std::ofstream& f) = 0;

  virtual void LoadFromFile(char* , float* fv);
  virtual void SaveToFile(char* , float* fv);
     
 protected:
  int m_rect_width;
  int m_rect_height;
  int m_feature_num;
  int m_feature_size;
  int m_width;
  int m_height;
};

#endif
