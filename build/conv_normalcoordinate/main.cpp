
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

typedef cv::Vec3f TNormal;     //! type of the normal vector
static const int kCvTypeNormal = CV_MAKETYPE(cv::DataType<TNormal>::depth, 3); //! OpenCV type of the normal vector

int convertTangentToCam(const cv::Mat_<cv::Vec3f>& src, cv::Mat_<cv::Vec3f>& dst, const float fx, const float fy, const float cx, const float cy) {

	const auto size = src.size();

	if (dst.size() != size) {
		dst = cv::Mat_<TNormal>(size);
	}

	for (int y = 0; y < size.height; ++y) {
		float tz = 1.0f;
		float ty = (static_cast<float>(y) - cy) / fy * tz;
		for (int x = 0; x < size.width; ++x) {
			float tx = (static_cast<float>(x) - cx) / fx * tz;

			cv::Vec3f cRay(-tx, ty, tz);
			cRay /= cv::norm(cRay);

			cv::Vec3f cCam(0.0f, 0.0f, 1.0f);
			cCam /= cv::norm(cCam);

			cv::Vec3f axis = cCam.cross(cRay);

			const auto srcVec = src(y, x);

			float angle = static_cast<float>(asin(cv::norm(axis)));
			axis /= cv::norm(axis);
			axis *= angle;

			cv::Mat_<float> cRr;
			cv::Rodrigues(axis, cRr);

			cv::Vec3f ov = cv::Vec3f(0.0f);
			ov[0] = cRr(2, 2) * srcVec[0] + cRr(2, 1) * srcVec[1] + cRr(2, 0) * srcVec[2];
			ov[1] = cRr(1, 2) * srcVec[0] + cRr(1, 1) * srcVec[1] + cRr(1, 0) * srcVec[2];
			ov[2] = cRr(0, 2) * srcVec[0] + cRr(0, 1) * srcVec[1] + cRr(0, 0) * srcVec[2];

			dst(y, x) = ov;
		}
	}
	return 0;
}

int convertTangentToCam2(const cv::Mat_<cv::Vec3d>& src, cv::Mat_<cv::Vec3d>& dst, const double fx, const double fy, const double cx, const double cy) {

	const auto size = src.size();

	if (dst.size() != size) {
		dst = cv::Mat_<TNormal>(size);
	}

	for (int y = 0; y < size.height; ++y) {
		double tz = 1.0;
		double ty = (static_cast<double>(y) - cy) / fy * tz;
		for (int x = 0; x < size.width; ++x) {
			double tx = (static_cast<double>(x) - cx) / fx * tz;

			cv::Vec3d cRay(-tx, ty, tz);
			cRay /= cv::norm(cRay);

			cv::Vec3d cCam(0.0, 0.0, 1.0);
			cCam /= cv::norm(cCam);

			cv::Vec3d axis = cCam.cross(cRay);

			const auto srcVec = src(y, x);

			double angle = static_cast<double>(asin(cv::norm(axis)));
			axis /= cv::norm(axis);
			axis *= angle;

			cv::Mat_<double> cRr;
			cv::Rodrigues(axis, cRr);

			cv::Vec3d ov = cv::Vec3d(0.0);
			ov[0] = cRr(2, 2) * srcVec[0] + cRr(2, 1) * srcVec[1] + cRr(2, 0) * srcVec[2];
			ov[1] = cRr(1, 2) * srcVec[0] + cRr(1, 1) * srcVec[1] + cRr(1, 0) * srcVec[2];
			ov[2] = cRr(0, 2) * srcVec[0] + cRr(0, 1) * srcVec[1] + cRr(0, 0) * srcVec[2];

			dst(y, x) = ov;
		}
	}
	return 0;
}


int convertCamToTangent2(const cv::Mat_<cv::Vec3f>& src, cv::Mat_<cv::Vec3f>& dst, const float fx, const float fy, const float cx, const float cy) {

	const auto size = src.size();

	if (dst.size() != size) {
		dst = cv::Mat_<TNormal>(size);
	}

	for (int y = 0; y < size.height; ++y) {
		float tz = 1.0f;
		float ty = (static_cast<float>(y) - cy) / fy * tz;
		float y2 = ty * ty;
		for (int x = 0; x < size.width; ++x) {
			float tx = (static_cast<float>(x) - cx) / fx * tz;
			float x2 = tx * tx;

			float x2_y2 = x2 + y2;
			float sqrt_x2_y2 = std::sqrt(x2_y2);
			float inv_sqrt_x2_y2_z2 = 1.0f / std::sqrt(x2_y2 + tz * tz);
			float inv_x2_y2 = 1.0f / x2_y2;
			float inv_sqrt_x2_y2 = 1.0f / sqrt_x2_y2;

			float sin_theta = -sqrt_x2_y2 * inv_sqrt_x2_y2_z2;
			float cos_theta = tz * inv_sqrt_x2_y2_z2;
			float one_cos_theta = 1 - cos_theta;

			cv::Mat_<float> cRr = cv::Mat_<float>(3, 3);
			cRr.at<float>(0, 0) = cos_theta + one_cos_theta * y2 * inv_x2_y2;
			cRr.at<float>(1, 0) = one_cos_theta * tx * ty * inv_x2_y2;
			cRr.at<float>(2, 0) = tx * sin_theta * inv_sqrt_x2_y2;
			cRr.at<float>(0, 1) = cRr.at<float>(1, 0);
			cRr.at<float>(1, 1) = cos_theta + one_cos_theta * x2 * inv_x2_y2;
			cRr.at<float>(2, 1) = -sin_theta * ty * inv_sqrt_x2_y2;
			cRr.at<float>(0, 2) = -cRr.at<float>(2, 0);
			cRr.at<float>(1, 2) = -cRr.at<float>(2, 1);
			cRr.at<float>(2, 2) = cos_theta;

#if 0
			cv::Vec3f cRay(-tx, ty, tz);
			cRay /= cv::norm(cRay);

			cv::Vec3f cCam(0.0f, 0.0f, 1.0f);
			cCam /= cv::norm(cCam);

			cv::Vec3f axis = cCam.cross(cRay);


			float angle = static_cast<float>(asin(cv::norm(axis)));
			axis /= cv::norm(axis);
			axis *= angle;

			cv::Mat_<float> cRr;
			cv::Rodrigues(axis, cRr);
#endif
			const auto srcVec = src(y, x);

			cv::Vec3f ov = cv::Vec3f(0.0f);
			ov[0] = cRr(2, 2) * srcVec[0] + cRr(2, 1) * srcVec[1] + cRr(2, 0) * srcVec[2];
			ov[1] = cRr(1, 2) * srcVec[0] + cRr(1, 1) * srcVec[1] + cRr(1, 0) * srcVec[2];
			ov[2] = cRr(0, 2) * srcVec[0] + cRr(0, 1) * srcVec[1] + cRr(0, 0) * srcVec[2];

			dst(y, x) = ov;
		}
	}
	return 0;
}

