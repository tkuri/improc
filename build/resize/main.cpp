
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>



void usage(){
	std::cout << "usage: " << std::endl;
	std::cout << "xx.exe input resize_width resize_height output" << std::endl;
}


int main( int argc, char** argv )
{
	if (argc != 5){
		usage();
		return -1;
	}

	//cv::Mat src = cv::imread(argv[1], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	cv::Mat src = cv::imread(argv[1], CV_LOAD_IMAGE_ANYCOLOR);
	if (src.data == NULL){
		std::cout << "input is invalid. " << std::endl;
		return -1;
	}
	int width = atoi(argv[2]);
	int height = atoi(argv[3]);
	cv::Size size(width, height);

	cv::Mat dst(size, src.type());

	cv::resize(src, dst, size);
	cv::imwrite(argv[4], dst);

	std::cout << "Process finished. " << std::endl;
//	std::cout << "Please return any key.. ";
//	getchar();	

	return 0;	
}

