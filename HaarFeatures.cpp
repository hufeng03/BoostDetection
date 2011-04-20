/**
 * @file   HaarFeatures.cpp
 * @author feng <feng@feng-desktop>
 * @date   Sun Jan 10 16:03:16 2010
 * 
 * @brief  implementation of Haar Feature
 * 
 * 
 */

#include "HaarFeatures.h"

HaarFeatures::HaarFeatures(int width, int height) : Features(width,height)
{
  //push_back all the features
  int x1,x2,x3,x4,y1,y2,y3,y4;
  int pickup = 9;

  int index = 0;

  Haar H;

  m_paddedsize  = 1.0/((1.0+m_rect_height)*(1.0+m_rect_width));

  for(y1=0;y1<m_rect_height;y1+=1)
    for(y3=y1+2;y3<=m_rect_height;y3+=2)
      for(x1=0;x1<m_rect_width;x1+=1)
	for(x3=x1+1;x3<=m_rect_width;x3+=1)
	  {
	    y2 = (y1+y3)/2;
	    x2 = x4 = y4 = -1;
	    H.type = 0;
	    H.y1 = y1; H.y2 = y2; H.y3 = y3; H.y4 = y4;
	    H.x1 = x1; H.x2 = x2; H.x3 = x3; H.x4 = x4;
	    if(index%10==pickup) m_haars.push_back(H);
	    index++;
	  }

  for(y1=0;y1<m_rect_height;y1+=1)
    for(y3=y1+1;y3<=m_rect_height;y3+=1)
      for(x1=0;x1<m_rect_width;x1+=1)
	for(x3=x1+2;x3<=m_rect_width;x3+=2)
	  {
	    x2 = (x1+x3)/2;
	    y2 = y4 = x4 = -1;
	    H.type = 1;
	    H.y1 = y1; H.y2 = y2; H.y3 = y3; H.y4 = y4;
	    H.x1 = x1; H.x2 = x2; H.x3 = x3; H.x4 = x4;
	    if(index%10==pickup) m_haars.push_back(H);
	    index++;
	  }

  for(y1=0;y1<m_rect_height;y1++)
    for(y4=y1+3;y4<=m_rect_height;y4+=3)
      for(x1=0;x1<m_rect_width;x1+=1)
	for(x3=x1+1;x3<=m_rect_width;x3+=1)
	  {
	    y2 = y1 + (y4-y1)/3;
	    y3 = y2 + (y4-y1)/3;
	    x2 = x4 = -1;
	    H.type = 2;
	    H.y1 = y1; H.y2 = y2; H.y3 = y3; H.y4 = y4;
	    H.x1 = x1; H.x2 = x2; H.x3 = x3; H.x4 = x4;
	    if(index%10==pickup) m_haars.push_back(H);
	    index++;
	  }

  for(y1=0;y1<m_rect_height;y1++)
    for(y3=y1+1;y3<=m_rect_height;y3+=1)
      for(x1=0;x1<m_rect_width;x1++)
	for(x4=x1+3;x4<=m_rect_width;x4+=3)
	  {
	    x2 = x1 + (x4-x1)/3;
	    x3 = x2 + (x4-x1)/3;
	    y2 = y4 = -1;
	    H.type = 3;
	    H.y1 = y1; H.y2 = y2; H.y3 = y3; H.y4 = y4;
	    H.x1 = x1; H.x2 = x2; H.x3 = x3; H.x4 = x4;
	    if(index%10==pickup) m_haars.push_back(H);
	    index++;
	  }

  for(y1=0;y1<m_rect_height;y1+=1)
    for(y3=y1+2;y3<=m_rect_height;y3+=2)
      for(x1=0;x1<m_rect_width;x1+=1)
	for(x3=x1+2;x3<=m_rect_width;x3+=2)
	  {
	    y2 = (y1+y3)/2;
	    x2 = (x1+x3)/2;
	    y4 = x4 = -1;
	    H.type = 4;
	    H.y1 = y1; H.y2 = y2; H.y3 = y3; H.y4 = y4;
	    H.x1 = x1; H.x2 = x2; H.x3 = x3; H.x4 = x4;
	    if(index%10==pickup) m_haars.push_back(H);
	    index++;
	  }

  m_feature_num = m_haars.size();
  m_feature_size = 1;
  m_integral = NULL;
  m_square = NULL;
}

