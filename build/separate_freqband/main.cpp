
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

float gauss(const float d, const float sigma){
	float v = std::exp(-(d * d) / (2.0f * sigma * sigma));
	return v;
}

void LowPassFilter(const cv::Mat_<cv::Vec3w>& src, cv::Mat_<cv::Vec3f>& dst, const int tap)
{
	int cnt = tap / 2;

#pragma omp parallel for
	for( int y = 0 ; y < src.rows ; y++ )
	{
		for( int x = 0 ; x < src.cols ; x++ )
		{
			cv::Vec3f v = 0;
			float w = 0;
			for( int yy = 0 ; yy < tap ; yy++ )
			{
				int j = std::min(std::max(y - cnt + yy, 0), src.rows - 1);
				for( int xx = 0 ; xx < tap ; xx++ )
				{
					int i = std::min(std::max(x - cnt + xx, 0), src.cols - 1);

					float d = static_cast<float>((x - i) * (x - i) + (y - j) * (y - j));
					d = std::sqrt(d);


					v += static_cast<cv::Vec3f>(src.at<cv::Vec3w>(j, i) * gauss(d, 70.0f));
					w += gauss(d, 70.0f);

				}
			}

			if(w > 0) v /= w;

			dst.at<cv::Vec3f>( y, x ) = v;
		}
	}
}

void LowPassFilterBilateral2(const cv::Mat_<cv::Vec3w>& src, cv::Mat_<cv::Vec3f>& dst, const int tap, const float sigma = 15000.0f)
{
	int cnt = tap / 2;

	const int sw = tap;
	const int sh = tap;
	const int swc = sw >> 1;
	const int shc = sh >> 1;

	const int wbit = 65535;
	float weights[wbit];
    int emax;
    for(int i = 0; i < wbit; i++ )
    {
        float v = std::exp(-std::max(i / (2.0f * sigma * sigma) * wbit, 0.0f));
        weights[i] = v;

		//if weight is too small, clip 0.
        if(v < 0.001f)
        {
            emax = i;
            break;
        }
    }
    for(int i = emax; i < wbit; i++ ) weights[i] = 0.0f;


#pragma omp parallel for
	for( int y = 0 ; y < src.rows ; y++ )
	{
		for( int x = 0 ; x < src.cols ; x++ )
		{
			cv::Vec3d v = 0;
			double wsum = 0.0f;
			cv::Vec3f r = static_cast<cv::Vec3f>(src.at<cv::Vec3w>(y, x));

			for( int yy = 0 ; yy < tap ; yy++ )
			{
				int j = std::min(std::max(y - cnt + yy, 0), src.rows - 1);
				for( int xx = 0 ; xx < tap ; xx++ )
				{
					int i = std::min(std::max(x - cnt + xx, 0), src.cols - 1);

					cv::Vec3f t = static_cast<cv::Vec3f>(src.at<cv::Vec3w>(j, i));

					float l2 = static_cast<float>(cv::norm(r, t));
					float w = weights[std::max(std::min(static_cast<int>(l2), wbit), 0)];

					float d = static_cast<float>((x - i) * (x - i) + (y - j) * (y - j));
					d = std::sqrt(d);
					float w2 = gauss(d, 70.0f);

					v += static_cast<cv::Vec3d>(t * w * w2);
					wsum += static_cast<double>(w * w2);

				}
			}

			v = wsum > 0.0 ? v / wsum : static_cast<cv::Vec3d>(r);
			dst.at<cv::Vec3f>( y, x ) = static_cast<cv::Vec3f>(v);
		}
	}
}

void LowPassFilterBilateral(const cv::Mat_<cv::Vec3w>& src, cv::Mat_<cv::Vec3f>& dst, const int tap, const float sigma = 15000.0f)
{
	int cnt = tap / 2;

	const int sw = tap;
	const int sh = tap;
	const int swc = sw >> 1;
	const int shc = sh >> 1;

	const int wbit = 65535;
	float weights[wbit];
    int emax;
    for(int i = 0; i < wbit; i++ )
    {
        float v = std::exp(-std::max(i / (2.0f * sigma * sigma) * wbit, 0.0f));
        weights[i] = v;

		//if weight is too small, clip 0.
        if(v < 0.001f)
        {
            emax = i;
            break;
        }
    }
    for(int i = emax; i < wbit; i++ ) weights[i] = 0.0f;


#pragma omp parallel for
	for( int y = 0 ; y < src.rows ; y++ )
	{
		for( int x = 0 ; x < src.cols ; x++ )
		{
			cv::Vec3f v = 0;
			double wsum = 0.0f;
			cv::Vec3f r = static_cast<cv::Vec3f>(src.at<cv::Vec3w>(y, x));

			for( int yy = 0 ; yy < tap ; yy++ )
			{
				int j = std::min(std::max(y - cnt + yy, 0), src.rows - 1);
				for( int xx = 0 ; xx < tap ; xx++ )
				{
					int i = std::min(std::max(x - cnt + xx, 0), src.cols - 1);

					cv::Vec3f t = static_cast<cv::Vec3f>(src.at<cv::Vec3w>(j, i));

					float l2 = static_cast<float>(cv::norm(r, t));
					float w = weights[std::max(std::min(static_cast<int>(l2), wbit), 0)];

					v += t * w;
					wsum += static_cast<double>(w);

				}
			}

			v = wsum > 0.0 ? v / wsum : r;
			dst.at<cv::Vec3f>( y, x ) = v;
		}
	}
}


void SeparateBandAndMerge(const cv::Mat& src, cv::Mat& dst, float alpha, float beta, int tap)
{
	dst = src.clone();

	cv::Mat_<cv::Vec3f> lowPass = cv::Mat::zeros(src.size(), CV_32FC3);
	cv::Mat_<cv::Vec3f> highPass = cv::Mat::zeros(src.size(), CV_32FC3);

	//LowPassFilter(src, lowPass, tap);
	LowPassFilterBilateral(src, lowPass, tap);
	//LowPassFilterBilateral2(src, lowPass, tap);
	cv::subtract(src, lowPass, highPass);

	cv::Mat tmp = highPass*alpha + lowPass*beta;
	//cv::Mat tmp = cv::abs(highPass*alpha + lowPass*beta);

	tmp.convertTo(dst, CV_16UC3);
	

}


int main( int argc, char** argv )
{
	if(argc < 3) return 1;

	cv::Mat img;
	cv::Mat dst;

	std::string file = argv[1];
	std::string outfile = argv[2];
	float alpha = static_cast<float>(atof(argv[3]));
	float beta = static_cast<float>(atof(argv[4]));
	int tap = atoi(argv[5]);

	img = cv::imread( file.c_str(),  6 );
	SeparateBandAndMerge(img, dst, alpha, beta, tap);

	cv::imwrite( outfile.c_str(), dst);

	std::cout << "Process finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();


	

	return 0;	
}

