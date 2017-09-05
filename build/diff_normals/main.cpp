
#include <fstream>
#include <opencv2/opencv.hpp>

std::string ofn = "diff.txt";
std::string file0, file1;
const float kPi = 3.14159265358979f;

template<typename T>
	struct FloatType{
		typedef float type;
};
 
template<typename T, int C>
	struct FloatType<cv::Vec<T, C>>{
		typedef cv::Vec<float, C> type;
};


template<int ZERO, class T>
int normalDiff( cv::Mat_<T>& src0, cv::Mat_<T>& src1, cv::Mat_<ushort>& dst )
{
	cv::Vec3f zero = cv::Vec3f(ZERO, ZERO, ZERO);

	int cnt = 0;
	float maxDiff = 0.;
	float aveDiff = 0.;

	for( int y = 0 ; y < src0.rows ; y++ )
	{
		for( int x = 0 ; x < src0.cols ; x++ )
		{
			cv::Vec3f v0 = static_cast<cv::Vec3f>(src0.at<T>( y, x ));
			cv::Vec3f v1 = static_cast<cv::Vec3f>(src1.at<T>( y, x ));

			v0 -= zero;
			v1 -= zero;

			float vdot = v0.dot(v1);
			float norm0 = static_cast<float>(cv::norm(v0));
			float norm1 = static_cast<float>(cv::norm(v1));
			//float vcos = static_cast<float>(vdot / (cv::norm(v0) * cv::norm(v1)));
			float vcos = static_cast<float>(vdot / (norm0 * norm1));

			float deg = fabs(acos(vcos) * 180.0f / kPi);

			aveDiff += deg;
			if( deg > maxDiff ) maxDiff = deg;

			dst.at<ushort>( y, x ) = static_cast<ushort>( deg );
		}
	}

	aveDiff /= ( src0.rows * src0.cols );

	std::cout << "cntDiff = " << cnt << std::endl;
	std::cout << "aveDiff = " << aveDiff << std::endl;
	std::cout << "maxDiff = " << maxDiff << std::endl;

	std::ofstream ofs( ofn.c_str(), std::ios::app );
	ofs << file0.c_str() << ", " << file1.c_str() << std::endl;
	ofs << "cntDiff = " << cnt << std::endl;
	ofs << "aveDiff = " << aveDiff << std::endl;
	ofs << "maxDiff = " << maxDiff << std::endl;
	ofs.close();
	
	return 0;
}



int main( int argc, char** argv )
{

	cv::Mat img0, img1;

	if( argc < 3 )
	{
		std::cout << " Error : 2 filename are needed as arguments. ";
		return -1;
	}

	file0 = argv[1];
	file1 = argv[2];

	img0 = cv::imread( file0.c_str(),  CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR );
	img1 = cv::imread( file1.c_str(),  CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR );

	if( ( img0.rows != img1.rows ) || ( img0.cols != img1.cols ) )
	{
		std::cout << " Error : Image sizes are diffrent. ";
		return -1;
	}

	if( img0.depth() != img1.depth() )
	{
		std::cout << " Error : Image depths are diffrent. ";
		return -1;
	}

	if( img0.channels() != img1.channels() )
	{
		std::cout << " Error : Image channels are diffrent. ";
		return -1;
	}

	int width = img0.cols;
	int height = img0.rows;
	int dep = img0.depth();
	int ch = img0.channels();

	cv::Mat_<ushort> diffMap = cv::Mat_<ushort>(height, width);

	if( dep == 0 && ch == 3 ) normalDiff<128>(cv::Mat_<cv::Vec3b>(img0), cv::Mat_<cv::Vec3b>(img1), diffMap);
	else if( dep == 2 && ch == 3 ) normalDiff<32768>(cv::Mat_<cv::Vec3w>(img0), cv::Mat_<cv::Vec3w>(img1), diffMap);
	else
	{
		std::cout << " Error : Invalid depth or channels. ";
		return -1;
	}

	cv::imwrite( "normalDiffMap.png", diffMap );

	std::cout << "Success calc normal diff " << std::endl;

	

	return 0;	
}

