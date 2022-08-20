#include "Match.h"


void Match::init(const cv::Mat &bytes_left, const cv::Mat &bytes_right, const option &op){
	this->bytes_left = bytes_left;
	this->bytes_right = bytes_right;
	this->op = op;

}


void Match::siftmatch() {
	if (bytes_left.empty() || bytes_right.empty()) {
		std::cout << "error: cannot reading image: " << std::endl;
	}
	// sift特征提取
	std::vector<cv::KeyPoint> keyPoint1, keyPoint2;
	cv::Ptr<cv::Feature2D> feature = cv::SIFT::create(op.pointNum);  // 提取pointNum个特征点

	feature->detect(bytes_left, keyPoint1);
	feature->detect(bytes_right, keyPoint2);
	std::cout << "detect feature ok\n";

	cv::Mat descor1, descor2;
	feature->compute(bytes_left, keyPoint1, descor1);
	feature->compute(bytes_right, keyPoint2, descor2);
	std::cout << "generate  descor ok\n";

	//绘制特征点(关键点)
	cv::Mat feature_pic1, feature_pic2;
	cv::drawKeypoints(bytes_left, keyPoint1, feature_pic1, cv::Scalar(0, 255, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	cv::drawKeypoints(bytes_right, keyPoint1, feature_pic2, cv::Scalar(0, 255, 0), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
	//显示结果
	//cv::imwrite("feature1.jpg", feature_pic1);
	//cv::imwrite("feature2.jpg", feature_pic2);

	cv::FlannBasedMatcher matcher;  //实例化FLANN匹配器
	std::vector<cv::DMatch>matches;   //定义匹配结果变量
	matcher.match(descor1, descor2, matches);  //实现描述符之间的匹配
	std::cout << "original match numbers: " << matches.size() << std::endl;

	cv::Mat oriMatchRes;
	cv::drawMatches(bytes_left, keyPoint1, bytes_right, keyPoint2, matches, oriMatchRes,
		cv::Scalar(0, 255, 0), cv::Scalar::all(-1));
	cv::imwrite("oriMatchResult.jpg", oriMatchRes);

	double sum = 0;
	double max_dist = 0;
	double min_dist = 100;
	for (int i = 0; i < matches.size(); i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist)
			min_dist = dist;
		if (dist > max_dist)
			max_dist = dist;
	}
	std::cout << "max distance: " << max_dist << std::endl;
	std::cout << "min distance: " << min_dist << std::endl;

	//筛选出较好的匹配点  
	std::vector<cv::DMatch> goodMatches;
	double dThreshold = 0.7;    //匹配的阈值，越大匹配的点数越多
	for (int i = 0; i < matches.size(); i++) {
		if (matches[i].distance < dThreshold * max_dist) {
			goodMatches.push_back(matches[i]);
		}
	}

	//RANSAC 消除误匹配特征点 主要分为三个部分：
	//1）根据matches将特征点对齐,将坐标转换为float类型
	//2）使用求基础矩阵方法findFundamentalMat,得到RansacStatus
	//3）根据RansacStatus来将误匹配的点也即RansacStatus[i]=0的点删除

	//根据matches将特征点对齐,将坐标转换为float类型
	std::vector<cv::KeyPoint> R_keypoint01, R_keypoint02;
	for (int i = 0; i < goodMatches.size(); i++) {
		R_keypoint01.push_back(keyPoint1[goodMatches[i].queryIdx]);
		R_keypoint02.push_back(keyPoint2[goodMatches[i].trainIdx]);
		// 这两句话的理解：R_keypoint1是要存储img01中能与img02匹配的特征点，
		// matches中存储了这些匹配点对的img01和img02的索引值
	}

	//坐标转换
	std::vector<cv::Point2f> p01, p02;
	for (int i = 0; i < goodMatches.size(); i++) {
		p01.push_back(R_keypoint01[i].pt);
		p02.push_back(R_keypoint02[i].pt);
	}

	//计算基础矩阵并剔除误匹配点
	std::vector<uchar> RansacStatus;
	cv::Mat Fundamental = findFundamentalMat(p01, p02, RansacStatus, CV_RANSAC);
	cv::Mat dst;
	warpPerspective(bytes_left, dst, Fundamental, cv::Size(bytes_left.cols, bytes_left.rows));
	//cv::imwrite("epipolarImage.jpg", dst); // 核线影像

	// 剔除误匹配的点对
	std::vector<cv::KeyPoint> RR_keypoint01, RR_keypoint02;
	// 重新定义RR_keypoint 和RR_matches来存储新的关键点和匹配矩阵
	std::vector<cv::DMatch> RR_matches;
	int index = 0;
	for (int i = 0; i < goodMatches.size(); i++) {
		if (RansacStatus[i] != 0) {
			RR_keypoint01.push_back(R_keypoint01[i]);
			RR_keypoint02.push_back(R_keypoint02[i]);
			goodMatches[i].queryIdx = index;
			goodMatches[i].trainIdx = index;
			RR_matches.push_back(goodMatches[i]);
			index++;
		}
	}
	l_keypoint.assign(RR_keypoint01.begin(), RR_keypoint01.end()); 
	r_keypoint.assign(RR_keypoint02.begin(), RR_keypoint02.end());
	SiftMatch.assign(RR_matches.begin(), RR_matches.end());//保存特征点与匹配关系


	/**   把keypoint转成point2f   **/
	for (int i = 0; i < l_keypoint.size(); i++) {
		l_point.push_back(l_keypoint[i].pt);
		r_point.push_back(r_keypoint[i].pt);
	}
	if (l_point.size() != l_keypoint.size() || r_point.size() != r_keypoint.size())
		std::cout << "特征点拷贝错误" << std::endl;

	std::cout << "refine match pairs: " << SiftMatch.size() << std::endl;
	// 画出消除误匹配后的图
	cv::Mat img_RR_matches;
	cv::drawMatches(bytes_left, l_keypoint, bytes_right, r_keypoint, SiftMatch, img_RR_matches,
		cv::Scalar(0, 255, 0), cv::Scalar::all(-1));
	cv::imwrite("refineMatchResult.jpg", img_RR_matches);

}


//计算三角剖分
std::vector<TRIANGLE_DESC> delaunayAlgorithm(const cv::Rect boundRc, const std::vector<cv::Point2f>& points)
{
	if (points.empty())
	{
		return std::vector<TRIANGLE_DESC>();
		std::cout << "三角剖分点集为空";
	}
	std::vector<TRIANGLE_DESC> result;

	std::vector<cv::Vec6f> _temp_result;
	cv::Subdiv2D subdiv2d(boundRc);
	for (const auto point : points)
	{
		subdiv2d.insert(cv::Point2f(point.x, point.y));
	}
	//计算三角剖分
	subdiv2d.getTriangleList(_temp_result);

	//存储三角顶点
	for (const auto& _tmp_vec : _temp_result)
	{
		cv::Point2f pt1(_tmp_vec[0], _tmp_vec[1]);
		cv::Point2f pt2(_tmp_vec[2], _tmp_vec[3]);
		cv::Point2f pt3(_tmp_vec[4], _tmp_vec[5]);
		result.push_back(TRIANGLE_DESC(pt1, pt2, pt3));
	}
	return result;
}


void trian(const cv::Mat& image, const std::vector<cv::Point2f>& keypoint, std::string name, std::vector<TRIANGLE_DESC>& tri) {
	const int width = image.cols;
	const int height = image.rows;


	const cv::Rect pageRc(0, 0, width, height);
	auto triangles = delaunayAlgorithm(pageRc, keypoint);

	//画出三角剖分结果并保存
	for (const auto& triangle : triangles)
	{
		line(image, triangle.pt1, triangle.pt2, cv::Scalar(0, 255, 0));
		line(image, triangle.pt1, triangle.pt3, cv::Scalar(0, 255, 0));
		line(image, triangle.pt2, triangle.pt3, cv::Scalar(0, 255, 0));
	}
	cv::imwrite(name, image);
	tri = triangles;
}



void Match::triangle() {
	
	trian(bytes_left, l_point, "triangle.jpg", Match::triangles);
	std::cout << "共建立" << triangles.size() << "个三角形"<<std::endl;
	
}



    /**    计算并保存仿射矩阵  **/

void Save_Mat(std::vector<TRIANGLE_DESC> triangles, std::vector<cv::Point2f> l_point, std::vector<cv::Point2f> r_point, std::vector<cv::Mat> &AffineTransform) {

	/**  取出对应的三个顶点 **/
	cv::Point2f left_point[3], right_point[3]; int count = 0; int ans;
	/**   将矩阵写到txt   **/
	std::ofstream location_out;
	location_out.open("Martix.txt", std::ios::out | std::ios::app);  //以写入和在文件末尾添加的方式打开.txt文件，没有的话就创建该文件。
	if (!location_out.is_open())
		std::cout << "txt文件打开失败" << std::endl;

	/**     把矩阵导出到txt      **/
	for (auto& i : triangles) {

		left_point[0] = i.pt1;
		left_point[1] = i.pt2;
		left_point[2] = i.pt3;

		//根据匹配关系提取右图对应点
		auto result = std::find(l_point.begin(), l_point.end(), left_point[0]);
		ans = result - l_point.begin();
		right_point[0] = r_point[ans];

		result = std::find(l_point.begin(), l_point.end(), left_point[1]);
		ans = result - l_point.begin();
		right_point[1] = r_point[ans];

		result = std::find(l_point.begin(), l_point.end(), left_point[2]);
		ans = result - l_point.begin();
		right_point[2] = r_point[ans];

		//三对匹配点计算仿射矩阵并保存
		AffineTransform.push_back(cv::getAffineTransform(left_point, right_point));
		location_out << AffineTransform.back() << std::endl ;
	}
	location_out.close();

}

            /**如果三角形逆时针方向有一点都在三条边左侧，则该点在三角形内**/
float product(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3) {
	//首先根据坐标计算p1p2和p1p3的向量，然后再计算叉乘
	//p1p2 向量表示为 (p2.x-p1.x,p2.y-p1.y)
	//p1p3 向量表示为 (p3.x-p1.x,p3.y-p1.y)
	return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

            /**判断一点是否在三角形内**/
bool isInTriangle(cv::Point2f p1, cv::Point2f p2, cv::Point2f p3, cv::Point2f o) {
	//保证p1，p2，p3是逆时针顺序
	if (product(p1, p2, p3) < 0) return isInTriangle(p1, p3, p2, o);
	if (product(p1, p2, o) > 0 && product(p2, p3, o) > 0 && product(p3, p1, o) > 0)
		return true;
	return false;
}


      /**    遍历计算每个像素所在的三角形  **/
std::vector<int32_t> Compute_Triangle(int32_t rows,int32_t cols,const std::vector<TRIANGLE_DESC> &triangles){
	cv::Point2f target; 
	std::vector<int32_t> temp;
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			target.x = x; target.y = y; bool mark = false;
			for (int i = 0; i < triangles.size();i++) {
				if (isInTriangle(triangles[i].pt1, triangles[i].pt2, triangles[i].pt3, target))
				{
					temp.push_back(i);                      //将三角形下标记录
					mark = true; break;
				}                                 
			}
			if(!mark)
			temp.push_back(triangles.size());      //若不在任何一个三角形内，将下标设置为triangles.size()
		}
	}

	//将像素点对应三角形情况可视化展示
	cv::Mat img = cv::Mat::zeros(rows, cols, CV_8UC1); float num = 0;
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			num = (float)temp[col + row * cols] / triangles.size();
			img.at<uchar>(row, col) = 100*num+155;
		}
	}
	cv::imwrite("TriangleBW.jpg",img);
	return temp;
}

       /**    从txt读取仿射矩阵     **/
