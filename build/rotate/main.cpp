#define _USE_MATH_DEFINES
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>


void usage(){
	std::cout << "usage: " << std::endl;
	std::cout << "xx.exe input rotate_angle(degree) output" << std::endl;
}


int main( int argc, char** argv )
{
	if (argc != 4){
		usage();
		return -1;
	}

	cv::Mat src = cv::imread(argv[1], CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
	//cv::Mat src = cv::imread(argv[1], CV_LOAD_IMAGE_ANYCOLOR);
	if (src.data == NULL){
		std::cout << "input is invalid. " << std::endl;
		return -1;
	}
	double angle = atof(argv[2]);
	double angle_rad = angle / 180.0*M_PI;
	int w = src.cols;
	int h = src.rows;
	cv::Size size(w, h);

	int w_rot = int(std::round(h * std::fabs(std::sin(angle_rad)) + w * std::fabs(std::cos(angle_rad))));
	int h_rot = int(std::round(h * std::fabs(std::cos(angle_rad)) + w * std::fabs(std::sin(angle_rad))));
	cv::Size size_rot(w_rot, h_rot);

	cv::Point2f center(w / 2.0f, h / 2.0f);
	double scale = 1.0;

	cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, angle, scale);
	cv::Mat affine_matrix = rotation_matrix.clone();

	affine_matrix.at<double>(0, 2) = affine_matrix.at<double>(0, 2) - w / 2 + w_rot / 2;
	affine_matrix.at<double>(1, 2) = affine_matrix.at<double>(1, 2) - h / 2 + h_rot / 2;

	cv::Mat img_rot;
	cv::warpAffine(src, img_rot, affine_matrix, size_rot, cv::INTER_CUBIC);

	cv::imwrite(argv[3], img_rot);

	std::cout << "Process finished. " << std::endl;
//	std::cout << "Please return any key.. ";
//	getchar();	

	return 0;	
}

