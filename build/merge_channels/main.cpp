
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "misc/filename_parser.h"

template<class T>
int mergeChannels(const cv::Mat& src0, const cv::Mat& src1, const cv::Mat& src2, const cv::Mat& src3, cv::Mat& dst)
{
	int width = src0.cols;
	int height = src0.rows;

	dst = cv::Mat::zeros(height * 2, width * 2, src0.type());

	for(int y = 0 ; y < height ; y++)
	{
		int j[2];
		j[0] = std::min(y * 2, height*2 - 1);
		j[1] = std::min(y * 2 + 1, height*2 - 1);
		for(int x = 0 ; x < width ; x++)
		{
			int i[2];
			i[0] = std::min(x * 2, width*2 - 1);
			i[1] = std::min(x * 2 + 1, width*2 - 1);

			dst.at<T>(j[0], i[0]) = src0.at<T>(y, x);
			dst.at<T>(j[0], i[1]) = src1.at<T>(y, x);
			dst.at<T>(j[1], i[0]) = src2.at<T>(y, x);
			dst.at<T>(j[1], i[1]) = src3.at<T>(y, x);
		}
	}

	return 0;
}

int main( int argc, char** argv )
{
	cv::Mat img0, img1, img2, img3;
	cv::Mat dst;


	std::string file0 = argv[1];
	std::string file1 = argv[2];
	std::string file2 = argv[3];
	std::string file3 = argv[4];

	img0 = cv::imread( file0.c_str(),  CV_LOAD_IMAGE_ANYDEPTH );
	img1 = cv::imread( file1.c_str(),  CV_LOAD_IMAGE_ANYDEPTH );
	img2 = cv::imread( file2.c_str(),  CV_LOAD_IMAGE_ANYDEPTH );
	img3 = cv::imread( file3.c_str(),  CV_LOAD_IMAGE_ANYDEPTH );

	int dep = img0.depth();
	int ch = img0.channels();

	if( dep == 0 && ch == 1 ) mergeChannels<uchar>(img0, img1, img2, img3, dst);
	else if( dep == 2 && ch == 1 ) mergeChannels<ushort>(img0, img1, img2, img3, dst);
	else
	{
		std::cout << " Error : Invalid depth or channels. ";
		return -1;
	}


	std::string filename = FilenameParser::removeExtension(argv[1]) + "mrg.png";
	cv::imwrite(filename, dst);

	std::cout << "Process finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();


	

	return 0;	
}

