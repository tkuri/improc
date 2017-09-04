
#include <Windows.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>

const float PI = 3.141592654f;

template<typename TMat>
static int writeNormal(const std::string& filename, TMat& normal, int option = 0)
{
	cv::Mat temp;
	switch (option) {
	case 0:
		normal.convertTo(temp, CV_16U, 32767.0, 32768.0);
		break;
	default:
		normal.convertTo(temp, CV_8U, 127.0, 128.0);
		break;
	}
	return cv::imwrite(filename, temp);
}


int makeNormal( const float azimuth, const float zenith, cv::Mat_<cv::Vec3f>& normal )
{
	for( int y = 0 ; y < normal.rows ; y++ )
	{
		for( int x = 0 ; x < normal.cols ; x++ )
		{
			float nx = std::cos(azimuth) * std::sin(zenith);
			float ny = std::sin(azimuth) * std::sin(zenith);
			float nz = std::cos(zenith);

			normal.at<cv::Vec3f>(y, x)[0] = nz;
			normal.at<cv::Vec3f>(y, x)[1] = ny;
			normal.at<cv::Vec3f>(y, x)[2] = nx;

		}
	}

	return 0;
}

void usage(){
	std::cout << "make normal image from azimuth and zeniths" << std::endl;
	std::cout << "usage: " << std::endl;
	std::cout << "xx.exe filename width height azimuth zenith" << std::endl;
}


int main( int argc, char** argv )
{
	if (argc != 6){
		usage();
		return -1;
	}

	std::string file = argv[1];
	int width = atoi(argv[2]);
	int height = atoi(argv[3]);
	float azimuth = static_cast<float>(atof(argv[4]));
	float zenith = static_cast<float>(atof(argv[5]));
	azimuth = azimuth / 180.0f * PI;
	zenith = zenith / 180.0f * PI;

	cv::Mat_<cv::Vec3f> normal(height, width);

	makeNormal(azimuth, zenith, normal);
	writeNormal( file, normal );
	

	std::cout << "Process finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();


	

	return 0;	
}

