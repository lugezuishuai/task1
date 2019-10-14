//神符识别
// 运动物体检测——帧差法  
#include "opencv2/opencv.hpp"  
using namespace cv;
#include <iostream>  
using namespace std;
// 运动物体检测函数声明  
Mat MoveDetect(Mat temp, Mat frame);
vector<vector<Point>> contours;
 
int main()
{
	// 定义VideoCapture类video
	double start = static_cast<double>(getTickCount());
	VideoCapture video("../视频/2.mp4");
	if (!video.isOpened())  //对video进行异常检测  
	{
		cout << "video open error!" << endl;
		return 0;
	}
	// 获取帧数
	int frameCount = video.get(CV_CAP_PROP_FRAME_COUNT);
	// 获取FPS
	double FPS = video.get(CV_CAP_PROP_FPS);
	// 存储帧
	Mat frame;
	// 存储前一帧图像
	Mat temp;
	// 存储结果图像
	Mat result;
	for (int i = 0; i < frameCount; i++)
	{
		// 读帧进frame
		video >> frame;
		imshow("frame", frame);
		// 对帧进行异常检测
		if (frame.empty())
		{
			cout << "frame is empty!" << endl;
			break;
		}
		// 获取帧位置(第几帧)
		int framePosition = video.get(CV_CAP_PROP_POS_FRAMES); 
		cout << "framePosition: " << framePosition << endl;
		// 如果为第一帧（temp还为空）
		if (i == 0)
		{
			// 调用MoveDetect()进行运动物体检测，返回值存入result
			result = MoveDetect(frame, frame);
		}
		//若不是第一帧（temp有值了）
		else
		{
			// 调用MoveDetect()进行运动物体检测，返回值存入result
			result = MoveDetect(temp, frame);
		}
		imshow("result", result);
		// 按原FPS显示
		temp = frame.clone();
		if (waitKey(1000.0 / FPS) == 27)
		{
			cout << "ESC退出!" << endl;
			break;
		}
		contours.clear();
	}
	double time =  ((double)getTickCount() - start) / getTickFrequency();
    double timeperframe = time/frameCount;
    cout<<"所用总时间为："<<time<<"秒"<<endl;
    cout<<"每一帧所用时间为："<<timeperframe<<"秒"<<endl;
	return 0;
}
Mat MoveDetect(Mat temp, Mat frame)
{
	Mat result = frame.clone();
	// 1.将background和frame转为灰度图  
	Mat gray1, gray2;
	cvtColor(temp, gray1, CV_BGR2GRAY);
	cvtColor(frame, gray2, CV_BGR2GRAY);
	// 2.将background和frame做差  
	Mat diff;
	absdiff(gray1, gray2, diff);
	imshow("diff", diff);
	// 3.对差值图diff_thresh进行阈值化处理  
	Mat diff_thresh;
	threshold(diff, diff_thresh, 50, 255, CV_THRESH_BINARY);
	imshow("diff_thresh", diff_thresh);
	// 均值滤波
	Mat diff_thresh_blur;
	blur(diff_thresh, diff_thresh_blur,Size(10, 10));
	imshow("blur",diff_thresh_blur);
	// 4.腐蚀  
	Mat kernel_erode = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat kernel_dilate = getStructuringElement(MORPH_RECT, Size(18, 18));
	erode(diff_thresh_blur, diff_thresh_blur, kernel_erode);
	imshow("erode", diff_thresh_blur);
	// 5.膨胀  
	dilate(diff_thresh_blur, diff_thresh_blur, kernel_dilate);
	imshow("dilate", diff_thresh_blur);
	// 6.查找轮廓并绘制轮廓 
	findContours(diff_thresh_blur, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	// 在result上绘制轮廓
	//drawContours(result, contours, -1, Scalar(0, 0, 255), 2);
	// 7.查找正外接矩形  
	vector<Rect> boundRect(contours.size());
	int x0=0, y0=0, w0=0, h0=0;
	for (int i = 0; i < contours.size(); i++)
	{
		boundRect[i] = boundingRect(contours[i]);
		// 在result上绘制正外接矩形
		x0 = boundRect[i].x;  //获得第i个外接矩形的左上角的x坐标
        y0 = boundRect[i].y; //获得第i个外接矩形的左上角的y坐标
        w0 = boundRect[i].width; //获得第i个外接矩形的宽度
        h0 = boundRect[i].height; //获得第i个外接矩形的高度
        //筛选
        if(w0>50 && h0>100)
            rectangle(result, Point(x0, y0), Point(x0+w0, y0+h0), Scalar(0, 255, 0), 2, 8); //绘制第i个外接矩形
	}
	// 返回result
	return result;
}
