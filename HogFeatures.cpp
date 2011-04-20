/**
 * @file   HogFeatures.cpp
 * @author feng <huf07@mails.tsinghua.edu.cn>
 * @date   Tue Sep  9 13:47:01 2008
 * 
 * @brief  cpp file of HogFeatures.h
 * 
 * 
 */

#include "HogFeatures.h"

#define PI 3.1415927

HogFeatures::HogFeatures(int width, int height) : Features(width,height)
{
  Hog H;
  m_hogs.clear();

  int x, y, w, h, k, xh, yh, num;


  H.x = 0;
  H.y = 0;
  for(w = 12; w<=width; w+=2)
  {
	  H.w = w;
	  H.h = w;
	  if(H.h<=height)
	  {
		m_hogs.push_back(H);
	  }
  }

  for(w = 12; w<=width; w+=2)
  {
	  H.w = w;
	  H.h = w*2;
	  if(H.h<=height)
	  {
		m_hogs.push_back(H);
	  }
  }

  for(h = 12; h<=height; h+=2)
  {
	  H.h = h;
	  H.w = h*2;
	  if(H.w<=width)
	  {
		m_hogs.push_back(H);
	  }
  }


  num = m_hogs.size();
  for(k=0;k<num;k++)
  {
	  w = m_hogs[k].w;
	  h = m_hogs[k].h;
	  if(w<20)
	  {
		xh=4;
	  }
	  else if(w<28)
      {
	    xh=6;
	  }
	  else
	  {
		xh=8;
	  }
	  if(h<20)
	  {
		yh=4;
	  }
	  else if(h<28)
      {
	    yh=6;
	  }
	  else
	  {
		yh=8;
	  }
	  H.w = w;
	  H.h = h;
	  for(y=0; y+h-1<m_rect_height; y+=yh)
	  {
		for(x=0; x+w-1<m_rect_width; x+=xh)    
		{
			if(x==0 && y==0)
			{
				continue;
			}
			H.x = x;
			H.y = y;
			m_hogs.push_back(H);
		}
	  }
  }

  m_feature_num = m_hogs.size();
  m_feature_size = 36;

  m_gradient_amplitude = NULL;
  m_gradient_angle = NULL;
  for(k=0; k<9; k++)
  {
	  m_intergral_gradient[k] = NULL;
  }
  m_gradient_x = NULL;
  m_gradient_y = NULL;
  for(k=0;k<9;k++)
  {
	  m_gradient_k[k] = NULL;
  }
  return;
}


HogFeatures::~HogFeatures()
{
	Clear();
  return;
}

