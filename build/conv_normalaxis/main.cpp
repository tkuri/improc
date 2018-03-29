
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

typedef cv::Vec3f TNormal;     //! type of the normal vector
static const int kCvTypeNormal = CV_MAKETYPE(cv::DataType<TNormal>::depth, 3); //! OpenCV type of the normal vector

inline std::string ExtractPathWithoutExt(const std::string &fn)
{
    std::string::size_type pos;
    if((pos = fn.find_last_of(".")) == std::string::npos){
        return fn;
    }
 
    return fn.substr(0, pos);
}


int main( int argc, char** argv )
{
	std::string file;
	cv::Mat img;
	cv::Mat dst;


	for(int n=1; n<argc; n++){
		file = argv[n];
		img = cv::imread( file.c_str(),  CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR );

		std::cout << "normal : " << argv[1] << std::endl;

		int width = img.cols;
		int height = img.rows;
		int dep = img.depth();
		int ch = img.channels();

		dst = cv::Mat( height, width, img.type() );

		if( dep == 0 && ch == 3 ){
			 std::vector<cv::Mat> planes;
			 cv::split(img, planes);
			 cv::Mat xaxis = planes[2].clone();
			 cv::Mat yaxis = planes[1].clone();
			 cv::Mat zaxis = planes[0].clone();

			 //planes[2] = xaxis; //x
			 //planes[1] = zaxis; //y
			 //planes[0] = 255 - yaxis; //z
			 planes[2] = 255 - xaxis; //x
			 planes[1] = yaxis; //y
			 planes[0] = zaxis; //z

			 cv::merge(planes, dst);
		}
		else if( dep == 2 && ch == 3 ){
			 std::vector<cv::Mat> planes;
			 cv::split(img, planes);
			 cv::Mat xaxis = planes[2].clone();
			 cv::Mat yaxis = planes[1].clone();
			 cv::Mat zaxis = planes[0].clone();

			 //planes[2] = xaxis; //x
			 //planes[1] = zaxis; //y
			 //planes[0] = 65535 - yaxis; //z
			 planes[2] = 65535 - xaxis; //x
			 planes[1] = yaxis; //y
			 planes[0] = zaxis; //z

			 cv::merge(planes, dst);
		}
		else
		{
			std::cout << " Error : Invalid depth or channels. ";
			return -1;
		}
		std::string ofile = ExtractPathWithoutExt(file);
		ofile += "_conv.png";

		cv::imwrite( ofile.c_str(), dst);
	}


	std::cout << "Convert finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();


	

	return 0;	
}

