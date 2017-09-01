
#include <opencv2/opencv.hpp>
typedef float TDepth;
typedef cv::Vec3f TNormal;
typedef uchar TConfidence;

inline static int imwrite(const std::string& filename, cv::InputArray img)
{
	static std::vector<int> pngParam;
	pngParam.push_back(cv::IMWRITE_PNG_COMPRESSION);
	pngParam.push_back(0);
	return cv::imwrite(filename, img, pngParam);
}

template<typename TMat>
static int writeNormal(const std::string& filename, TMat& normal, int option = 0)
{
	cv::Mat temp;
	switch (option) {	
	case 0:
		normal.convertTo(temp, CV_16U, 32767.0, 32768.0);
		break;
	default:
		normal.convertTo(temp, CV_8U, 127.0, 128.0);
		break;
	}
	return imwrite(filename, temp);
}

int	depthConvCoordinate(const cv::Mat_<TDepth>& inDepth, const float fl, const float cx, const float cy, cv::Mat& outDepth)
{
	const int height = inDepth.rows;
	const int width = inDepth.cols;

	for (int y = 0; y < height-1; ++y) {
		float v = static_cast<float>(y) - cy;
		for (int x = 0; x < width-1; ++x) {
			float u = static_cast<float>(x) - cx;
			auto z = inDepth.at<TDepth>(y, x);
			float w = sqrt(u*u + v*v);
			//float angle = std::atan(w/fl);
			float angle = std::atan(w/fl);
			float zworld = z * std::cos(angle);
			outDepth.at<TDepth>(y, x) = zworld;
		}
	}

	return 0;
}

void usage()
{
	std::cout << "convert depth to world depth" << std::endl;
	std::cout << "usage: " << std::endl;
	std::cout << "xx.exe in_depth_file fl cx cy out_depth_file" << std::endl;
}


int main( int argc, char** argv )
{
	cv::Mat inDepth; //! 16bit 1ch
	cv::Mat outDepth; //! 16bit 1ch
	float fl, cx, cy;

	if(argc == 6) {
		inDepth = cv::imread(argv[1], -1);
		fl = static_cast<float>(atof(argv[2]));
		cx = static_cast<float>(atof(argv[3]));
		cy = static_cast<float>(atof(argv[4]));

		std::cout << "inDepth : " << argv[1] << std::endl;
		std::cout << "outDepth : " << argv[5] << std::endl;
		std::cout << "flocal length : " << fl << std::endl;
		std::cout << "center x : " << cx << std::endl;
		std::cout << "center y : " << cy << std::endl;

		if(inDepth.data == NULL){
			std::cout << "Data pointer is NULL" << std::endl;
			return 1;
		}
	}
	else {
		std::cout << "parameter not enough" << std::endl;
		usage();
		return 1;
	}

	if(inDepth.channels() != 1) {
		std::cout << "depth bit length must be 1ch" << std::endl;
		return 1;
	}

	inDepth.convertTo(inDepth, CV_32F);
	outDepth = inDepth.clone();
	depthConvCoordinate(inDepth, fl, cx, cy, outDepth);
	outDepth.convertTo(outDepth, CV_16U);

	cv::imwrite(argv[5], outDepth);

	return 0;
}