void HogFeatures::Init(IplImage* image)
{
	int width = image->width;
	int height = image->height;
	int i,j,k;

	if(m_gradient_amplitude == NULL || m_gradient_amplitude->width!=width || m_gradient_amplitude->height!=height)
	{
		Clear();
		m_gradient_amplitude = cvCreateMat(height, width, CV_64FC1);
		m_gradient_angle = cvCreateMat(height, width, CV_8UC1);
		for(k=0;k<10;k++)
		{
			m_intergral_gradient[k] = cvCreateMat(height+1,width+1,CV_64FC1);
		}
		m_gradient_x = new int[width*height];
		m_gradient_y = new int[width*height];
		m_gradient_k[9];
		for(k=0;k<9;k++)
		{
			m_gradient_k[k] = new double[width*height];
		}
	}

	double partialsum[10];

	for(k=0;k<9;k++)
	{
		for(i=0;i<height;i++)
		{
			for(j=0;j<width;j++)
			{
				m_gradient_k[k][i*width+j] = 0.0;
			}
		}
	}
	double amplitude;
	float theta;
	int theta_bin;

	for(i=1;i<height-1;i++)
	{
		for(j=1;j<width-1;j++)
		{
			m_gradient_x[i*width+j] = ((unsigned char*)image->imageData)[i*image->widthStep+j+1]-((unsigned char*)image->imageData)[i*image->widthStep+j-1];
			m_gradient_y[i*width+j] = ((unsigned char*)image->imageData)[(i+1)*image->widthStep+j]-((unsigned char*)image->imageData)[(i-1)*image->widthStep+j];
		}
	}
	for(i=1;i<height-1;i++)
	{
		m_gradient_x[i*width] = m_gradient_x[i*width+1];
		m_gradient_y[i*width] = m_gradient_y[i*width+1];
		m_gradient_x[i*width+width-1] = m_gradient_x[i*width+width-2];
		m_gradient_y[i*width+width-1] = m_gradient_y[i*width+width-2];
	}
	for(j=0;j<width;j++)
	{
		m_gradient_x[j] = m_gradient_x[width+j];
		m_gradient_y[j] = m_gradient_y[width+j];
		m_gradient_x[(height-1)*width+j] = m_gradient_x[(height-2)*width+j];
		m_gradient_y[(height-1)*width+j] = m_gradient_y[(height-2)*width+j];
	}
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			amplitude = sqrt(double(m_gradient_x[i*width+j]*m_gradient_x[i*width+j]+m_gradient_y[i*width+j]*m_gradient_y[i*width+j]));
/*			if(m_gradient_x[i*width+j] ==0 && m_gradient_y[i*width+j] ==0)
			{
				angle = 4;			
			}
			else if(m_gradient_x[i*width+j] == 0)
			{
				if(m_gradient_y[i*width+j]<0)
				{
					angle = 0;
				}
				else
				{
					angle = 8;
				}
			}
			else
			{
				angle = cvFloor((atan(double(m_gradient_y[i*width+j]/m_gradient_x[i*width+j]))+PI/2)/PI*9);
			}*/
			theta = atan2(float(m_gradient_y[i*width+j]),float(m_gradient_x[i*width+j]));
			if (theta < 0)	theta = (float)(theta+CV_PI);	// normalize to [0, PI], CV_PI
			if (theta >= CV_PI)	theta = 0.0f;
			//theta = (float)(theta*180.0f/CV_PI);
			theta_bin = cvFloor(9.0*theta/CV_PI);

			m_gradient_amplitude->data.db[i*width+j] = amplitude;
			m_gradient_angle->data.ptr[i*width+j] = theta_bin;
			m_gradient_k[theta_bin][i*width+j] = amplitude;
		}
	}
	
	for(i=0;i<width+1;i++)
	{
		for(k=0;k<10;k++)
		{
			m_intergral_gradient[k]->data.db[i] = 0.0;
		}
	}
	for(i=1;i<height+1;i++)
	{
		for(k=0;k<10;k++)
		{
			partialsum[k] = 0.0;
			m_intergral_gradient[k]->data.db[i*(width+1)]=0.0;
		}
		for(j=1;j<width+1;j++)
		{	
			for(k=0;k<10;k++)
			{
				if(k==0)
				{
					partialsum[k] += m_gradient_amplitude->data.db[(i-1)*width+j-1];
				}
				else
				{
					partialsum[k] += m_gradient_k[k-1][(i-1)*width+j-1];
				}
				m_intergral_gradient[k]->data.db[i*(width+1)+j] = m_intergral_gradient[k]->data.db[(i-1)*(width+1)+j] + partialsum[k];
			}
		}
	}
	return;
}

void HogFeatures::Clear()
{
	int k;
	if(m_gradient_amplitude != NULL)
	{
		cvReleaseMat(&m_gradient_amplitude);
		cvReleaseMat(&m_gradient_angle);
		for(k=0;k<10;k++)
		{
			cvReleaseMat(&(m_intergral_gradient[k]));
		}
		delete []m_gradient_x;
		delete []m_gradient_y;
		for(k=0;k<9;k++)
		{
			delete [](m_gradient_k[k]);
		}
	}
}

