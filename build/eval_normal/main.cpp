
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

const float PI = 3.141592654f;

float zenith2dopDiffuse(const float zenith, const float n = 1.6f){
	float dop = sin(zenith) * sin(zenith) * (n - 1.0f / n) * (n - 1.0f / n);
	dop /= 2 + 2 * n*n - sin(zenith)*sin(zenith)*(n + 1.0f / n)*(n + 1.0f / n) + 4 * cos(zenith)*sqrt(n*n - sin(zenith)*sin(zenith));
	return dop;
}

int convNormal2Angle(const cv::Mat_<cv::Vec3w>& normal, cv::Mat_<float>& azimuth, cv::Mat_<float>& zenith )
{
	for (int y = 0; y < normal.rows; y++)
	{
		for( int x = 0 ; x < normal.cols ; x++ )
		{
			float nx = static_cast<float>(normal.at<cv::Vec3w>(y, x)[2]) / 32767.0f - 32768.0f / 32767.0f;
			float ny = static_cast<float>(normal.at<cv::Vec3w>(y, x)[1]) / 32767.0f - 32768.0f / 32767.0f;
			float nz = static_cast<float>(normal.at<cv::Vec3w>(y, x)[0]) / 32767.0f - 32768.0f / 32767.0f;

			float zen = acos(nz);
			float azi = atan2f(ny, nx);
			if (azi < 0) azi += 2*PI;
			else if (azi >= 2*PI) azi -= 2*PI;

			azimuth(y, x) = azi;
			zenith(y, x) = zen;

		}
	}

	return 0;
}

int evalNormal(const cv::Mat_<float>& azimuthGT, const cv::Mat_<float>& zenithGT, const cv::Mat_<float>& azimuth, const cv::Mat_<float>& zenith, const int x0, const int y0, const int width, const int height, float& azimuthError, float& zenithError)
{
	float azimuthDiff(0.0f), zenithDiff(0.0f);
	for (int y = y0; y < y0 + height; ++y)
	{
		for (int x = x0; x < x0 + width; ++x)
		{
			float aziGT = azimuthGT(y, x) * 180.0f / PI;
			float zenGT = zenithGT(y, x) * 180.0f / PI;
			float azi = azimuth(y, x) * 180.0f / PI;
			float zen = zenith(y, x) * 180.0f / PI;

			//azimuthDiff += (aziGT - azi) * (aziGT - azi);
			//zenithDiff  += (zenGT - zen) * (zenGT - zen);			
			azimuthDiff += fabsf(aziGT - azi);
			zenithDiff += fabsf(zenGT - zen);
		}
	}

	azimuthDiff /= width * height;
	zenithDiff /= width * height;

	azimuthError = azimuthDiff;
	zenithError = zenithDiff;

	return 0;
}


void usage(){
	std::cout << "usage: " << std::endl;
	std::cout << "xx.exe normal_file(GT) normal_file x0 y0 width height eval_file(txt,csv)" << std::endl;
}



int main(int argc, char** argv)
{
	if (argc != 8){
		usage();
		return -1;
	}

	cv::Mat normalGT = cv::imread(argv[1], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);
	cv::Mat normal = cv::imread(argv[2], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR);
	const int x0 = atoi(argv[3]);
	const int y0 = atoi(argv[4]);
	const int width = atoi(argv[5]);
	const int height = atoi(argv[6]);
	std::string evalFile = argv[7];


	if (normalGT.cols != normal.cols || normalGT.rows != normal.rows){
		std::cout << " Error : Sizes of image and nromal must be equal. ";
		return -1;
	}

	if (normalGT.channels() != 3 || normal.channels() != 3){
		std::cout << " Error : The number of normal channel must be 3. ";
		return -1;
	}

	if (normalGT.depth() != 2 || normal.depth() != 2){
		std::cout << " Error : Both normal images bit must be 16bit. ";
		return -1;
	}

	cv::Mat_<float> azimuthGT(normalGT.size());
	cv::Mat_<float> zenithGT(normalGT.size());
	cv::Mat_<float> azimuth(normalGT.size());
	cv::Mat_<float> zenith(normalGT.size());

	convNormal2Angle(normalGT, azimuthGT, zenithGT);
	convNormal2Angle(normal, azimuth, zenith);

	float azimuthError(0.0f), zenithError(0.0f);
	evalNormal(azimuthGT, zenithGT, azimuth, zenith, x0, y0, width, height, azimuthError, zenithError);

	std::ofstream ofs(evalFile.c_str(), std::ios::app);
	//ofs << "azimuthError,zenithError" << std::endl;
	ofs << azimuthError << "," << zenithError << std::endl;
	ofs.close();

	std::cout << "Convert finished. ";


	

	return 0;	
}

