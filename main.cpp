#include "Match.h"



int main(int argv, char** argc) {

	//if (argv < 3) {
	//	std::cout << "�������٣�������ָ������Ӱ��·����" << std::endl;
	//	return -1;
	//}

	//printf("Image Loading...");
	////��������������������������������������������������������������������������������������������������������������������������������������������������������������//
	//// ��ȡӰ��
	//std::string path_left = argc[1];
	//std::string path_right = argc[2];

	//cv::Mat img_left = cv::imread(path_left, cv::IMREAD_COLOR);
	//cv::Mat img_right = cv::imread(path_right, cv::IMREAD_COLOR);

	//if (img_left.data == nullptr || img_right.data == nullptr) {
	//	std::cout << "��ȡӰ��ʧ�ܣ�" << std::endl;
	//	return -1;
	//}
	//if (img_left.rows != img_right.rows || img_left.cols != img_right.cols) {
	//	std::cout << "����Ӱ��ߴ粻һ�£�" << std::endl;
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