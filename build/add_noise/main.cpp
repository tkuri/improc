
#include <fstream>
#include <iostream>
#include <random>
#include <opencv2/opencv.hpp>


int addNoise(const cv::Mat_<ushort>& src, const float SN, cv::Mat_<ushort>& dst)
{
	std::random_device seed_gen;
	std::default_random_engine engine(seed_gen());

	for( int y = 0 ; y < dst.rows ; y++ )
	{
		for( int x = 0 ; x < dst.cols ; x++ )
		{
			float v = src(y, x);
			float std = v / powf(10.0f, SN / 20.0f);
			std::normal_distribution<> dist(v, std);
			double result = dist(engine);
			dst(y, x) = static_cast<ushort>(result);
		}
	}

	return 0;
}

void usage(){
	std::cout << "usage: " << std::endl;
	std::cout << "xx.exe input(1ch,16bit) SN[dB] output" << std::endl;
}


int main( int argc, char** argv )
{
	if (argc != 4){
		usage();
		return -1;
	}

	cv::Mat src = cv::imread(argv[1], CV_LOAD_IMAGE_ANYDEPTH);
	if (src.channels() != 1){
		std::cout << " Error : The number of image channel must be 1. ";
		return -1;
	}

	if (src.depth() != 2){
		std::cout << " Error : Image bit must be 16bit. ";
		return -1;
	}

	float SN = static_cast<float>(atof(argv[2]));
	cv::Mat_<ushort> dst(src.size());

	addNoise(src, SN, dst);
	cv::imwrite(argv[3], dst);

	std::cout << "Process finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();	

	return 0;	
}

