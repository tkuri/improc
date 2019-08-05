
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

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
	if(argc < 2) return 1;

	std::string file;
	cv::Mat img;
	cv::Mat dst;

	for(int n=1; n<argc; n++){

		file = argv[n];
		//const float gain = static_cast<float>(std::atof(argv[2]));
		//const float gain = 1 / 16.0f;
		const float gain = 2.0f;

		img = cv::imread( file.c_str(),  6 );

		dst = img.clone();

		dst = img * gain;

		std::string ofile = ExtractPathWithoutExt(file);
		//ofile += ".bmp";
		ofile += "_g.png";

		cv::imwrite( ofile.c_str(), dst);
	}

	std::cout << "Profile finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();


	

	return 0;	
}

