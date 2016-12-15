
#include <Windows.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>

std::string file;
std::string ofn = "calcSigmaLum.txt";

template<class T>
int calcSigma( const cv::Mat& src,  int x0, int y0, int w, int h )
{
	std::ofstream ofs( ofn.c_str(), std::ios::app );
	//ofs << file << std::endl;
	//ofs << "lum,sigma,SNR" << std::endl;

	float snr = 0.0f;
	float lumDC = 0.0f;
	float lumSigma = 0.0f;
	float lumRMS = 0.0f;
	int count = 0;

	for( int y = y0 ; y <= y0 + h ; y++ )
	{
		for( int x = x0 ; x <= x0 + w ; x++ )
		{

			T v = src.at<T>( y, x );
			float l = static_cast<float>(v);

			lumDC += l;
			lumSigma += (l * l);
			count++;


		}
	}

	lumDC /= static_cast<float>(count);
	lumRMS = lumSigma;
	lumRMS /= static_cast<float>(count);
	lumRMS = std::sqrt(lumRMS);

	lumSigma /= static_cast<float>(count);
	lumSigma = lumSigma - (lumDC * lumDC);
	lumSigma = std::max(lumSigma, 0.0f);
	lumSigma = sqrt(lumSigma);

	if(lumSigma > 0.0f) snr = 20.0f * log10(lumDC / lumSigma);

	//ofs << lumDC << "," << lumSigma << "," << snr << std::endl;
	//ofs << lumDC << "," << lumRMS <<  "," << lumSigma << std::endl;
	//ofs << lumRMS << std::endl;
	ofs << lumRMS << " ";
	ofs.close();

	return 0;
}

int main( int argc, char** argv )
{
	cv::Mat img;
	cv::Mat dst;

	file = argv[1];
	int x0 = std::atoi(argv[2]);
	int y0 = std::atoi(argv[3]);
	int w = std::atoi(argv[4]);
	int h = std::atoi(argv[5]);

	img = cv::imread( file.c_str(),  CV_LOAD_IMAGE_ANYDEPTH );

	int width = img.cols;
	int height = img.rows;
	int dep = img.depth();
	int ch = img.channels();

	if( dep == 0 && ch == 1 ) calcSigma<uchar>( img, x0, y0, w, h );
	else if( dep == 2 && ch == 1 ) calcSigma<ushort>( img, x0, y0, w, h );
	else
	{
		std::cout << " Error : Invalid depth or channels. ";
		return -1;
	}



	std::cout << "Process finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();


	

	return 0;	
}