// bool HogFeatures::Extract(float* fv,int y, int x)
// {
// 	int i,j,k,m,n,tx,ty,w,h;
// 	int width = m_intergral_gradient[0]->width;
// 	int height = m_intergral_gradient[0]->height;
// 	double norm_L1;
// 	for(i=0;i<m_feature_num;i++)
// 	{
// 		tx = m_hogs[i].x + x;
// 		ty = m_hogs[i].y + y;
// 		w = m_hogs[i].w/2;
// 		h = m_hogs[i].h/2;
// 
// 		n = i*m_feature_size;
// 		for(k=1;k<=9;k++)
// 		{
// 			fv[n++] = m_intergral_gradient[k]->data.db[ty*width+tx] + m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w]
// 				- m_intergral_gradient[k]->data.db[(ty+h)*width+tx] - m_intergral_gradient[k]->data.db[ty*width+tx+w];
// 		}
// 		for(k=1;k<=9;k++)
// 		{
// 			fv[n++] = m_intergral_gradient[k]->data.db[ty*width+tx+w] + m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w*2]
// 			- m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w] - m_intergral_gradient[k]->data.db[ty*width+tx+w*2];
// 		}
// 		for(k=1;k<=9;k++)
// 		{
// 			fv[n++] = m_intergral_gradient[k]->data.db[(ty+h)*width+tx] + m_intergral_gradient[k]->data.db[(ty+h*2)*width+tx+w]
// 			- m_intergral_gradient[k]->data.db[(ty+h*2)*width+tx] - m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w];
// 		}
// 		for(k=1;k<=9;k++)
// 		{
// 			fv[n++] = m_intergral_gradient[k]->data.db[(ty+h)*width+(tx+w)] + m_intergral_gradient[k]->data.db[(ty+h*2)*width+tx+w*2]
// 			- m_intergral_gradient[k]->data.db[(ty+h*2)*width+tx+w] - m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w*2];
// 		}
// 
// 		//normalization L1 norm used
// 		norm_L1 = m_intergral_gradient[0]->data.db[ty*width+tx] + m_intergral_gradient[0]->data.db[(ty+h*2)*width+tx+w*2]
// 		- m_intergral_gradient[0]->data.db[(ty+h*2)*width+tx] - m_intergral_gradient[0]->data.db[ty*width+tx+w*2];
// 		if(norm_L1 != 0 )
// 		{
// 			n = i*m_feature_size;
// 			for(j= 0;j<36;j++)
// 			{
// 				fv[n+j] = fv[n+j]/norm_L1;
// 			}
// 		}
// 		
// 	}
// 	return true;
// }


bool HogFeatures::Extract(float* fv,int y, int x, bool* selected)
{
	int i,j,k,n,tx,ty,w,h;
	int width = m_intergral_gradient[0]->width;
	int height = m_intergral_gradient[0]->height;
	double norm_L1,scale;
	for(i=0;i<m_feature_num;i++)
	{
		if(selected!=NULL && !selected[i])
		{
			continue;
		}
		tx = m_hogs[i].x + x;
		ty = m_hogs[i].y + y;
		w = m_hogs[i].w/2;
		h = m_hogs[i].h/2;

		n = i*m_feature_size;
		for(k=1;k<=9;k++)
		{
			fv[n++] = m_intergral_gradient[k]->data.db[ty*width+tx] + m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w]
			- m_intergral_gradient[k]->data.db[(ty+h)*width+tx] - m_intergral_gradient[k]->data.db[ty*width+tx+w];
		}
		for(k=1;k<=9;k++)
		{
			fv[n++] = m_intergral_gradient[k]->data.db[ty*width+tx+w] + m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w*2]
			- m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w] - m_intergral_gradient[k]->data.db[ty*width+tx+w*2];
		}
		for(k=1;k<=9;k++)
		{
			fv[n++] = m_intergral_gradient[k]->data.db[(ty+h)*width+tx] + m_intergral_gradient[k]->data.db[(ty+h*2)*width+tx+w]
			- m_intergral_gradient[k]->data.db[(ty+h*2)*width+tx] - m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w];
		}
		for(k=1;k<=9;k++)
		{
			fv[n++] = m_intergral_gradient[k]->data.db[(ty+h)*width+(tx+w)] + m_intergral_gradient[k]->data.db[(ty+h*2)*width+tx+w*2]
			- m_intergral_gradient[k]->data.db[(ty+h*2)*width+tx+w] - m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w*2];
		}

		//normalization L1 norm used
		norm_L1 = m_intergral_gradient[0]->data.db[ty*width+tx] + m_intergral_gradient[0]->data.db[(ty+h*2)*width+tx+w*2]
		- m_intergral_gradient[0]->data.db[(ty+h*2)*width+tx] - m_intergral_gradient[0]->data.db[ty*width+tx+w*2];
		//		if(norm_L1 != 0 )
		//{
		scale = 1.f/(norm_L1+1e-3f);
			n = i*m_feature_size;
			for(j= 0;j<36;j++)
			{
				fv[n+j] = fv[n+j]*scale;
			}
			//}
	}
	return true;
}

