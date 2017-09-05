
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

std::string ofn = "profile.csv";

template<class T>
int profile( const cv::Mat& src,  int x0, int y0, int x1, int y1 )
{
	std::ofstream ofs( ofn.c_str(), std::ios::out );
	ofs << "x,y,value" << std::endl;

	for( int y = y0 ; y <= y1 ; y++ )
	{
		for( int x = x0 ; x <= x1 ; x++ )
		{
			T v = src.at<T>( y, x );
			ofs << x << "," << y << "," << v << std::endl;
		}
	}

	ofs.close();

	return 0;
}

template<class T>
int profile3ch( const cv::Mat& src,  int x0, int y0, int x1, int y1 )
{
	std::ofstream ofs( ofn.c_str(), std::ios::out );
	ofs << "x,y,value" << std::endl;

	for( int y = y0 ; y <= y1 ; y++ )
	{
		for( int x = x0 ; x <= x1 ; x++ )
		{
			T v = src.at<T>( y, x );
			ofs << x << "," << y << "," << v << std::endl;
			//ofs << x << "," << y << "," << std::to_string(static_cast<long double>(v[2])) << "," << v[1] << "," << v[0] << std::endl;
		}
	}

	ofs.close();

	return 0;
}

int main( int argc, char** argv )
{
	std::string file;
	cv::Mat img;
	cv::Mat dst;

	file = argv[1];
	int x0 = std::atoi(argv[2]);
	int y0 = std::atoi(argv[3]);
	int x1 = std::atoi(argv[4]);
	int y1 = std::atoi(argv[5]);

	img = cv::imread( file.c_str(),  CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);

	int width = img.cols;
	int height = img.rows;
	int dep = img.depth();
	int ch = img.channels();

	if( dep == 0 && ch == 1 ) profile<uchar>( img, x0, y0, x1, y1 );
	else if( dep == 2 && ch == 1 ) profile<ushort>( img, x0, y0, x1, y1 );
	else if( dep == 0 && ch == 3 ) profile3ch<cv::Vec3b>( img, x0, y0, x1, y1 );
	else if( dep == 2 && ch == 3 ) profile3ch<cv::Vec3w>( img, x0, y0, x1, y1 );
	else
	{
		std::cout << " Error : Invalid depth or channels. ";
		return -1;
	}



	std::cout << "Profile finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();


	

	return 0;	
}

