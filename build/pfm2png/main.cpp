
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "misc/filename_parser.h"
#include "misc/pfm_readwrite.h"

//const float pfmMin = -1; //normal
const float pfmMin = 0; //image
const float pfmMax = 1;
const int pngMax = (1 << 16) - 1;

int main(int argc, char** argv)
{
	if(argc < 2) return 1;

	for(int n=1; n<argc; n++){

		if(FilenameParser::getExtension(argv[n]) == "pfm"){

			//cv::Mat image = cv::imread(argv[n], 6);
			cv::Mat image = loadPFM(argv[n]);
			if (image.data == NULL) break;
			if(image.channels()==1){
				cv::Mat buf;
				cv::cvtColor(image, buf, cv::COLOR_GRAY2BGR);
				image = buf.clone();
			}
				
			unsigned short width = image.cols;
			unsigned short height = image.rows;

			std::string filename = FilenameParser::removeExtension(argv[n]) + ".png";
			std::cout << filename << std::endl;

			image = (image - pfmMin) / (pfmMax - pfmMin) * pngMax;
			image.convertTo(image, CV_16UC3);

			cv::imwrite(filename, image);

		}
		else if(FilenameParser::getExtension(argv[n]) == "png"){

			cv::Mat image = cv::imread(argv[n], 6);
			if(image.data == NULL) break;
			if(image.channels()==1){
				cv::Mat buf;
				cv::cvtColor(image, buf, cv::COLOR_GRAY2BGR);
				image = buf.clone();
			}
				
			unsigned short width = image.cols;
			unsigned short height = image.rows;

			std::string filename = FilenameParser::removeExtension(argv[n]) + ".pfm";
			std::cout << filename << std::endl;

			image.convertTo(image, CV_32FC3);
			image = image / pngMax * (pfmMax - pfmMin) + pfmMin;
			savePFM(image, filename);

		}
		else{
			std::cout << "Invalid file format" << std::endl;
		}
	}

	return 0;
}