HaarFeatures::~HaarFeatures()
{
  if(m_integral!=NULL)
    {
      cvReleaseMat(&m_integral);
    }
  if(m_square!=NULL)
    {
      cvReleaseMat(&m_square);
    }
  return;
}

void HaarFeatures::Init(IplImage* image)
{
  m_width = image->width;
  m_height = image->height;
  m_sum_width = m_width + 1;
  m_sum_height = m_height + 1;

  if(m_integral == NULL || m_integral->width!=m_width+1 || m_integral->height!=m_height+1)
    {
      Clear();
      m_integral = cvCreateMat(m_height + 1,m_width + 1,CV_32SC1);
      m_square = cvCreateMat(m_height + 1,m_width + 1,CV_64FC1);
    }

  cvIntegral(image,m_integral,m_square,0);
//  CalcIntegral(image);
  return;
}
void HaarFeatures::CalcIntegral(IplImage* img)
{
     int i,j,k,v,partialsum;
     double partialsumq;
     for(i=0;i<m_sum_width;i++)
     {
       m_integral->data.i[i] = 0;
       m_square->data.db[i] = 0;
     }
     for(i=1;i<m_sum_height;i++)
     {
	  char* pt = img->imageData+(i-1)*img->widthStep;
	  partialsum = 0;
	  partialsumq = 0;
	  m_integral->data.i[i*m_sum_width]=0;
	  m_square->data.db[i*m_sum_width]=0;
	  for(j=1;j<m_sum_width;j++)
	  {	
	       v = pt[j-1];
	       partialsum += v;
	       partialsumq += v*v;
	       m_integral->data.i[i*m_sum_width+j] = m_integral->data.i[(i-1)*m_sum_width+j] + partialsum;
	       m_square->data.db[i*m_sum_width+j] = m_square->data.db[(i-1)*m_sum_width+j] + partialsumq;
	  }
     }
}


void HaarFeatures::Clear()
{
  if(m_integral != NULL)
    {
      cvReleaseMat(&m_integral);
    }
  if(m_square != NULL)
    {
      cvReleaseMat(&m_square);
    }
  return;
}

// bool HaarFeatures::Extract(float* fv, int y, int x)
// {
//   float f;
//   int type,x1,x2,x3,x4,x5,y1,y2,y3,y4;
//   double paddedsize = 1.0/((1.0+m_rect_height)*(1.0+m_rect_width));
//   double ex = cvmGet(m_integral,y+m_rect_height,x+m_rect_width)+cvmGet(m_integral,y,x)-cvmGet(m_integral,y+m_rect_height,x)-cvmGet(m_integral,y,x+m_rect_width);
//   double sq = cvmGet(m_square,y+m_rect_height,x+m_rect_width)+cvmGet(m_square,y,x)-cvmGet(m_square,y+m_rect_height,x)-cvmGet(m_square,y,x+m_rect_width);
//   ex = ex*paddedsize;
//   ex = ex*ex;
//   sq = sq*paddedsize;
//   sq = sq - ex;
//   if(sq > 0)
//     {
//       m_var = sqrt(sq);
//     }
//   else
//     {
//       m_var = 1.0;
//     }
//   Haar H;
// 
//   for (int i=0;i<m_feature_num;i++)
//     {
//       H = m_haars[i];
// 
//       type = H.type;
//       x1 = x+H.x1;
//       x2 = x+H.x2;
//       x3 = x+H.x3;
//       x4 = x+H.x4;
//       y1 = y+H.y1;
//       y2 = y+H.y2;
//       y3 = y+H.y3;
//       y4 = y+H.y4;
//       fv[i] = GetOneFeatureValue(type, y1, y2, y3, y4, x1, x2, x3, x4);
//     }
//   return true;
// }

