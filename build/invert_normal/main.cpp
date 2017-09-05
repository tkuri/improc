
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

//const int zeroLevel = 32767;
//const int zeroLevel = 128;

template<class T, int zeroLevel>
int inv( const cv::Mat& src,  cv::Mat& dst )
{
	for( int y = 0 ; y < src.rows ; y++ )
	{
		for( int x = 0 ; x < src.cols ; x++ )
		{
			T v = src.at<T>( y, x );

			//! Convert x(=R) value
			//if( v[2] < zeroLevel )
			//{
				int d = zeroLevel - v[2];
				v[2] = zeroLevel + d;
			//}
			

			dst.at<T>( y, x ) = v;
		}
	}

	return 0;
}


int main( int argc, char** argv )
{
	std::string file;
	cv::Mat img;
	cv::Mat dst;

	file = argv[1];
	img = cv::imread( file.c_str(),  CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR );

	int width = img.cols;
	int height = img.rows;
	int dep = img.depth();
	int ch = img.channels();

	dst = cv::Mat( height, width, img.type() );

	if( dep == 0 && ch == 3 ) inv<cv::Vec3b, 128>( img, dst );
	else if( dep == 2 && ch == 3 ) inv<cv::Vec3w, 32768>( img, dst );
	else
	{
		std::cout << " Error : Invalid depth or channels. ";
		return -1;
	}

	cv::imwrite( "invNormal.png", dst );

	std::cout << "Convert finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();

	return 0;	
}

