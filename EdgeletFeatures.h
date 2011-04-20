/**
 * @file   EdgeletFeatures.h
 * @author feng <huf07@mails.tsinghua.edu.cn>
 * @date   Tue Sep  9 10:58:12 2008
 * 
 * @brief  header file of EdgeletFeatures.cpp
 * 
 * 
 */

#ifndef _edgeletfeatures_h_
#define _edgeletfeatures_h_

#include "Features.h"
#include <fstream>
#include <math.h>
#include <cv.h>
#include <highgui.h>
#include <vector>

template <class T, int N>
  struct array {
    inline T& operator[] (int i) {return val[i];}
    inline const T& operator[] (int i) const {return val[i];}
    T val[N];
  };

typedef array<int, 3> Pixel;	/**< 3 elements are x, y and orientation qualization */
  
typedef std::vector<Pixel> Edgelet; /**< Contains 4-12 Pixels */


class EdgeletFeatures : public Features 
{
 public:
  EdgeletFeatures(int width = 24, int height = 58);

  ~EdgeletFeatures();

  void Init(IplImage*);

  /** 
   * This function calculate the feature vector.
   * The result was written to m_features, and each element equals the reponse of the correspondent edgelet.
   */
//  bool Extract(float* fv, int y, int x);
  bool Extract(float*, int , int, bool* selected = NULL);
  bool Extract(float*, int, int, int* selected,int num);
  void Clear();
  void DrawFeature(IplImage* img, int n, CvScalar);

  void ReadFromFile(std::ifstream& f);
  void WriteToFile(std::ofstream& f);

 private:
  /** 
   * This function is to use standard Bresenham line algorithm to rander line edgelet.
   * It is called in InitEdgelets.
   * @param st Start point of line.
   * @param ed End point of line.
   * 
   * @return The rendered edgelet.
   */
  Edgelet RenderLine(CvPoint st, CvPoint ed);

  /** 
   * This function is to use Bresenham circle algorithm to render arc edgelet.
   * It is not mid-point algorithm and biggest difference is that this function supports float point center and radius.
   * Stupid code and it needs to be optimized.
   * It is called in InitEdgelets.
   * @param st Start point of the arc.
   * @param ed End point of the arc.
   * @param k Shape of arc: 2, 4, 8.
   * 
   * @return The rendered edgelet.
   */
  Edgelet RenderArc(CvPoint st, CvPoint ed, int k);

 
  CvMat* m_edge_intensity;	/**< Sobel intensity of input image */
  CvMat* m_edge_norm;		/**< Sobel orientation quantilization of input image  */
  
  float m_l[6];			/**< Table of dot production between two orientation quantilizations */
  std::vector<Edgelet> m_edgelets; /**< Array of edgelets contained in the m_width*m_height rectangle */
};


#endif
