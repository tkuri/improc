
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

cv::Vec3b mask0 = cv::Vec3b(0,0,0);

/*!
 * ファイル名から拡張子を削除
 * @param[in] fn ファイル名(フルパス or 相対パス)
 * @return フォルダパス
 */
inline std::string ExtractPathWithoutExt(const std::string &fn)
{
    std::string::size_type pos;
    if((pos = fn.find_last_of(".")) == std::string::npos){
        return fn;
    }
 
    return fn.substr(0, pos);
}

cv::Mat makeMask(const cv::Mat& src)
{
	cv::Mat dst = src.clone();

#pragma omp parallel for
	for( int y = 0 ; y < src.rows ; y++ )
	{
		for( int x = 0 ; x < src.cols ; x++ )
		{
			cv::Vec3b v = src.at<cv::Vec3b>(y, x);
			cv::Vec3b dv;
			if (v == mask0) dv = cv::Vec3b(255, 255, 255);
			else dv = cv::Vec3b(0, 0, 0);
			dst.at<cv::Vec3b>(y, x) = dv;
		}
	}
	return dst;

}


int main( int argc, char** argv )
{
	if(argc < 2) return 1;

	std::string file;
	cv::Mat img;
	cv::Mat dst;


	for(int n=1; n<argc; n++){

		file = argv[n];

		img = cv::imread( file.c_str(),  1 );
		if(img.data==NULL) break;

		dst = makeMask(img);

		std::string ofile = ExtractPathWithoutExt(file);
		ofile += "_mask.png";

		cv::imwrite( ofile.c_str(), dst);
	}

	std::cout << "Profile finished. ";
	

	return 0;	
}

