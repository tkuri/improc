
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include "misc/filename_parser.h"

int main(int argc, char** argv)
{
	if(argc < 2) return 1;

	for(int n=1; n<argc; n++){

		if(FilenameParser::getExtension(argv[n]) == "png"){

			cv::Mat image = cv::imread(argv[n], 6);
			if(image.channels()==1){
				cv::Mat buf;
				cv::cvtColor(image, buf, cv::COLOR_GRAY2BGR);
				image = buf.clone();
			}
				
			unsigned short width = image.cols;
			unsigned short height = image.rows;

			std::string filename = FilenameParser::removeExtension(argv[n]) + ".ppm";
			std::cout << filename << std::endl;

			cv::imwrite(filename, image);

		}
		else{
			std::cout << "Invalid file format" << std::endl;
		}
	}

	return 0;
}