int convertCamToTangent(const cv::Mat_<cv::Vec3f>& src, cv::Mat_<cv::Vec3f>& dst, const float fx, const float fy, const float cx, const float cy) {

	const auto size = src.size();

	if (dst.size() != size) {
		dst = cv::Mat_<TNormal>(size);
	}

	for (int y = 0; y < size.height; ++y) {
		float tz = 1.0f;
		float ty = (static_cast<float>(y) - cy) / fy * tz;
		for (int x = 0; x < size.width; ++x) {
			float tx = (static_cast<float>(x) - cx) / fx * tz;

			cv::Vec3f cRay(-tx, ty, tz);
			cRay /= cv::norm(cRay);

			cv::Vec3f cCam(0.0f, 0.0f, 1.0f);
			cCam /= cv::norm(cCam);

			cv::Vec3f axis = cCam.cross(cRay);

			const auto srcVec = src(y, x);

			float angle = -static_cast<float>(asin(cv::norm(axis)));
			axis /= cv::norm(axis);
			axis *= angle;

			cv::Mat_<float> cRr;
			cv::Rodrigues(axis, cRr);

			cv::Vec3f ov = cv::Vec3f(0.0f);
			ov[0] = cRr(2, 2) * srcVec[0] + cRr(2, 1) * srcVec[1] + cRr(2, 0) * srcVec[2];
			ov[1] = cRr(1, 2) * srcVec[0] + cRr(1, 1) * srcVec[1] + cRr(1, 0) * srcVec[2];
			ov[2] = cRr(0, 2) * srcVec[0] + cRr(0, 1) * srcVec[1] + cRr(0, 0) * srcVec[2];

			dst(y, x) = ov;
		}
	}
	return 0;
}

void usage()
{
	std::cout << "convert normal coordinate" << std::endl;
	std::cout << "usage: " << std::endl;
	std::cout << "xx.exe in_normal_file fx fy cx cy out_normal_file" << std::endl;
}


int main( int argc, char** argv )
{
	std::string file;
	cv::Mat img;
	cv::Mat dst;
	double fx,fy,cx,cy;

	if(argc == 7) {
		file = argv[1];
		img = cv::imread( file.c_str(),  CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_COLOR );
		fx = static_cast<float>(atof(argv[2]));
		fy = static_cast<float>(atof(argv[3]));
		cx = static_cast<float>(atof(argv[4]));
		cy = static_cast<float>(atof(argv[5]));

		std::cout << "inNormal : " << argv[1] << std::endl;
		std::cout << "outNormal : " << argv[6] << std::endl;
		std::cout << "flocal length x : " << fx << std::endl;
		std::cout << "flocal length y : " << fy << std::endl;
		std::cout << "center x : " << cx << std::endl;
		std::cout << "center y : " << cy << std::endl;
	}
	else {
		std::cout << "parameter not enough" << std::endl;
		usage();
		return 1;
	}
		
	int width = img.cols;
	int height = img.rows;
	int dep = img.depth();
	int ch = img.channels();

	dst = cv::Mat( height, width, img.type() );

	cv::Mat fimg;
	cv::Mat_<cv::Vec3d> fdst = cv::Mat_<cv::Vec3d>( height, width );
	if( dep == 0 && ch == 3 ){
		 img.convertTo(fimg, CV_64FC3, 1.0/127.0, -128.0/127.0);
		 convertTangentToCam2(fimg, fdst, fx, fy, cx, cy);
		 fdst.convertTo(dst, CV_8UC3, 127.0, 128.0);
	}
	else if( dep == 2 && ch == 3 ){
		 //img.convertTo(fimg, CV_32FC3, 1.0/32767.0, -32767.0/32768.0);
		 img.convertTo(fimg, CV_64FC3, 1.0/32767.0, -32768.0/32767.0);
		 convertTangentToCam2(fimg, fdst, fx, fy, cx, cy);
		 fdst.convertTo(dst, CV_16UC3, 32767.0, 32768.0);
	}
	else
	{
		std::cout << " Error : Invalid depth or channels. ";
		return -1;
	}


	cv::imwrite(argv[6], dst);

	std::cout << "Convert finished. ";
//	std::cout << "Please return any key.. ";
//	getchar();


	

	return 0;	
}