bool HogFeatures::Extract(float* fv,int y, int x, int* selected, int num)
{
	int i,j,k,n,tx,ty,w,h;
	int width = m_intergral_gradient[0]->width;
	int height = m_intergral_gradient[0]->height;
	double norm_L1,scale;
	for(i=0;i<num;i++)
	{
		j = selected[i];
		tx = m_hogs[j].x + x;
		ty = m_hogs[j].y + y;
		w = m_hogs[j].w/2;
		h = m_hogs[j].h/2;

		n = i*m_feature_size;
		for(k=1;k<=9;k++)
		{
			fv[n++] = m_intergral_gradient[k]->data.db[ty*width+tx] + m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w]
			- m_intergral_gradient[k]->data.db[(ty+h)*width+tx] - m_intergral_gradient[k]->data.db[ty*width+tx+w];
		}
		for(k=1;k<=9;k++)
		{
			fv[n++] = m_intergral_gradient[k]->data.db[ty*width+tx+w] + m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w*2]
			- m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w] - m_intergral_gradient[k]->data.db[ty*width+tx+w*2];
		}
		for(k=1;k<=9;k++)
		{
			fv[n++] = m_intergral_gradient[k]->data.db[(ty+h)*width+tx] + m_intergral_gradient[k]->data.db[(ty+h*2)*width+tx+w]
			- m_intergral_gradient[k]->data.db[(ty+h*2)*width+tx] - m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w];
		}
		for(k=1;k<=9;k++)
		{
			fv[n++] = m_intergral_gradient[k]->data.db[(ty+h)*width+(tx+w)] + m_intergral_gradient[k]->data.db[(ty+h*2)*width+tx+w*2]
			- m_intergral_gradient[k]->data.db[(ty+h*2)*width+tx+w] - m_intergral_gradient[k]->data.db[(ty+h)*width+tx+w*2];
		}

		//normalization L1 norm used
		norm_L1 = m_intergral_gradient[0]->data.db[ty*width+tx] + m_intergral_gradient[0]->data.db[(ty+h*2)*width+tx+w*2]
		- m_intergral_gradient[0]->data.db[(ty+h*2)*width+tx] - m_intergral_gradient[0]->data.db[ty*width+tx+w*2];
		//if(norm_L1 != 0 )
		//{
		scale = 1.f/(norm_L1+1e-3f);
			n = i*m_feature_size;
			for(j= 0;j<36;j++)
			{
				fv[n+j] = fv[n+j]*scale;
			}
			//}
	}
	return true;
}

void HogFeatures::DrawFeature(IplImage* img, int n, CvScalar rgb)
{
     cvRectangle(img, cvPoint(m_hogs[n].x,m_hogs[n].y), cvPoint(m_hogs[n].x+m_hogs[n].w-1,m_hogs[n].y+m_hogs[n].h-1), rgb, 1, 8, 0); 
}

void HogFeatures::ReadFromFile(std::ifstream& f)
{

}

void HogFeatures::WriteToFile(std::ofstream& f)
{

}
