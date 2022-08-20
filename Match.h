#pragma once
#include <opencv2/opencv.hpp>
#include<vector>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/features2d.hpp>
#include<math.h>
#include"txt.h"


/**���ǽṹ�壬������������**/
typedef struct _TRIANGLE_DESC_
{
	cv::Point2f pt1, pt2, pt3;
	_TRIANGLE_DESC_(const cv::Point2f _pt1, const cv::Point2f _pt2, const cv::Point2f _pt3) :
		pt1(_pt1), pt2(_pt2), pt3(_pt3) {}
}TRIANGLE_DESC;

//������
struct option {
	int pointNum;//siftƥ��㱣������
	int width;    //ͼ����
	int heigh;    //ͼ��߶�
};

//ƥ����
class Match {
public:
	//ƥ���ʼ��,���ò���
	void init(const cv::Mat &bytes_left, const cv::Mat &bytes_right,const option &op);

	//siftƥ��
	void siftmatch();

	//����delaunay����
	void triangle();

	//���ܼ���ƥ���
	void DenseMatch();

private:

	option op;
	cv::Mat bytes_left;//��ͼ
	cv::Mat bytes_right;//��ͼ
	std::vector<cv::KeyPoint> l_keypoint;
	std::vector<cv::KeyPoint> r_keypoint;//sift������
	std::vector<cv::Point2f> l_point;
	std::vector<cv::Point2f> r_point;//sift����������
	std::vector<cv::DMatch> SiftMatch;//������ƥ���ϵ
	std::vector<TRIANGLE_DESC> triangles;//delaunay�����ʷֽ��
	std::vector<cv::Mat> AffineTransform;//����任���󣬺�triangles�±��Ӧ
	std::vector<int32_t> TriangleNumber;   //��ͼ�����Ͻǿ�ʼ���д洢�����ض�Ӧ�ľ����±�
	std::vector<cv::Point2f> GeoMatchPt;     //��ͼ�����Ͻǿ�ʼ���д洢�����ض�Ӧ����ͼ�����꣬���������ʷ������ڵĵ��Ӧ�������(0,0)
};