bool HaarFeatures::Extract(float* fv, int y, int x, bool* selected)
{
  float f;
  int type,x1,x2,x3,x4,x5,y1,y2,y3,y4,val;
//  static double paddedsize = 1.0/((1.0+m_rect_height)*(1.0+m_rect_width));
  int ex = m_integral->data.i[(y+m_rect_height)*m_sum_width+x+m_rect_width]+m_integral->data.i[y*m_sum_width+x]-m_integral->data.i[(y+m_rect_height)*m_sum_width+x]-m_integral->data.i[y*m_sum_width+x+m_rect_width];
  double sq = m_square->data.db[(y+m_rect_height)*m_sum_width+x+m_rect_width]+m_square->data.db[y*m_sum_width+x]-m_square->data.db[(y+m_rect_height)*m_sum_width+x]-m_square->data.db[y*m_sum_width+x+m_rect_width];
  ex = ex*m_paddedsize;
  ex = ex*ex;
  sq = sq*m_paddedsize;
  sq = sq - ex;
  if(sq > 0)
    {
      m_var = sqrt(sq);
    }
  else
    {
      m_var = 1.0;
    }

  m_var = 1.0;
  for (int i=0;i<m_feature_num;i++)
    {
      if(selected!=NULL && !selected[i])
	{
	  continue;
	}
      type = m_haars[i].type;
      x1 = x+m_haars[i].x1;
      x2 = x+m_haars[i].x2;
      x3 = x+m_haars[i].x3;
      x4 = x+m_haars[i].x4;
      y1 = y+m_haars[i].y1;
      y2 = y+m_haars[i].y2;
      y3 = y+m_haars[i].y3;
      y4 = y+m_haars[i].y4;
//      fv[i] = GetOneFeatureValue(type, y1, y2, y3, y4, x1, x2, x3, x4);
      val = 0;
      switch(type)
      {
      case 0:
	   val =   m_integral->data.i[y1*m_sum_width+x3] - m_integral->data.i[y1*m_sum_width+x1] + m_integral->data.i[y3*m_sum_width+x3] - m_integral->data.i[y3*m_sum_width+x1]
		+ 2*(m_integral->data.i[y2*m_sum_width+x1] - m_integral->data.i[y2*m_sum_width+x3]);
	   break;
      case 1:
	   val =   m_integral->data.i[y3*m_sum_width+x1] + m_integral->data.i[y3*m_sum_width+x3] - m_integral->data.i[y1*m_sum_width+x1] - m_integral->data.i[y1*m_sum_width+x3]
		+ 2*(m_integral->data.i[y1*m_sum_width+x2] - m_integral->data.i[y3*m_sum_width+x2]);
	   break;
      case 2:
	   val =   m_integral->data.i[y1*m_sum_width+x1] -m_integral->data.i[y1*m_sum_width+x3] + m_integral->data.i[y4*m_sum_width+x3] - m_integral->data.i[y4*m_sum_width+x1]
		+ 3*(m_integral->data.i[y2*m_sum_width+x3] - m_integral->data.i[y2*m_sum_width+x1] + m_integral->data.i[y3*m_sum_width+x1] - m_integral->data.i[y3*m_sum_width+x3]);
	   break;
      case 3:
	   val =   m_integral->data.i[y1*m_sum_width+x1] - m_integral->data.i[y1*m_sum_width+x4] + m_integral->data.i[y3*m_sum_width+x4] - m_integral->data.i[y3*m_sum_width+x1]
		+ 3*(m_integral->data.i[y3*m_sum_width+x2] - m_integral->data.i[y3*m_sum_width+x3] + m_integral->data.i[y1*m_sum_width+x3] - m_integral->data.i[y1*m_sum_width+x2] );
	   break;
      case 4:
	   val =   m_integral->data.i[y1*m_sum_width+x1] + m_integral->data.i[y1*m_sum_width+x3] + m_integral->data.i[y3*m_sum_width+x1] + m_integral->data.i[y3*m_sum_width+x3]
		- 2*(m_integral->data.i[y2*m_sum_width+x1] + m_integral->data.i[y2*m_sum_width+x3] + m_integral->data.i[y1*m_sum_width+x2] + m_integral->data.i[y3*m_sum_width+x2])
		+ 4*m_integral->data.i[y2*m_sum_width+x2];
	   break;
      }
      fv[i] = val/m_var;
    }
  return true;
}

