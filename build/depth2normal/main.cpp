
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

int	depth2normal(const cv::Mat_<TDepth>& inDepth, const cv::Mat_<TConfidence>& inDepthConf, const uchar confidenceThreshhold, const int taps, const float fl, const float cx, const float cy, cv::Mat_<TNormal>& outNormal)
{
	const int row = inDepth.rows;
	const int col = inDepth.cols;
	cv::Mat_<TDepth> upos = cv::Mat_<TDepth>(row, col);
	cv::Mat_<TDepth> vpos = cv::Mat_<TDepth>(row, col);

	for (int r = 0; r < row; ++r) {
		for (int c = 0; c < col; ++c) {
			upos(r, c) = static_cast<TDepth>(float(c) - cx);
			vpos(r, c) = static_cast<TDepth>(cy - float(r));
		}
	}
	cv::Mat_<TDepth>  uposOrg = upos.clone();
	cv::Mat_<TDepth>  vposOrg = vpos.clone();

	upos = upos.mul(inDepth, 1.0f/fl);
	vpos = vpos.mul(inDepth, 1.0f/fl);

	cv::Mat_<TDepth> uposDevx, uposDevy, vposDevx, vposDevy, depthDevx, depthDevy;
	cv::Sobel(upos, uposDevx, CV_32F, 1, 0, taps);
	cv::Sobel(upos, uposDevy, CV_32F, 0, 1, taps);
	cv::Sobel(vpos, vposDevx, CV_32F, 1, 0, taps);
	cv::Sobel(vpos, vposDevy, CV_32F, 0, 1, taps);
	cv::Sobel(inDepth, depthDevx, CV_32F, 1, 0, taps);
	cv::Sobel(inDepth, depthDevy, CV_32F, 0, 1, taps);

	if (outNormal.empty() || outNormal.rows != row || outNormal.cols != col) {
		outNormal = cv::Mat_<TNormal>(row, col);
	}
	// (uposDevx, vposDevx, depthDevx) x (uposDevy, vposDevy, depthDevy)
	cv::Mat_<TDepth> erodeDepth;
	cv::erode(inDepth, erodeDepth, cv::Mat_<uchar>::ones(taps,taps));
	for (int r = 0; r < row-1; ++r) {
		for (int c = 0; c < col-1; ++c) {
			if (erodeDepth(r, c) < 1.0f || inDepthConf(r, c) < confidenceThreshhold) {
				outNormal(r, c) = 0.0f;
				continue;
			}
			//! original
			outNormal(r, c)[0] = static_cast<float>(uposDevx(r, c) * vposDevy(r, c) - vposDevx(r, c) * uposDevy(r, c)); //nz
			outNormal(r, c)[1] = -1.0f * depthDevx(r, c) * uposDevy(r, c) + uposDevx(r, c) * depthDevy(r, c); //ny
			outNormal(r, c)[2] = -1.0f * vposDevx(r, c) * depthDevy(r, c) + depthDevx(r, c) * vposDevy(r, c); //nx
			outNormal(r, c) = -1.0f * outNormal(r, c);

			//! ER
			//outNormal(r, c)[0] = (uposOrg(r, c)*depthDevx(r, c)/8.0 - vposOrg(r, c)*depthDevy(r, c)/8.0 + inDepth(r, c)) / fl; //nz
			//outNormal(r, c)[1] = -depthDevy(r, c) / 8.0; //ny
			//outNormal(r, c)[2] = depthDevx(r, c) / 8.0; //nx

			//! mihara
			//cv::Vec3f ptx = cv::Vec3f((uposOrg(r,c+1)*inDepth(r,c+1) - uposOrg(r,c)*inDepth(r,c))/fl, (vposOrg(r,c+1)*inDepth(r,c+1) - vposOrg(r,c)*inDepth(r,c))/fl, inDepth(r,c+1)-inDepth(r,c));
			//cv::Vec3f pty = cv::Vec3f((uposOrg(r+1,c)*inDepth(r+1,c) - uposOrg(r,c)*inDepth(r,c))/fl, (vposOrg(r+1,c)*inDepth(r+1,c) - vposOrg(r,c)*inDepth(r,c))/fl, inDepth(r+1,c)-inDepth(r,c));
			//pty = pty * -1;
		 //   outNormal(r, c)[0] = (ptx[0]*pty[1] - ptx[1]*pty[0]); //nz
			//outNormal(r, c)[1] = -(ptx[2]*pty[0] - ptx[0]*pty[2]); //ny
			//outNormal(r, c)[2] = -(ptx[1]*pty[2] - ptx[2]*pty[1]); //nx

			//if (outNormal(r, c)[0] < 0.f) {
			//	outNormal(r, c)[0] = -1.0f * outNormal(r, c)[0];
			//}
			float norm = static_cast<float>(cv::norm(outNormal(r, c), cv::NORM_L2));
			if (norm > 1E-6) {
				outNormal(r, c) = outNormal(r, c) / norm;
			}
		}
	}
	return 0;
}

void usage()
{
	std::cout << "convert depth to normal" << std::endl;
	std::cout << "usage: " << std::endl;
	std::cout << "xx.exe in_depth_file fl cx cy out_normal_file" << std::endl;
}


int main( int argc, char** argv )
{
	usage();

	cv::Mat inDepth; //! 16bit 1ch
	cv::Mat_<TNormal> outNormal; //! 16bit 3ch
	float fl, cx, cy;
	int tap;

	if(argc == 7) {
		inDepth = cv::imread(argv[1], -1);
		fl = static_cast<float>(atof(argv[2]));
		cx = static_cast<float>(atof(argv[3]));
		cy = static_cast<float>(atof(argv[4]));
		tap = atoi(argv[5]);

		std::cout << "depth : " << argv[1] << std::endl;
		std::cout << "normal : " << argv[6] << std::endl;
		std::cout << "flocal length : " << fl << std::endl;
		std::cout << "center x : " << cx << std::endl;
		std::cout << "center y : " << cy << std::endl;
		std::cout << "tap : " << tap << std::endl;
	}
	else {
		std::cout << "parameter not enough" << std::endl;
		return 1;
	}

	//if(inDepth.type() != CV_16U) {
	//	std::cout << "depth bit length must be 16bit 1ch" << std::endl;
	//	return 1;
	//}
	if(inDepth.channels() != 1) {
		std::cout << "depth bit length must be 1ch" << std::endl;
		return 1;
	}

	cv::Mat inDepthConf = inDepth.clone();
	inDepthConf = 255;

	inDepth.convertTo(inDepth, CV_32F);
	//! convert [mm]
	//inDepth.convertTo(inDepth, CV_32F, 4.0f / 65535.0f * 1000.0f);
	depth2normal(inDepth, inDepthConf, 220, tap, fl, cx, cy, outNormal);
	writeNormal(argv[6], outNormal);

	return 0;
}

