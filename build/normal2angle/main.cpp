
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <misc/filename_parser.h>

const float PI = 3.141592654f;
#define PROC_8bit 0


int convNormal2Angle(const cv::Mat_<cv::Vec3w>& normal, cv::Mat_<ushort>& azimuth, cv::Mat_<ushort>& zenith)
{
	for (int y = 0; y < normal.rows; y++)
	{
		for (int x = 0; x < normal.cols; x++)
		{
			float nx = static_cast<float>(normal.at<cv::Vec3w>(y, x)[2]) / 32767.0f - 32768.0f / 32767.0f;
			float ny = static_cast<float>(normal.at<cv::Vec3w>(y, x)[1]) / 32767.0f - 32768.0f / 32767.0f;
			float nz = static_cast<float>(normal.at<cv::Vec3w>(y, x)[0]) / 32767.0f - 32768.0f / 32767.0f;

			float azi = atan2f(ny, nx);
			if (azi < 0) azi += 2 * PI;
			else if (azi >= 2 * PI) azi -= 2 * PI;

			float zen = acos(nz);
			if (zen < 0) zen = 0;
			else if (zen > PI/2) zen = PI/2;

			azimuth(y, x) = static_cast<ushort>(azi / 2.0f / PI * 65535.0f);
			zenith(y, x) = static_cast<ushort>(zen * 2.0f / PI * 65535.0f);
		}
	}

	return 0;
}

int convNormal2Angle8bit(const cv::Mat_<cv::Vec3b>& normal, cv::Mat_<ushort>& azimuth, cv::Mat_<ushort>& zenith)
{
	for (int y = 0; y < normal.rows; y++)
	{
		for (int x = 0; x < normal.cols; x++)
		{
			float nx = static_cast<float>(normal.at<cv::Vec3b>(y, x)[2]) / 127.0f - 128.0f / 127.0f;
			float ny = static_cast<float>(normal.at<cv::Vec3b>(y, x)[1]) / 127.0f - 128.0f / 127.0f;
			float nz = static_cast<float>(normal.at<cv::Vec3b>(y, x)[0]) / 127.0f - 128.0f / 127.0f;

			float azi = atan2f(ny, nx);
			if (azi < 0) azi += 2 * PI;
			else if (azi >= 2 * PI) azi -= 2 * PI;

			float zen = acos(nz);
			if (zen < 0) zen = 0;
			else if (zen > PI / 2) zen = PI / 2;

			azimuth(y, x) = static_cast<ushort>(azi / 2.0f / PI * 65535.0f);
			zenith(y, x) = static_cast<ushort>(zen * 2.0f / PI * 65535.0f);
		}
	}

	return 0;
}

void usage(){
	std::cout << "usage: " << std::endl;
	std::cout << "xx.exe normal_file" << std::endl;
}



int main(int argc, char** argv)
{
	for (int n = 1; n < argc; n++){

		cv::Mat normal = cv::imread(argv[n], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);

		if (normal.channels() != 3){
			std::cout << " Error : The number of normal channel must be 3. ";
			return -1;
		}

		//if (normal.depth() != 2){
		//	std::cout << " Error : Both normal images bit must be 16bit. ";
		//	return -1;
		//}

		cv::Mat_<ushort> azimuth(normal.size());
		cv::Mat_<ushort> zenith(normal.size());

		if (normal.depth() == 0){
			std::cout << " 8bit input. ";
			convNormal2Angle8bit(normal, azimuth, zenith);

		}
		else if (normal.depth() == 2){
			std::cout << " 16bit input. ";
			convNormal2Angle(normal, azimuth, zenith);
		}
		else{
			std::cout << " Error : Normal images bit must be 8bit or 16bit. ";
			return -1;
		}


		//convNormal2Angle(normal, azimuth, zenith);
		std::string filename = FilenameParser::removeExtension(argv[n]);


#if PROC_8bit==0
		cv::imwrite(filename + "_azi.png", azimuth);
		cv::imwrite(filename + "_zen.png", zenith);
#else
		cv::Mat_<uchar> azimuth8U(normal.size());
		cv::Mat_<uchar> zenith8U(normal.size());
		azimuth.convertTo(azimuth8U, CV_8U, 255.0f / 65535.0f);
		zenith.convertTo(zenith8U, CV_8U, 255.0f / 65535.0f);
		cv::imwrite(filename + "_azi8u.png", azimuth8U);
		cv::imwrite(filename + "_zen8u.png", zenith8U);
#endif



		std::cout << "Convert finished. ";
	}

	return 0;	
}

