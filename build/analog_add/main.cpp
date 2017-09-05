
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "misc/filename_parser.h"


template<class T>
int analogAdd(const cv::Mat& src, cv::Mat& dst)
{
	int width = src.cols;
	int height = src.rows;

	dst = cv::Mat::zeros(height / 2, width / 2, src.type());

	for(int y = 0 ; y < height / 2 ; y = y + 2)
	{
		int j[2];
		j[0] = std::min(y * 2, height - 1);
		j[1] = std::min(y * 2 + 1, height - 1);
		for(int x = 0 ; x < width / 2 ; x = x + 2)
		{
			int i[2];
			i[0] = std::min(x * 2, width - 1);
			i[1] = std::min(x * 2 + 1, width - 1);

			for(int jj = 0 ; jj < 2 ; jj++) {
				for(int ii = 0 ; ii < 2 ; ii++)	{
					float p0 = static_cast<float>(src.at<T>(j[jj], i[ii] ));
					float p1 = static_cast<float>(src.at<T>(j[jj], i[ii] + 2));
					float p2 = static_cast<float>(src.at<T>(j[jj] + 2, i[ii]));
					float p3 = static_cast<float>(src.at<T>(j[jj] + 2, i[ii] + 2));

					float p = (p0 + p1 + p2 + p3) * 0.25f;

					dst.at<T>(std::min(y + jj, height / 2 - 1), std::min(x + ii, width / 2 - 1)) = static_cast<T>(p);
				}
			}

		}
	}

	return 0;
}

int main( int argc, char** argv )
{
	cv::Mat img;
	cv::Mat dst;


	std::string file = argv[1];

	img = cv::imread( file.c_str(),  CV_LOAD_IMAGE_ANYDEPTH );

	int dep = img.depth();
	int ch = img.channels();

	if( dep == 0 && ch == 1 ) analogAdd<uchar>( img, dst );
	else if( dep == 2 && ch == 1 ) analogAdd<ushort>( img, dst );
	else
	{
		std::cout << " Error : Invalid depth or channels. ";
		return -1;
	}


	std::string filename = FilenameParser::removeExtension(argv[1]) + "_add.png";
	std::cout << filename << std::endl;

	cv::imwrite(filename, dst);

	std::cout << "Process finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();


	

	return 0;	
}

