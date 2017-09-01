
#include <Windows.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>

float zenith2dopDiffuse(const float zenith, const float n = 1.6f){
	float dop = sin(zenith) * sin(zenith) * (n - 1.0f / n) * (n - 1.0f / n);
	dop /= 2 + 2 * n*n - sin(zenith)*sin(zenith)*(n + 1.0f / n)*(n + 1.0f / n) + 4 * cos(zenith)*sqrt(n*n - sin(zenith)*sin(zenith));
	return dop;
}

int convWGP2(const cv::Mat_<ushort>& img, const cv::Mat_<cv::Vec3w>& normal, cv::Mat_<ushort>& dst)
{
	for( int y = 0 ; y < img.rows ; y++ )
	{
		for( int x = 0 ; x < img.cols ; x++ )
		{
			float nx = static_cast<float>(normal.at<cv::Vec3f>[2](y, x));
			float ny = static_cast<float>(normal.at<cv::Vec3f>[1](y, x));
			float nz = static_cast<float>(normal.at<cv::Vec3f>[0](y, x));

			float zenith = acos(nz);
			float azimuth = atan2f(ny, nx);
			if (azimuth < 0) azimuth += 360;
			else if (azimuth >= 360) azimuth -= 360;

			float dop = zenith2dopDiffuse(zenith);


		}
	}

	return 0;
}


int main( int argc, char** argv )
{
	std::string file;
	cv::Mat img;
	cv::Mat normal;

	normal = cv::imread( argv[1],  CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR );
	img = cv::imread(argv[2], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);

	if (img.cols != normal.cols || img.rows != normal.rows){
		std::cout << " Error : Sizes of image and nromal must be equal. ";
		return -1;
	}

	if (normal.channels() != 3){
		std::cout << " Error : The number of normal channel must be 3. ";
		return -1;
	}

	if (img.depth() != 2 || normal.depth() != 2){
		std::cout << " Error : Both images bit must be 16bit. ";
		return -1;
	}

	if (img.channels() != 1){
		std::cout << " Error : The number of image channel must be 1. ";
		return -1;
	}

	cv::Mat_<ushort> dst( img.rows, img.cols);

	convWGP2(img, normal, dst);

	cv::imwrite( argv[3], dst );

	std::cout << "Convert finished. ";


	

	return 0;	
}

