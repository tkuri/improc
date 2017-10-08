#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include "Misc/filename_parser.h"

int main(int argc, char** argv)
{
	if(argc < 2) return 1;

	for(int n=1; n<argc; n++){

		if(FilenameParser::getExtension(argv[n]) == "png"){

			cv::Mat image = cv::imread(argv[n], 6);
			if(image.data == NULL) break;
				
			unsigned short width = image.cols;
			unsigned short height = image.rows;

			std::string filename = FilenameParser::removeExtension(argv[n]) + ".bin";
			std::cout << filename << std::endl;

			std::ofstream output;
			output.open(filename, std::ios::binary);
			output.write((char*)&width, sizeof(unsigned short));
			output.write((char*)&height, sizeof(unsigned short));
			output.write((char*)image.data, width * height * sizeof(unsigned short));
		}
		else if(FilenameParser::getExtension(argv[n]) == "bin"){

			std::ifstream input;
			input.open(argv[n], std::ios::binary);
			if(!input) break;

			unsigned short width;
			unsigned short height;
			input.read((char*)&width, sizeof(unsigned short));
			input.read((char*)&height, sizeof(unsigned short));
		
			cv::Mat output = cv::Mat(height, width, CV_16U);
			input.read((char*)output.data, width * height * sizeof(unsigned short));
		
			std::string filename = FilenameParser::removeExtension(argv[n]) + ".png";
			std::cout << filename << std::endl;

			cv::imwrite(filename, output);
		}
		else{
			std::cout << "Invalid file format" << std::endl;
		}
	}

	return 0;
}