//暂时确定为装甲片任务
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace std;
using namespace cv;

//得到旋转矩阵中心点
Point getRectCenter(RotatedRect temp)
{
    Point2f edge_points[4];
    temp.points(edge_points);
    return Point((edge_points[0].x + edge_points[2].x)/2,(edge_points[0].y + edge_points[2].y)/2);
}

//旋转矩阵左上角点
Point getRectLeft_up(RotatedRect temp, Point center)
{
    float x = center.x - temp.size.height/2;
    float y = center.y - temp.size.width/2;
    return Point(x, y);
}

//旋转矩阵左下角点
Point getRectLeft_down(RotatedRect temp, Point center)
{
    float x = center.x + temp.size.height/2;
    float y = center.y - temp.size.width/2;
    return Point(x, y);
}

//旋转矩阵右上角点
Point getRectRight_up(RotatedRect temp, Point center)
{
    float x = center.x - temp.size.height/2;
    float y = center.y + temp.size.width/2;
    return Point(x, y);
}

//旋转矩阵右下角点
Point getRectRight_down(RotatedRect temp, Point center)
{
    float x = center.x + temp.size.height/2;
    float y = center.y + temp.size.width/2;
    return Point(x, y);
}

int main(int argc, char** argv)
{
    double start = static_cast<double>(getTickCount());
    Mat frame;
    Mat result;
    Mat hsv;
    Mat Image1;
    Mat Image2;
    //Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));

    //vector<Mat>channels;
    Rect rect,rect_1,rect_2,rect_3;
    RotatedRect rotate_Rect, rotate_Rect1, rotate_Rect2;
    vector<vector<Point>> contours;
    vector<vector<Point>> contours1,contours2, contours3;
    vector<Vec4i> hierarchy;
    vector<Rect> rect_Group, rect_Group1, rect_Group2, rect_Group3;
    vector<RotatedRect> rotate_Rect_Group, rotate_Rect_Group1, rotate_Rect_Group2;
    Point center1,center2,center3,center4;
    Point left_up, left_down, right_up, right_down;

    VideoCapture video("../视频/armor.mp4");
    if (!video.isOpened())  //对video进行异常检测  
	{
		cout << "video open error!" << endl;
		return 0;
	}
    // 获取帧数
	int frameCount = video.get(CV_CAP_PROP_FRAME_COUNT);
    //一帧一帧读入视频
    for (int i = 0; i < frameCount; i++)
    {
        video.read(frame);
        result = frame.clone();
        GaussianBlur(frame, Image1, Size(3,3),0);
        cvtColor(Image1, hsv, COLOR_BGR2HSV);      //转化为HSV图像
        inRange(hsv, Scalar(60,43,46),Scalar(125,255,255),Image1);      //提取出色域为蓝色范围内的图像
        threshold(Image1, Image2, 240, 255, THRESH_BINARY);       //阈值去除一些干扰
        findContours(Image2, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_NONE);    //找轮廓
        //drawContours(frame, contours, -1, Scalar(0,0,255),2);
        for(int i = 0; i<contours.size(); i++)
        {
            rect = boundingRect(Mat(contours[i]));
            rect_Group.push_back(rect);
        }
        for(int i = 0; i<contours.size(); i++)
        {
            float contours_area = contourArea(contours[i]);
            float x = rect_Group[i].width;
            float y = rect_Group[i].height;
            float rectangle_area = x*y;
            //根据最小相切矩形的面积和轮廓的面积差过滤掉一些不规则的干扰项,根据轮廓面积和相切矩形的长宽差过滤掉背景的小蓝点
            if(rectangle_area - contours_area > 60 || contours_area < 45 || abs(x-y)<4.5)   continue;
            
            rect_1 = boundingRect(Mat(contours[i]));
            rotate_Rect = minAreaRect(contours[i]);
            contours1.push_back(contours[i]);
            rect_Group1.push_back(rect_1);
            rotate_Rect_Group.push_back(rotate_Rect);
            rectangle(frame, rect_1, Scalar(0,255,0),2);
        }
        //根据相邻相切矩形之间的长宽比来选出灯条
        for(int i=0; i<contours1.size(); i++)
        {
            center1 = getRectCenter(rotate_Rect_Group[i]);
            circle(result, center1, 2, Scalar(255, 255, 255));
            for(int j=i+1; j<contours1.size(); j++)
            {
                center2 = getRectCenter(rotate_Rect_Group[j]);
                circle(result, center2, 2, Scalar(255, 255, 255));
                float center_to_center = sqrtf((center1.x-center2.x)*(center1.x-center2.x)+(center1.y-center2.y)*(center1.y-center2.y));
                float rate_wd_hg1 = center_to_center/rotate_Rect_Group[i].size.height;
                float rate_wd_hg2 = center_to_center/rotate_Rect_Group[j].size.height;
                if(center_to_center>rotate_Rect_Group[i].size.height && center_to_center>rotate_Rect_Group[j].size.height && abs(rate_wd_hg1-rate_wd_hg2)<0.01)
                {
                    rect_2 = boundingRect(Mat(contours1[j]));
                    rect_3 = boundingRect(Mat(contours1[i]));
                    rotate_Rect1 = minAreaRect(contours1[i]);
                    rotate_Rect2 = minAreaRect(contours1[j]);
                    contours2.push_back(contours1[i]);
                    contours3.push_back(contours1[j]);
                    rect_Group2.push_back(rect_2);
                    rect_Group3.push_back(rect_3);
                    rotate_Rect_Group1.push_back(rotate_Rect1);
                    rotate_Rect_Group2.push_back(rotate_Rect2);
                    rectangle(result, rect_2, Scalar(0,255,0),2);
                    rectangle(result, rect_3, Scalar(0,255,0),2);
                }
            }
        }
        //绘制出中心点和四个角点
        for(int i=0; i<contours2.size(); i++)
        {
            center3 = getRectCenter(rotate_Rect_Group1[i]);
            left_up = getRectLeft_up(rotate_Rect_Group1[i], center3);
            left_down = getRectLeft_down(rotate_Rect_Group1[i], center3);
            right_up = getRectRight_up(rotate_Rect_Group1[i], center3);
            right_down = getRectRight_down(rotate_Rect_Group1[i], center3);
            circle(result, center3, 2, Scalar(0,255,0));
            circle(result, left_up, 2, Scalar(0,255,0));
            circle(result, left_down, 2, Scalar(0,255,0));
            circle(result, right_up, 2, Scalar(0,255,0));
            circle(result, right_down, 2, Scalar(0,255,0));
        }
        for(int i=0; i<contours3.size(); i++)
        {
            center4 = getRectCenter(rotate_Rect_Group2[i]);
            left_up = getRectLeft_up(rotate_Rect_Group1[i], center4);
            left_down = getRectLeft_down(rotate_Rect_Group1[i], center4);
            right_up = getRectRight_up(rotate_Rect_Group1[i], center4);
            right_down = getRectRight_down(rotate_Rect_Group1[i], center4);
            circle(result, center4, 2, Scalar(0,255,0));
            circle(result, left_up, 2, Scalar(0,255,0));
            circle(result, left_down, 2, Scalar(0,255,0));
            circle(result, right_up, 2, Scalar(0,255,0));
            circle(result, right_down, 2, Scalar(0,255,0));
        }
        //imshow("Image2",Image2);
        //imshow("frame",frame);
        imshow("result",result);
        waitKey(30);
        rect_Group.clear();
        rect_Group1.clear();
        rect_Group2.clear();
        rect_Group3.clear();
        contours.clear();
        contours1.clear();
        contours2.clear();
        contours3.clear();
        rotate_Rect_Group.clear();
        rotate_Rect_Group1.clear();
        rotate_Rect_Group2.clear();
    }
    double time =  ((double)getTickCount() - start) / getTickFrequency();
    double timeperframe = time/frameCount;
    cout<<"所用总时间为："<<time<<"秒"<<endl;
    cout<<"每一帧所用时间为："<<timeperframe<<"秒"<<endl;
    return 0;
}