bool HaarFeatures::Extract(float* fv, int y, int x, int* selected,int num)
{
	float f;
	int type,x1,x2,x3,x4,x5,y1,y2,y3,y4,i,j,val;
//	static double paddedsize = 1.0/((1.0+m_rect_height)*(1.0+m_rect_width));
	double ex = m_integral->data.i[(y+m_rect_height)*m_sum_width+x+m_rect_width]+m_integral->data.i[y*m_sum_width+x]-m_integral->data.i[(y+m_rect_height)*m_sum_width+x]-m_integral->data.i[y*m_sum_width+x+m_rect_width];
	double sq = m_square->data.db[(y+m_rect_height)*m_sum_width+x+m_rect_width]+m_square->data.db[y*m_sum_width+x]-m_square->data.db[(y+m_rect_height)*m_sum_width+x]-m_square->data.db[y*m_sum_width+x+m_rect_width];
	ex = ex*m_paddedsize;
	ex = ex*ex;
	sq = sq*m_paddedsize;
	sq = sq - ex;
	if(sq > 0)
	{
		m_var = sqrt(sq);
	}
	else
	{
		m_var = 1.0;
	}

	for (i=0;i<num;i++)
	{
		j = selected[i];
		type = m_haars[j].type;
		x1 = x+m_haars[j].x1;
		x2 = x+m_haars[j].x2;
		x3 = x+m_haars[j].x3;
		x4 = x+m_haars[j].x4;
		y1 = y+m_haars[j].y1;
		y2 = y+m_haars[j].y2;
		y3 = y+m_haars[j].y3;
		y4 = y+m_haars[j].y4;
//		fv[i] = GetOneFeatureValue(type, y1, y2, y3, y4, x1, x2, x3, x4);
		val = 0;
		switch(type)
		{
		case 0:
		     val =   m_integral->data.i[y1*m_sum_width+x3] - m_integral->data.i[y1*m_sum_width+x1] + m_integral->data.i[y3*m_sum_width+x3] - m_integral->data.i[y3*m_sum_width+x1]
			  + 2*(m_integral->data.i[y2*m_sum_width+x1] - m_integral->data.i[y2*m_sum_width+x3]);
		     break;
		case 1:
		     val =   m_integral->data.i[y3*m_sum_width+x1] + m_integral->data.i[y3*m_sum_width+x3] - m_integral->data.i[y1*m_sum_width+x1] - m_integral->data.i[y1*m_sum_width+x3]
			  + 2*(m_integral->data.i[y1*m_sum_width+x2] - m_integral->data.i[y3*m_sum_width+x2]);
		     break;
		case 2:
		     val =   m_integral->data.i[y1*m_sum_width+x1] -m_integral->data.i[y1*m_sum_width+x3] + m_integral->data.i[y4*m_sum_width+x3] - m_integral->data.i[y4*m_sum_width+x1]
			  + 3*(m_integral->data.i[y2*m_sum_width+x3] - m_integral->data.i[y2*m_sum_width+x1] + m_integral->data.i[y3*m_sum_width+x1] - m_integral->data.i[y3*m_sum_width+x3]);
		     break;
		case 3:
		     val =   m_integral->data.i[y1*m_sum_width+x1] - m_integral->data.i[y1*m_sum_width+x4] + m_integral->data.i[y3*m_sum_width+x4] - m_integral->data.i[y3*m_sum_width+x1]
			  + 3*(m_integral->data.i[y3*m_sum_width+x2] - m_integral->data.i[y3*m_sum_width+x3] + m_integral->data.i[y1*m_sum_width+x3] - m_integral->data.i[y1*m_sum_width+x2] );
		     break;
		case 4:
		     val =   m_integral->data.i[y1*m_sum_width+x1] + m_integral->data.i[y1*m_sum_width+x3] + m_integral->data.i[y3*m_sum_width+x1] + m_integral->data.i[y3*m_sum_width+x3]
			  - 2*(m_integral->data.i[y2*m_sum_width+x1] + m_integral->data.i[y2*m_sum_width+x3] + m_integral->data.i[y1*m_sum_width+x2] + m_integral->data.i[y3*m_sum_width+x2])
			  + 4*m_integral->data.i[y2*m_sum_width+x2];
		     break;
		}
		fv[i] = val/m_var;
	}
	return true;
}

