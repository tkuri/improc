
#include <Windows.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include "Misc/FilenameParser.h"

template<class T>
int separateChannels(const cv::Mat& src, cv::Mat& dst0, cv::Mat& dst1, cv::Mat& dst2, cv::Mat& dst3)
{
	int width = src.cols;
	int height = src.rows;

	dst0 = cv::Mat::zeros(height / 2, width / 2, src.type());
	dst1 = cv::Mat::zeros(height / 2, width / 2, src.type());
	dst2 = cv::Mat::zeros(height / 2, width / 2, src.type());
	dst3 = cv::Mat::zeros(height / 2, width / 2, src.type());

	for(int y = 0 ; y < height / 2 ; y++)
	{
		int j[2];
		j[0] = std::min(y * 2, height - 1);
		j[1] = std::min(y * 2 + 1, height - 1);
		for(int x = 0 ; x < width / 2 ; x++)
		{
			int i[2];
			i[0] = std::min(x * 2, width - 1);
			i[1] = std::min(x * 2 + 1, width - 1);


			float p0 = static_cast<float>(src.at<T>(j[0], i[0] ));
			float p1 = static_cast<float>(src.at<T>(j[0], i[1] ));
			float p2 = static_cast<float>(src.at<T>(j[1], i[0] ));
			float p3 = static_cast<float>(src.at<T>(j[1], i[1] ));

			dst0.at<T>(y, x) = static_cast<T>(p0);
			dst1.at<T>(y, x) = static_cast<T>(p1);
			dst2.at<T>(y, x) = static_cast<T>(p2);
			dst3.at<T>(y, x) = static_cast<T>(p3);

		}
	}

	return 0;
}

int main( int argc, char** argv )
{
	cv::Mat img;
	cv::Mat dst0, dst1, dst2, dst3;


	std::string file = argv[1];

	img = cv::imread( file.c_str(),  CV_LOAD_IMAGE_ANYDEPTH );

	int dep = img.depth();
	int ch = img.channels();

	if( dep == 0 && ch == 1 ) separateChannels<uchar>( img, dst0, dst1, dst2, dst3 );
	else if( dep == 2 && ch == 1 ) separateChannels<ushort>( img, dst0, dst1, dst2, dst3 );
	else
	{
		std::cout << " Error : Invalid depth or channels. ";
		return -1;
	}


	//std::string filename0 = FilenameParser::removeExtension(argv[1]) + "sep0.png";
	//std::string filename1 = FilenameParser::removeExtension(argv[1]) + "sep1.png";
	//std::string filename2 = FilenameParser::removeExtension(argv[1]) + "sep2.png";
	//std::string filename3 = FilenameParser::removeExtension(argv[1]) + "sep3.png";
	std::string filename0 = FilenameParser::removeExtension(argv[1]) + "sep0.jp2";
	std::string filename1 = FilenameParser::removeExtension(argv[1]) + "sep1.jp2";
	std::string filename2 = FilenameParser::removeExtension(argv[1]) + "sep2.jp2";
	std::string filename3 = FilenameParser::removeExtension(argv[1]) + "sep3.jp2";

	cv::imwrite(filename0, dst0);
	cv::imwrite(filename1, dst1);
	cv::imwrite(filename2, dst2);
	cv::imwrite(filename3, dst3);

	std::cout << "Process finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();


	

	return 0;	
}

