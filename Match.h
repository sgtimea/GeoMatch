#pragma once
#include <opencv2/opencv.hpp>
#include<vector>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/features2d.hpp>
#include<math.h>
#include"txt.h"


/**三角结构体，储存三个顶点**/
typedef struct _TRIANGLE_DESC_
{
	cv::Point2f pt1, pt2, pt3;
	_TRIANGLE_DESC_(const cv::Point2f _pt1, const cv::Point2f _pt2, const cv::Point2f _pt3) :
		pt1(_pt1), pt2(_pt2), pt3(_pt3) {}
}TRIANGLE_DESC;

//参数类
struct option {
	int pointNum;//sift匹配点保留数量
	int width;    //图像宽度
	int heigh;    //图像高度
};

//匹配类
class Match {
public:
	//匹配初始化,配置参数
	void init(const cv::Mat &bytes_left, const cv::Mat &bytes_right,const option &op);

	//sift匹配
	void siftmatch();

	//构造delaunay三角
	void triangle();

	//稠密计算匹配点
	void DenseMatch();

private:

	option op;
	cv::Mat bytes_left;//左图
	cv::Mat bytes_right;//右图
	std::vector<cv::KeyPoint> l_keypoint;
	std::vector<cv::KeyPoint> r_keypoint;//sift特征点
	std::vector<cv::Point2f> l_point;
	std::vector<cv::Point2f> r_point;//sift特征点坐标
	std::vector<cv::DMatch> SiftMatch;//特征点匹配关系
	std::vector<TRIANGLE_DESC> triangles;//delaunay三角剖分结果
	std::vector<cv::Mat> AffineTransform;//仿射变换矩阵，和triangles下标对应
	std::vector<int32_t> TriangleNumber;   //从图像左上角开始逐行存储该像素对应的矩阵下标
	std::vector<cv::Point2f> GeoMatchPt;     //从图像左上角开始逐行存储该像素对应的右图点坐标，不在三角剖分区域内的点对应点均设置(0,0)
};
