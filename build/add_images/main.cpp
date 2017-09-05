
#include <Windows.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include "Misc/FilenameParser.h"

template<typename T>
struct FloatType{
	typedef float type;
};
 
template<typename T, int C>
struct FloatType<cv::Vec<T, C>>{
	typedef cv::Vec<float, C> type;
};


template<typename T>
T zeroClip(T v){
	if(v < 0) v = 0;
	return v;
};


template<typename T, int C>
cv::Vec<T, C> zeroClip(cv::Vec<T, C> v){
	for(int c=0; c<C; c++){
		if(v[c] < 0) v[c] = 0;
	}
	return v;
};

template<typename T>
T zeroReverse(T v){
	if(v < 0) v = -v;
	return v;
};


template<typename T, int C>
cv::Vec<T, C> zeroReverse(cv::Vec<T, C> v){
	for(int c=0; c<C; c++){
		if(v[c] < 0) v[c] = -v[c];
	}
	return v;
};



template<typename T>
T add(T v, float offset = 0.0f){
	v = v + static_cast<T>(offset);
	return v;
};


template<typename T, int C>
cv::Vec<T, C> add(cv::Vec<T, C> v, float offset = 0.0f){
	for(int c=0; c<C; c++){
		v[c] = v[c] + static_cast<T>(offset);
	}
	return v;
};




template<class T>
int addImg( cv::Mat& src0, cv::Mat& src1, cv::Mat& dst, const float alpha = 1.0f, const float beta = 1.0f, const float offset = 0.0f)
{
	dst = src0.clone();

	for( int y = 0 ; y < src0.rows ; y++ )
	{
		for( int x = 0 ; x < src0.cols ; x++ )
		{
			auto v0 = static_cast<FloatType<T>::type>(src0.at<T>( y, x ));
			auto v1 = static_cast<FloatType<T>::type>(src1.at<T>( y, x ));

			FloatType<T>::type v = add(v0, offset)*alpha + v1*beta;
			v = add(v, offset);
			//v = zeroReverse(v);

			//dst.at<T>( y, x ) = static_cast<T>( v0*alpha + v1*beta );
			dst.at<T>( y, x ) = static_cast<T>( v );
		}
	}

	return 0;
}


int main( int argc, char** argv )
{
	cv::Mat img0, img1;
	cv::Mat dst;

	std::string file0 = argv[1];
	std::string file1 = argv[2];
	std::string outfile = argv[3];
	float alpha = static_cast<float>(atof(argv[4]));
	float beta = static_cast<float>(atof(argv[5]));
	float offset = static_cast<float>(atof(argv[6]));

	img0 = cv::imread( file0.c_str(), 6 );
	img1 = cv::imread( file1.c_str(), 6 );

	if(img0.type() != img1.type()){
		std::cout << " Error : Inputs type must be equal. ";
		return -1;
	}


	if(img0.type() == CV_8UC1) addImg<uchar>(img0, img1, dst, alpha, beta, offset);
	else if(img0.type() == CV_16UC1) addImg<ushort>(img0, img1, dst, alpha, beta, offset);
	else if(img0.type() == CV_8UC3) addImg<cv::Vec3b>(img0, img1, dst, alpha, beta, offset);
	else if(img0.type() == CV_16UC3) addImg<cv::Vec3w>(img0, img1, dst, alpha, beta, offset);


	cv::imwrite(outfile, dst);

	std::cout << "Process finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();


	

	return 0;	
}

