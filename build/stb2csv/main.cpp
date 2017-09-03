#include <iostream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include "Misc/FilenameParser.h"

#define OUTPUT_MODE 1 //0:float, 1:int

int writePng16(const std::string& filename, const cv::Mat& img, float alpha = 1.0f, float beta = 0.0f)
{
	cv::Mat tmp;
	img.convertTo(tmp, CV_16U, alpha, beta);
	return cv::imwrite(filename, tmp);
}


template <class T>
int saveSplit(const cv::Mat_<T>& src, const std::string filename, const float alpha=1.0f){
	cv::Mat_<T> dst[4];
	for (int c = 0; c < 4; c++) dst[c] = cv::Mat_<T>::zeros(src.rows/2, src.cols/2);
	for (int y = 0; y < src.rows; y++)
	{
		for (int x = 0; x < src.cols; x++)
		{
			if (!(x % 2) && !(y % 2)) dst[0].at<T>(y/2, x/2) = src.at<T>(y, x);
			else if ((x % 2) && !(y % 2)) dst[1].at<T>(y/2, x/2) = src.at<T>(y, x);
			else if (!(x % 2) && (y % 2)) dst[2].at<T>(y/2, x/2) = src.at<T>(y, x);
			else if ((x % 2) && (y % 2)) dst[3].at<T>(y / 2, x / 2) = src.at<T>(y, x);
		}
	}

	std::ofstream ofs[4];
	ofs[0].open(filename + "_00.csv");
	ofs[1].open(filename + "_01.csv");
	ofs[2].open(filename + "_10.csv");
	ofs[3].open(filename + "_11.csv");
	for (int c = 0; c < 4; c++) ofs[c] << cv::format(dst[c], cv::Formatter::FMT_CSV) << std::endl;

	writePng16(filename + "_00.png", dst[0], alpha);
	writePng16(filename + "_01.png", dst[1], alpha);
	writePng16(filename + "_10.png", dst[2], alpha);
	writePng16(filename + "_11.png", dst[3], alpha);

	return 0;
}

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
		
			std::string filename = FilenameParser::removeExtension(argv[n]);
			std::cout << filename << std::endl;

#if OUTPUT_MODE==0
			cv::Mat_<float> output(height, width);
			input.read((char*)output.data, width * height * sizeof(float));
			saveSplit(output, filename, 255.0f);
#else
			cv::Mat_<int> output(height, width);
			input.read((char*)output.data, width * height * sizeof(int));
			saveSplit(output, filename);
#endif

			std::ofstream ofs(filename + ".csv");
			ofs << cv::format(output, cv::Formatter::FMT_CSV) << std::endl;

			
		}
		else{
			std::cout << "Invalid file format" << std::endl;
		}
	}

	return 0;
}
