
#include <opencv2/opencv.hpp>

void generateGridMesh(cv::Mat_<ushort>& depth, cv::Mat& image, float fl, float cx, float cy, float depthMax, std::string outFile)
{
	int row = depth.rows;
	int col = depth.cols;

	FILE* fid;
	fopen_s(&fid, outFile.c_str(),"w");

	cv::Mat vertex = cv::Mat(row, col, CV_32FC3); //! X Y Z
	int vertexNum = 0;

	double min, max;
	cv::minMaxLoc(depth, &min, &max);
	for(int i = 0; i < row; i++) {
		 for(int j = 0; j < col; j ++){
			depth.at<ushort>(i,j) = (depth.at<ushort>(i,j) == 0) ? static_cast<ushort>(max) : depth.at<ushort>(i,j);

			vertex.at<cv::Vec3f>(i,j)[2] = float(depth.at<ushort>(i,j));
			vertex.at<cv::Vec3f>(i,j)[0] = (float(j) - cx) * vertex.at<cv::Vec3f>(i,j)[2] / fl;
			vertex.at<cv::Vec3f>(i,j)[1] = (float(i) - cy) * vertex.at<cv::Vec3f>(i,j)[2] / fl;

			if(vertex.at<cv::Vec3f>(i,j)[2] <= depthMax) vertexNum++;

		}
	}

	fprintf(fid,"ply\n");
	fprintf(fid,"format ascii 1.0\n");
	fprintf(fid,"obj_info num_cols %d\n",row);
	fprintf(fid,"obj_info num_rows %d\n",col);
	fprintf(fid,"element vertex %d\n",vertexNum);
	fprintf(fid,"property float x\n");
	fprintf(fid,"property float y\n");
	fprintf(fid,"property float z\n");
	fprintf(fid,"property uchar red\n");
	fprintf(fid,"property uchar green\n");
	fprintf(fid,"property uchar blue\n");
	fprintf(fid,"element range_grid %d\n",row*col);
	fprintf(fid,"property list uchar int vertex_indices\n");
	fprintf(fid,"end_header\n");

	if(image.type() == CV_8UC1) {
		 for(int j = 0; j < col; j ++) {
			for(int i = 0; i < row; i++) {
				if(vertex.at<cv::Vec3f>(i,j)[2] <= depthMax)
					fprintf(fid,"%f %f %f %d %d %d\n",
					vertex.at<cv::Vec3f>(i,j)[0],
					vertex.at<cv::Vec3f>(i,j)[1],
					vertex.at<cv::Vec3f>(i,j)[2],
					image.at<uchar>(i,j),
					image.at<uchar>(i,j),
					image.at<uchar>(i,j));
			}
		}
	}
	else {
		for(int j = 0; j < col; j ++) {
			for(int i = 0; i < row; i++) {
				if(vertex.at<cv::Vec3f>(i,j)[2] <= depthMax)
					fprintf(fid,"%f %f %f %d %d %d\n",
					vertex.at<cv::Vec3f>(i,j)[0],
					vertex.at<cv::Vec3f>(i,j)[1],
					vertex.at<cv::Vec3f>(i,j)[2],
					image.at<cv::Vec3b>(i,j)[2],
					image.at<cv::Vec3b>(i,j)[1],
					image.at<cv::Vec3b>(i,j)[0]);
			}
		}
	}

	int v = 0;
	for(int j = 0; j < col; j ++) {
			for(int i = 0; i < row; i++) {
				if(vertex.at<cv::Vec3f>(i,j)[2] <= depthMax) {
					fprintf(fid,"%d %d\n", 1, v);
					++v;
				}
				else {
					fprintf(fid,"%d\n", 0);
				}
		}
	}

	fclose(fid);
}

void usage()
{
	std::cout << "grid mesh generation" << std::endl;
	std::cout << "usage: " << std::endl;
	std::cout << "xx.exe depth_file image_file fl cx cy depthmax(clip value) ply_file" << std::endl;
}


int main( int argc, char** argv )
{
	usage();

	cv::Mat inDepth; //! 16bit
	cv::Mat inImage; //! 8bit or 16bit , 1ch or 3ch
	std::string plyFile;
	float fl, cx, cy, depthMax;

	if(argc == 8) {
		inDepth = cv::imread(argv[1], -1);
		inImage = cv::imread(argv[2], -1);
		fl = static_cast<float>(atof(argv[3]));
		cx = static_cast<float>(atof(argv[4]));
		cy = static_cast<float>(atof(argv[5]));
		depthMax = static_cast<float>(atof(argv[6]));
		plyFile = argv[7];

		std::cout << "depth : " << argv[1] << std::endl;
		std::cout << "image : " << argv[2] << std::endl;
		std::cout << "plyFile : " << plyFile << std::endl;
		std::cout << "flocal length : " << fl << std::endl;
		std::cout << "center x : " << cx << std::endl;
		std::cout << "center y : " << cy << std::endl;
		std::cout << "depth max  : " << depthMax << std::endl;
	}
	else {
		std::cout << "parameter not enough" << std::endl;
		return 1;
	}

	if(inDepth.type() != CV_16U) {
		std::cout << "depth bit length must be 16bit 1ch" << std::endl;
		return 1;
	}

	if(inImage.type() == CV_16UC3) {
		inImage.convertTo(inImage, CV_8UC3, 1.0f / 256.0f);
	}
	else if(inImage.type() == CV_16U) {
		inImage.convertTo(inImage, CV_8U, 1.0f/ 256.0f);
	}

	assert(inDepth.rows == inImage.rows && inDepth.cols == inImage.cols);
	assert(inImage.type() == CV_8U || inImage.type() == CV_8UC3);


	generateGridMesh(cv::Mat_<ushort>(inDepth), inImage, fl, cx, cy, depthMax, plyFile);

	return 0;
}

