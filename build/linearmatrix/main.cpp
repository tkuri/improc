
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

struct LinearMatrixParameter{
	float gain_rr;
	float gain_rg;
	float gain_rb;
	float gain_gr;
	float gain_gg;
	float gain_gb;
	float gain_br;
	float gain_bg;
	float gain_bb;
};
//const LinearMatrixParameter DefaultLinearMatrixParameter{
//	1.887098986f, -0.805330562f, -0.081768424f,
//	-0.508694325f, 1.827249923f, -0.318555598f,
//	0.156677964f, -0.846626875f, 1.689948911f
//};
const LinearMatrixParameter DefaultLinearMatrixParameter{
	1.887098986f, -0.805330562f, -0.081768424f,
	-0.508694325f, 1.827249923f, -0.318555598f,
	0.156677964f, -0.846626875f, 1.689948911f
};


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

cv::Mat LinearMatrix(const cv::Mat& src, const LinearMatrixParameter& parameter = DefaultLinearMatrixParameter)
{
	cv::Mat dst = src.clone();

	const float SAT_TH0 = 0.7f;
	const float SAT_TH1 = 0.95f;
	const float SAT_DIFF_INV = 1.0f / (SAT_TH1 - SAT_TH0);

#pragma omp parallel for
	for( int y = 0 ; y < src.rows ; y++ )
	{
		for( int x = 0 ; x < src.cols ; x++ )
		{
			cv::Vec3f v = src.at<cv::Vec3f>(y, x);
			const float in_r = v[2];
			const float in_g = v[1];
			const float in_b = v[0];

			const float in_max = std::max(std::max(in_r, in_g), in_b);

			float out_r, out_g, out_b;
			if (in_max < SAT_TH0){
				out_r = in_r * parameter.gain_rr + in_g * parameter.gain_rg + in_b * parameter.gain_rb;
				out_g = in_r * parameter.gain_gr + in_g * parameter.gain_gg + in_b * parameter.gain_gb;
				out_b = in_r * parameter.gain_br + in_g * parameter.gain_bg + in_b * parameter.gain_bb;
			}
			else if (in_max < SAT_TH1){
				const float lm_blend = (in_max - SAT_TH0) * SAT_DIFF_INV;
				out_r = (in_r * parameter.gain_rr + in_g * parameter.gain_rg + in_b * parameter.gain_rb) * (1.0f - lm_blend) + in_r * (lm_blend);
				out_g = (in_r * parameter.gain_gr + in_g * parameter.gain_gg + in_b * parameter.gain_gb) * (1.0f - lm_blend) + in_g * (lm_blend);
				out_b = (in_r * parameter.gain_br + in_g * parameter.gain_bg + in_b * parameter.gain_bb) * (1.0f - lm_blend) + in_b * (lm_blend);
			}
			else{
				out_r = in_r;
				out_g = in_g;
				out_b = in_b;
			}

			dst.at<cv::Vec3f>(y, x) = cv::Vec3f(out_b, out_g, out_r);
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

		img = cv::imread( file.c_str(),  6 );
		if(img.data==NULL) break;

		cv::Mat tmp;
		if(img.depth()==2){
			img.convertTo(tmp, CV_32FC3, 1.0f/65535.0f);
		}
		else{
			img.convertTo(tmp, CV_32FC3, 1.0f/255.0f);
		}

		cv::Mat tmp2 = LinearMatrix(tmp);
		tmp = tmp2;

		if(img.depth()==2){
			tmp.convertTo(dst, img.type(), 65535.0f);
		}
		else{
			tmp.convertTo(dst, img.type(), 255.0f);
		}

		std::string ofile = ExtractPathWithoutExt(file);
		ofile += "_lm.png";

		cv::imwrite( ofile.c_str(), dst);
	}

	std::cout << "Profile finished. ";
	

	return 0;	
}

