#include "Match.h"



int main(int argv, char** argc) {

	//if (argv < 3) {
	//	std::cout << "参数过少，请至少指定左右影像路径！" << std::endl;
	//	return -1;
	//}

	//printf("Image Loading...");
	////···············································································//
	//// 读取影像
	//std::string path_left = argc[1];
	//std::string path_right = argc[2];

	//cv::Mat img_left = cv::imread(path_left, cv::IMREAD_COLOR);
	//cv::Mat img_right = cv::imread(path_right, cv::IMREAD_COLOR);

	//if (img_left.data == nullptr || img_right.data == nullptr) {
	//	std::cout << "读取影像失败！" << std::endl;
	//	return -1;
	//}
	//if (img_left.rows != img_right.rows || img_left.cols != img_right.cols) {
	//	std::cout << "左右影像尺寸不一致！" << std::endl;
	//	return -1;
	//}

	cv::Mat img_left = cv::imread("D:\\vsproject\\GeoMatch\\data\\view1.png", 1);
	cv::Mat img_right = cv::imread("D:\\vsproject\\GeoMatch\\data\\view5.png", 1);
	Match match;
	option op;
	op.pointNum = 200;
	op.width = img_left.cols;
	op.heigh = img_left.rows;


	match.init(img_left, img_right,op);
	match.siftmatch();
	match.triangle();
	match.DenseMatch();
	system("pause");
}