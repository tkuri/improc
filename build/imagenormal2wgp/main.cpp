
#include <Windows.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>
#include <Misc/pixel_array.h>

const float PI = 3.141592654f;

float zenith2dopDiffuse(const float zenith, const float n = 1.6f){
	float dop = sin(zenith) * sin(zenith) * (n - 1.0f / n) * (n - 1.0f / n);
	dop /= 2 + 2 * n*n - sin(zenith)*sin(zenith)*(n + 1.0f / n)*(n + 1.0f / n) + 4 * cos(zenith)*sqrt(n*n - sin(zenith)*sin(zenith));
	return dop;
}

int convWGP2(const cv::Mat_<ushort>& img, const cv::Mat_<cv::Vec3w>& normal, cv::Mat_<ushort>& dst)
{
	pol::PixelArray pixel_array(1, 11);

	for (int y = 0; y < img.rows; y++)
	{
		for( int x = 0 ; x < img.cols ; x++ )
		{
			float c = static_cast<float>(img.at<ushort>(y, x));

			float nx = static_cast<float>(normal.at<cv::Vec3w>(y, x)[2]) / 32767.0f - 32768.0f / 32767.0f;
			float ny = static_cast<float>(normal.at<cv::Vec3w>(y, x)[1]) / 32767.0f - 32768.0f / 32767.0f;
			float nz = static_cast<float>(normal.at<cv::Vec3w>(y, x)[0]) / 32767.0f - 32768.0f / 32767.0f;

			float zenith = acos(nz);
			float azimuth = atan2f(ny, nx);
			if (azimuth < 0) azimuth += 2*PI;
			else if (azimuth >= 2*PI) azimuth -= 2*PI;

			float dop = zenith2dopDiffuse(zenith);
			float v[4];
			v[0] = (c + dop*c*cos(2.0f*(0.0f / 180.0f * PI - azimuth))) / 2.0f;
			v[1] = (c + dop*c*cos(2.0f*(45.0f / 180.0f * PI - azimuth))) / 2.0f;
			v[2] = (c + dop*c*cos(2.0f*(90.0f / 180.0f * PI - azimuth))) / 2.0f;
			v[3] = (c + dop*c*cos(2.0f*(135.0f / 180.0f * PI - azimuth))) / 2.0f;

			dst.at<ushort>(y,x) = static_cast<ushort>(v[pixel_array.GetPolar(x, y)]);

		}
	}

	return 0;
}


int main(int argc, char** argv)
{
	std::string file;
	cv::Mat img;
	cv::Mat normal;

	normal = cv::imread(argv[1], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);
	img = cv::imread(argv[2], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);

	if (img.cols != normal.cols || img.rows != normal.rows){
		std::cout << " Error : Sizes of image and nromal must be equal. ";
		return -1;
	}

	if (normal.channels() != 3){
		std::cout << " Error : The number of normal channel must be 3. ";
		return -1;
	}

	if (img.depth() != 2 || normal.depth() != 2){
		std::cout << " Error : Both images bit must be 16bit. ";
		return -1;
	}

	if (img.channels() != 1){
		cv::cvtColor(img, img, CV_RGB2GRAY);
	}


	cv::Mat_<ushort> dst(img.rows, img.cols);

	if(0){
		cv::Mat_<ushort> buf(img.rows, img.cols, 64*255);
		img = buf.clone();
	}

	convWGP2(img, normal, dst);

	cv::imwrite( argv[3], dst );

	std::cout << "Convert finished. ";


	

	return 0;	
}

