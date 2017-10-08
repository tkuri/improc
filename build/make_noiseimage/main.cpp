
#include <fstream>
#include <iostream>
#include <random>
#include <opencv2/opencv.hpp>


int makeNoiseImage( const float mean, const float std, cv::Mat_<ushort>& dst )
{
	std::random_device seed_gen;
	std::default_random_engine engine(seed_gen());
	std::normal_distribution<> dist(mean, std);

	for( int y = 0 ; y < dst.rows ; y++ )
	{
		for( int x = 0 ; x < dst.cols ; x++ )
		{
			double result = dist(engine);
			dst.at<ushort>(y, x) = static_cast<ushort>(result);
		}
	}

	return 0;
}

void usage(){
	std::cout << "usage: " << std::endl;
	std::cout << "xx.exe filename width height mean stddev" << std::endl;
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
	float mean = static_cast<float>(atof(argv[4]));
	float std = static_cast<float>(atof(argv[5]));

	cv::Mat_<ushort> image(height, width);

	makeNoiseImage(mean, std, image);
	cv::imwrite(file, image);

	std::cout << "Process finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();	

	return 0;	
}