cv::Mat ReadMat(std::string file,int32_t pix) {
	
	cv::Mat A = cv::Mat::ones(2, 3, CV_32FC1);
	string line1 = ReadLine(file, 2 * pix+1);
	string line2 = ReadLine(file, 2 * pix+2);
	std::vector<int32_t> str = find_pix(line1, ",");
	A.at<float>(0, 0) = atof(substring(line1, 1, str[0] - 1).c_str());
	A.at<float>(0, 1) = atof(substring(line1, str[0] + 2, str[1] - 1).c_str());
	A.at<float>(0, 2) = atof(substring(line1, str[1] + 2, line1.length()- 2).c_str());
	str.clear();
	str = find_pix(line2, ",");
	A.at<float>(1, 0) = atof(substring(line2, 1, str[0] - 1).c_str());
	A.at<float>(1, 1) = atof(substring(line2, str[0] + 2, str[1] - 1).c_str());
	A.at<float>(1, 2) = atof(substring(line2, str[1] + 2, line2.length() - 2).c_str());
	return A;
}



       /**       计算右图的几何对应点坐标        **/
std::vector<cv::Point2f> ComputeGeoPt(const std::vector<int32_t> &TriangleNumber,const std::vector<cv::Mat> &AffineTransform,const option &op) {
	std::vector<cv::Point2f> GeoMatchPt; int i = 0;
	cv::Mat l_point = cv::Mat::ones(3, 1, CV_32FC1); cv::Mat B,AB;
	cv::Point2f save;
	for (int y = 0; y < op.heigh; y++) {
		for (int x = 0; x < op.width; x++) {
			if (TriangleNumber[i] < AffineTransform.size()) {
				l_point.at<float>(0, 0) = x;
				l_point.at<float>(1, 0) = y;
				l_point.at<float>(2, 0) = 1;
				B = ReadMat("Martix.txt", TriangleNumber[i]);  
				AB = B * l_point;             //      X右                           X左
				save.x = AB.at<float>(0, 0);  //      Y右(2*1)   =   M(2*3)    *    Y左(3*1)
				save.y = AB.at<float>(1, 0);  //                                     1
			}
			else {
				save.x = 0;
				save.y = 0;
			}
			GeoMatchPt.push_back(save);
			i++;
		}
	}
	std::ofstream location_out;
	location_out.open("MatchPoint.txt", std::ios::out | std::ios::app);
	i = 0;
	for (int y = 0; y < op.heigh; y++) {
		for (int x = 0; x < op.width; x++) {
			location_out << x << "," << y <<"---" << GeoMatchPt[i].x << "," << GeoMatchPt[i].y << std::endl;
			i++;
		}
	}
	location_out.close();
	return GeoMatchPt;
}



void Match::DenseMatch() {
	Save_Mat(triangles, l_point,r_point, AffineTransform);
	TriangleNumber = Compute_Triangle(op.heigh, op.width, triangles);
	GeoMatchPt=ComputeGeoPt(TriangleNumber, AffineTransform,op);
}