inline double HaarFeatures::GetOneFeatureValue(int type, int y1, int y2, int y3, int y4, int x1, int x2, int x3, int x4)
{
  double val = 0.0;

  switch(type)
    {
    case 0:
      val =   m_integral->data.i[y1*(m_width+1)+x3] - m_integral->data.i[y1*(m_width+1)+x1] + m_integral->data.i[y3*(m_width+1)+x3] - m_integral->data.i[y3*(m_width+1)+x1]
	   + 2*(m_integral->data.i[y2*(m_width+1)+x1] - m_integral->data.i[y2*(m_width+1)+x3]);
      break;
    case 1:
      val =   m_integral->data.i[y3*(m_width+1)+x1] + m_integral->data.i[y3*(m_width+1)+x3] - m_integral->data.i[y1*(m_width+1)+x1] - m_integral->data.i[y1*(m_width+1)+x3]
	+ 2*(m_integral->data.i[y1*(m_width+1)+x2] - m_integral->data.i[y3*(m_width+1)+x2]);
      break;
    case 2:
      val =   m_integral->data.i[y1*(m_width+1)+x1] -m_integral->data.i[y1*(m_width+1)+x3] + m_integral->data.i[y4*(m_width+1)+x3] - m_integral->data.i[y4*(m_width+1)+x1]
	+ 3*(m_integral->data.i[y2*(m_width+1)+x3] - m_integral->data.i[y2*(m_width+1)+x1] + m_integral->data.i[y3*(m_width+1)+x1] - m_integral->data.i[y3*(m_width+1)+x3]);
      break;
    case 3:
      val =   m_integral->data.i[y1*(m_width+1)+x1] - m_integral->data.i[y1*(m_width+1)+x4] + m_integral->data.i[y3*(m_width+1)+x4] - m_integral->data.i[y3*(m_width+1)+x1]
	+ 3*(m_integral->data.i[y3*(m_width+1)+x2] - m_integral->data.i[y3*(m_width+1)+x3] + m_integral->data.i[y1*(m_width+1)+x3] - m_integral->data.i[y1*(m_width+1)+x2] );
      break;
    case 4:
      val =   m_integral->data.i[y1*(m_width+1)+x1] + m_integral->data.i[y1*(m_width+1)+x3] + m_integral->data.i[y3*(m_width+1)+x1] + m_integral->data.i[y3*(m_width+1)+x3]
	- 2*(m_integral->data.i[y2*(m_width+1)+x1] + m_integral->data.i[y2*(m_width+1)+x3] + m_integral->data.i[y1*(m_width+1)+x2] + m_integral->data.i[y3*(m_width+1)+x2])
	+ 4*m_integral->data.i[y2*(m_width+1)+x2];
      break;
    }
  return val/m_var;
}

