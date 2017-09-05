
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

std::string ofn = "diff.txt";
std::string file0, file1;

template<class T>
int diff( cv::Mat& src0, cv::Mat& src1, cv::Mat dst )
{
	int cnt = 0;
	double maxDiff = 0.;
	double aveDiff = 0.;

	for( int y = 0 ; y < src0.rows ; y++ )
	{
		for( int x = 0 ; x < src0.cols ; x++ )
		{
			T v0 = src0.at<T>( y, x );
			T v1 = src1.at<T>( y, x );

			double v = static_cast<double>( v0 ) - static_cast<double>( v1 );
			v = fabs( v );

			aveDiff += v;
			if( v > maxDiff ) maxDiff = v;

			if( v > 0 )
			{
				//std::cout << "(" << x << "," << y << ") ";
				//std::cout << cnt << " : " << "img0 = " << v0 << ", img1 = " << v1 << std::endl;
				//std::cout << "diff = " << v << std::endl;
				v = 255;
				cnt++;
			}
			else v = 0;

			dst.at<uchar>( y, x ) = static_cast<uchar>( v );
		}
	}
	aveDiff /= ( src0.rows * src0.cols );

	std::cout << "cntDiff = " << cnt << std::endl;
	std::cout << "aveDiff = " << aveDiff << std::endl;
	std::cout << "maxDiff = " << maxDiff << std::endl;

	std::ofstream ofs( ofn.c_str(), std::ios::app );
	ofs << file0.c_str() << ", " << file1.c_str() << std::endl;
	ofs << "cntDiff = " << cnt << std::endl;
	ofs << "aveDiff = " << aveDiff << std::endl;
	ofs << "maxDiff = " << maxDiff << std::endl;
	ofs.close();


	return 0;
}

template<class T>
int diff3ch( cv::Mat& src0, cv::Mat& src1, cv::Mat dst )
{
	int cnt = 0;
	double maxDiff = 0.;
	double aveDiff = 0.;

	for( int y = 0 ; y < src0.rows ; y++ )
	{
		for( int x = 0 ; x < src0.cols ; x++ )
		{
			T v0 = src0.at<T>( y, x );
			T v1 = src1.at<T>( y, x );

			cv::Vec3d v = static_cast<cv::Vec3d>( v0 ) - static_cast<cv::Vec3d>( v1 );
			
			for( int c = 0 ; c < 3 ; c++ )
				v[c] = fabs( v[c] );

			double vn = sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );

			aveDiff += vn;
			if( vn > maxDiff ) maxDiff = vn;

			double vtmp = 0;

			if( vn > 1 )
			{
				//std::cout << "(" << x << "," << y << ") ";
				//std::cout << cnt << " : " << "img0 = " << v0 << ", img1 = " << v1 << std::endl;
				//std::cout << "diff = " << v << std::endl;
				vtmp = 255;
				cnt++;
			}
			else vtmp = 0;

			dst.at<uchar>( y, x ) = static_cast<uchar>( vtmp );
		}
	}

	aveDiff /= ( src0.rows * src0.cols );

	std::cout << "cntDiff = " << cnt << std::endl;
	std::cout << "aveDiff = " << aveDiff << std::endl;
	std::cout << "maxDiff = " << maxDiff << std::endl;

	std::ofstream ofs( ofn.c_str(), std::ios::app );
	ofs << file0.c_str() << ", " << file1.c_str() << std::endl;
	ofs << "cntDiff = " << cnt << std::endl;
	ofs << "aveDiff = " << aveDiff << std::endl;
	ofs << "maxDiff = " << maxDiff << std::endl;
	ofs.close();
	
	return 0;
}



int main( int argc, char** argv )
{

	cv::Mat img0, img1;
	cv::Mat diffMap;

	if( argc < 3 )
	{
		std::cout << " Error : 2 filename are needed as arguments. ";
		return -1;
	}

	file0 = argv[1];
	file1 = argv[2];

	img0 = cv::imread( file0.c_str(),  CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR );
	img1 = cv::imread( file1.c_str(),  CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR );

	if( ( img0.rows != img1.rows ) || ( img0.cols != img1.cols ) )
	{
		std::cout << " Error : Image sizes are diffrent. ";
		return -1;
	}

	if( img0.depth() != img1.depth() )
	{
		std::cout << " Error : Image depths are diffrent. ";
		return -1;
	}

	if( img0.channels() != img1.channels() )
	{
		std::cout << " Error : Image channels are diffrent. ";
		return -1;
	}

	int width = img0.cols;
	int height = img0.rows;
	int dep = img0.depth();
	int ch = img0.channels();

	diffMap = cv::Mat( height, width, CV_8U );

	if( dep == 0 && ch == 1 ) diff<uchar>( img0, img1, diffMap );
	else if( dep == 2 && ch == 1 ) diff<ushort>( img0, img1, diffMap );
	else if( dep == 0 && ch == 3 ) diff3ch<cv::Vec3b>( img0, img1, diffMap );
	else if( dep == 2 && ch == 3 ) diff3ch<cv::Vec3w>( img0, img1, diffMap );
	else
	{
		std::cout << " Error : Invalid depth or channels. ";
		return -1;
	}

	cv::imwrite( "diffMap.bmp", diffMap );

	std::cout << "Success calc diff " << std::endl;
//	std::cout << "Please return any key.. ";
//	getchar();


	

	return 0;	
}

