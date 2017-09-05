
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

//const int _polarPhase = 3; // 135 top
const int _polarPhase = 2; // 90 top
const int _opbVal = 240;

std::string file;
std::string ofn = "calcSigmaLumRaw.txt";

int GetPolarIndexPat2(const int x, const int y, const int polarPhase){
	int pidx = 0;
	//! 0 deg top
	if(polarPhase==0){
		if(x%2==0 && y%2==0) pidx=0;
		if(x%2==1 && y%2==0) pidx=3;
		if(x%2==0 && y%2==1) pidx=1;
		if(x%2==1 && y%2==1) pidx=2;
	}
	//! 45 deg top
	else if(polarPhase==3){
		if(x%2==0 && y%2==0) pidx=3;
		if(x%2==1 && y%2==0) pidx=0;
		if(x%2==0 && y%2==1) pidx=2;
		if(x%2==1 && y%2==1) pidx=1;
	}
	//! 90 deg top
	else if(polarPhase==2){
		if(x%2==0 && y%2==0) pidx=2;
		if(x%2==1 && y%2==0) pidx=1;
		if(x%2==0 && y%2==1) pidx=3;
		if(x%2==1 && y%2==1) pidx=0;
	}
	//! 135 deg top
	else if(polarPhase==1){
		if(x%2==0 && y%2==0) pidx=1;
		if(x%2==1 && y%2==0) pidx=2;
		if(x%2==0 && y%2==1) pidx=0;
		if(x%2==1 && y%2==1) pidx=3;
	}
	return pidx;
}


template<class T>
int calcSigma( const cv::Mat& src,  int x0, int y0, int w, int h )
{
	std::ofstream ofs( ofn.c_str(), std::ios::app );
	//ofs << "file" << file << std::endl;
	//ofs << "lum,sigma,SNR" << std::endl;

	float snr[4] = {0.0f};
	float lumDC[4] = {0.0f};
	float lumSigma[4] = {0.0f};
	int count[4] = {0};

	for( int y = y0 ; y <= y0 + h ; y++ )
	{
		for( int x = x0 ; x <= x0 + w ; x++ )
		{
			int pidx = GetPolarIndexPat2(x, y, _polarPhase); 

			T v = src.at<T>( y, x );
			float l = static_cast<float>(v);

			lumDC[pidx] += l;
			lumSigma[pidx] += (l * l);
			count[pidx]++;

		}
	}

	for(int p=0; p<4; p++){
		lumDC[p] /= static_cast<float>(count[p]);
		lumSigma[p] /= static_cast<float>(count[p]);
		lumSigma[p] = lumSigma[p] - (lumDC[p] * lumDC[p]);
		lumSigma[p] = std::max(lumSigma[p], 0.0f);
		lumSigma[p] = sqrt(lumSigma[p]);
		if(lumSigma[p] > 0.0f) snr[p] = 20.0f * log10(lumDC[p] / lumSigma[p]);
	}


	ofs << lumDC[0] << "," << lumDC[1] << "," << lumDC[2] << "," << lumDC[3] << ",";
	ofs << lumSigma[0] << "," << lumSigma[1] << "," << lumSigma[2] << "," << lumSigma[3] << std::endl;
	//ofs << lumDC[0] << "," << lumSigma[0] << "," << snr[0] << std::endl;
	//ofs << lumDC[1] << "," << lumSigma[1] << "," << snr[1] << std::endl;
	//ofs << lumDC[2] << "," << lumSigma[2] << "," << snr[2] << std::endl;
	//ofs << lumDC[3] << "," << lumSigma[3] << "," << snr[3] << std::endl;
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
	cv::subtract(img, cv::Scalar(_opbVal), img);

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