void HaarFeatures::ReadFromFile(std::ifstream& f)
{
  Haar H;
  m_haars.clear();
  f>>m_feature_num;
  f.ignore(256,'\n');
  for(int i=0; i<m_feature_num;i++)
    {
      f>>H.type>>H.y1>>H.y2>>H.y3>>H.y4>>H.x1>>H.x2>>H.x3>>H.x4;
      m_haars.push_back(H);
      f.ignore(256,'\n');
    }
  return;
}

void HaarFeatures::WriteToFile(std::ofstream& f)
{
  f<<m_feature_num<<std::endl;
  for(int i=0;i<m_feature_num;i++)
    {
      f<<m_haars[i].type<<" "<<m_haars[i].y1<<" "<<m_haars[i].y2<<" "<<m_haars[i].y3<<" "<<m_haars[i].y4<<" "<<m_haars[i].x1<<" "<<m_haars[i].x2<<" "<<m_haars[i].x3<<" "<<m_haars[i].x4<<std::endl;
    }
}

void HaarFeatures::DrawFeature(IplImage* img, int n, CvScalar rgb)
{
  switch(m_haars[n].type)
  {
  case 0:
    cvRectangle(img, cvPoint(m_haars[n].x1, m_haars[n].y1), cvPoint(m_haars[n].x3, m_haars[n].y2), rgb, CV_FILLED, 8, 0);
    cvRectangle(img, cvPoint(m_haars[n].x1, m_haars[n].y2), cvPoint(m_haars[n].x3, m_haars[n].y3), rgb, 1, 8, 0);
    break;
  case 1:
    cvRectangle(img, cvPoint(m_haars[n].x1, m_haars[n].y1), cvPoint(m_haars[n].x1, m_haars[n].y3), rgb, CV_FILLED, 8, 0);
    cvRectangle(img, cvPoint(m_haars[n].x2, m_haars[n].y1), cvPoint(m_haars[n].x3, m_haars[n].y3), rgb, 1, 8, 0);
    break;
  case 2:
    cvRectangle(img, cvPoint(m_haars[n].x1, m_haars[n].y1), cvPoint(m_haars[n].x3, m_haars[n].y2), rgb, 1, 8, 0);
    cvRectangle(img, cvPoint(m_haars[n].x1, m_haars[n].y2), cvPoint(m_haars[n].x3, m_haars[n].y3), rgb, CV_FILLED, 8, 0);
    cvRectangle(img, cvPoint(m_haars[n].x1, m_haars[n].y3), cvPoint(m_haars[n].x3, m_haars[n].y4), rgb, 1, 8, 0);
    break;
  case 3:
    cvRectangle(img, cvPoint(m_haars[n].x1, m_haars[n].y1), cvPoint(m_haars[n].x2, m_haars[n].y3), rgb, 1, 8, 0);
    cvRectangle(img, cvPoint(m_haars[n].x2, m_haars[n].y1), cvPoint(m_haars[n].x3, m_haars[n].y3), rgb, CV_FILLED, 8, 0);
    cvRectangle(img, cvPoint(m_haars[n].x4, m_haars[n].y1), cvPoint(m_haars[n].x4, m_haars[n].y3), rgb, 1, 8, 0);
    break;
  case 4:
    cvRectangle(img, cvPoint(m_haars[n].x1, m_haars[n].y1), cvPoint(m_haars[n].x2, m_haars[n].y2), rgb, 1, 8, 0);
    cvRectangle(img, cvPoint(m_haars[n].x2, m_haars[n].y1), cvPoint(m_haars[n].x3, m_haars[n].y2), rgb, CV_FILLED, 8, 0);    
    cvRectangle(img, cvPoint(m_haars[n].x1, m_haars[n].y2), cvPoint(m_haars[n].x2, m_haars[n].y3), rgb, CV_FILLED, 8, 0);
    cvRectangle(img, cvPoint(m_haars[n].x2, m_haars[n].y2), cvPoint(m_haars[n].x3, m_haars[n].y3), rgb, 1, 8, 0);
    break;
  }
  return;
}
