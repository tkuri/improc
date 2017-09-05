
#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include "misc/filename_parser.h"

int main(int argc, char** argv)
{
	if(argc < 2) return 1;

	for(int n=1; n<argc; n++){

		std::cout << "argc:" << argc << std::endl;
		std::cout << "argv[n]:" << argv[n] << std::endl;
		if(FilenameParser::getExtension(argv[n]) == "png"){

			cv::Mat image = cv::imread(argv[n], 6);
			if(image.channels()!=3){
				std::cout << "Image must be 3ch" << std::endl;
				return -1;
			}
				
			unsigned short width = image.cols;
			unsigned short height = image.rows;

			cv::Mat planes[3];
			cv::split(image, planes);

			std::string filename = FilenameParser::removeExtension(argv[n]);
			cv::imwrite(filename + "R.png", planes[2]);
			cv::imwrite(filename + "G.png", planes[1]);
			cv::imwrite(filename + "B.png", planes[0]);

		}
		else{
			std::cout << "Invalid file format" << std::endl;
		}
	}

	return 0;
}
