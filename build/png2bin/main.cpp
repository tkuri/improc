#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include "Misc/FilenameParser.h"

int main(int argc, char** argv)
{
	if(argc < 2) return 1;

	for(int n=1; n<argc; n++){

		if(FilenameParser::getExtension(argv[n]) == "stb"){

			std::ifstream input;
			input.open(argv[n], std::ios::in | std::ios::binary);
			if(!input.is_open()) break;

			int width;
			int height;
			int dummy;

			input.read((char*)&dummy, sizeof(int));
			input.read((char*)&dummy, sizeof(int));
			input.read((char*)&width, sizeof(int));
			input.read((char*)&height, sizeof(int));
		
			// floatèoóÕ
			//cv::Mat_<float> output(height, width);
			//input.read((char*)output.data, width * height * sizeof(float));

			// intèoóÕ
			cv::Mat_<int> output(height, width);
			input.read((char*)output.data, width * height * sizeof(int));

			std::string filename = FilenameParser::removeExtension(argv[n]) + ".csv";
			std::cout << filename << std::endl;

			std::ofstream ofs(filename);
			ofs << cv::format(output, cv::Formatter::FMT_CSV) << std::endl;

//			imwrite(filename, output);
		}
		else{
			std::cout << "Invalid file format" << std::endl;
		}
	}

	return 0;
}
