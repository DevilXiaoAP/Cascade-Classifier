

// ConsoleApplication1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include<iostream>
#include<opencv2\core\core.hpp>
#include<opencv2\opencv.hpp>
#include<opencv2\highgui.hpp>
#include<opencv2/objdetect/objdetect.hpp>
#include<cv.h>
#include<stdio.h>
#include<ctime>
using namespace std;
using namespace cv;

String car_config_name = "cascade550_1000_25.xml"; //���÷�����ģ��·��
CascadeClassifier car_cascade;
vector<Rect> cars;
VideoCapture capture;
//vector<vector<Point>> assemble1(4);
//vector<vector<Point>> assemble2(4);
Point lane1_point1 = Point(334, 0);//�����һ��������
Point lane1_point2 = Point(330, 720);
Point lane2_point1 = Point(334, 0);//����ڶ���������
Point lane2_point2 = Point(330, 720);
CvPoint  CrossPoint(const CvPoint line1, const CvPoint line2, const CvPoint line3, const CvPoint line4);
double cal_area(int xup_length, int xbottom_length, int y_height);

int FLAG = 0;//switch video or image
static double ratio;

int main()
{
	//static int point_x;
	//double tmpx = 103 * point_x - 34505;//���峵����λ��
	if (FLAG == 0)
	{
		capture.open("4.mp4");
		Mat img;
		Mat img_gray;

		if (capture.isOpened())  //�ж���Ƶ�Ƿ�ɹ���
		{
			clock_t startTime, endTime;

			Size sWH = Size((int)capture.get(CV_CAP_PROP_FRAME_WIDTH), (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT));// get the frame size
			VideoWriter writer; // ������Ƶ
			writer.open("test_out.avi", CV_FOURCC('M', 'P', '4', '2'), 25.0, sWH, true); // ��ʼ����Ƶ����;

			bool a = car_cascade.load(car_config_name);
			while (capture.grab()) //capture.grab() ����Ƶ�ļ��򲶻��豸��ץȡ��һ��֡��������óɹ�����true��
			{

				startTime = clock();//��ʱ��ʼ
				capture >> img;
				//imshow("original", img);
				if (img.empty())
				{
					printf(" --(!) No captured frame -- Break!"); break;
				}
				else
				{
					cout << "video opened" << endl;
				}
				//imshow("img", img);
				cvtColor(img, img_gray, CV_BGR2GRAY);//ת���ɻҶ�ͼ���ӿ쳵��ʶ����ٶ�
				equalizeHist(img_gray, img_gray);   //ֱ��ͼ���⻯
													//imshow("gray", img_gray);
													//waitKey(0);
				car_cascade.detectMultiScale(img_gray, cars, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
				////face_cascade.detectMultiScale(img_gray, faces, 1.1, 3, CV_HAAR_SCALE_IMAGE, Size(64, 64));//����������⣬����ĵط�
				cout << "2" << endl;
				for (int i = 0; i < cars.size(); ++i)
				{
					//��������
					Point p1 = Point(cars[i].x, cars[i].y);//���Ͻ�
					Point p4 = Point(cars[i].x + cars[i].width, cars[i].y + cars[i].height);//���½�
					Point p2 = Point(cars[i].x + cars[i].width, cars[i].y);//���Ͻ�
					Point p3 = Point(cars[i].x, cars[i].y + cars[i].height);//���½�
					rectangle(img, p1, p4, Scalar(255, 0, 0), 2, 8, 0);
					//�ж��Ƿ���
					//Point center_point = Point(cars[i].x + 0.5*cars[i].width, cars[i].y+0.5*cars[i].height);//�����Ż�����ͷ��д
					//point_x = cars[i].x + 0.5*cars[i].width;

					Point cross_point1 = CrossPoint(lane1_point1, lane1_point2, p1, p2);//���ϱ߿򽻵�
					Point cross_point2 = CrossPoint(lane1_point1, lane1_point2, p3, p4);//solve bottom cross point
																						//cout << "cross point"<< cross_point1 << cross_point2 << endl;

					if (cross_point1.x == 0 || cross_point2.x == 0)//judge the cross point existance start
					{
						continue;
					}
					else
					{
						//vector<Point>assemble1{ {(p1)}, {(p3)}, {(cross_point1)}, {(cross_point2)} };//create a point assemble for the segmented region
						//vector<Point>assemble2{ p1, p3, cross_point1, cross_point2 };

						//approxPolyDP(assemble1, assemble1, 1000, true);
						//approxPolyDP(assemble2, assemble2, 1000, true);
						//cout << assemble1 << endl;

						double area1 = cal_area(abs(cross_point1.x - p1.x), abs(cross_point2.x - p3.x), p3.y - p1.y);
						double area2 = cal_area(abs(cross_point1.x - p2.x), abs(cross_point2.x - p4.x), p3.y - p1.y);
						//double area1 = contourArea(assemble1);//calculate the area of region1
						//double area2 = contourArea(assemble2);
						//cout << "p1" << p1 << "p2" << p2 << endl;
						//cout << "area" << area1 << endl;
						//cout << "area ratio" << area1 / area2 << endl;
						if (area1 > area2) //������α������߷ָ���С����������ı�ֵ
							ratio = area2 / area1;
						else
							ratio = area1 / area2;
						if (ratio > 0.3)//judge change labne or not and put text�������ֵ����0.3�����ڻ���
						{
							putText(img, "change lane", p1, FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2, 8);//��ͼƬ��д����
							cout << "change lane" << endl;
						}
						else
						{
							continue;
						}
					}//judge the cross point existance end

				}


				endTime = clock();//��ʱ����
				cout << "The run time is: " << (double)(endTime - startTime) / CLOCKS_PER_SEC << "s" << endl;
				imshow("result", img);
				writer << img;//��������ÿһ֡д����Ƶ
				//waitKey(1000);
				int c = waitKey(10);
				if ((char)c == 'c') { break; }

			}
			writer.release();
		}


		//return 0;
	}
	else
	{
		Mat img;
		Mat img_gray;
		img = imread("2.jpg");
		
		assert(!img.empty());
		cout << "image opened" << endl;
		//assert(face_cascade.load(face_config_name));
		bool a = car_cascade.load(car_config_name);
		//Mat img_gray;

		cvtColor(img, img_gray, CV_BGR2GRAY);//ת���ɻҶ�ͼ���ӿ쳵��ʶ����ٶ�
		equalizeHist(img_gray, img_gray);   //ֱ��ͼ���⻯
		imshow("gray", img_gray);
		//waitKey(0);
		car_cascade.detectMultiScale(img_gray, cars, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
		//face_cascade.detectMultiScale(img_gray, faces, 1.1, 3, CV_HAAR_SCALE_IMAGE, Size(64, 64));//����������⣬����ĵط�
		cout << "2" << endl;
		for (int i = 0; i < cars.size(); ++i)
		{
			Point p1 = Point(cars[i].x, cars[i].y);
			Point p4 = Point(cars[i].x + cars[i].width, cars[i].y + cars[i].height);

			rectangle(img, p1, p4, Scalar(255, 0, 0), 2, 8, 0);
		}
		imshow("View", img);
		waitKey(0);
		return 0;
	}
	return 0;
}

//�����߶ν���
CvPoint CrossPoint(const CvPoint line1, const CvPoint line2, const CvPoint line3, const CvPoint line4) //����
{
	double x_member, x_denominator, y_member, y_denominator;
	CvPoint cross_point;
	x_denominator = line4.x*line2.y - line4.x*line1.y - line3.x*line2.y + line3.x*line1.y
		- line2.x*line4.y + line2.x*line3.y + line1.x*line4.y - line1.x*line3.y;

	x_member = line3.y*line4.x*line2.x - line4.y*line3.x*line2.x - line3.y*line4.x*line1.x + line4.y*line3.x*line1.x
		- line1.y*line2.x*line4.x + line2.y*line1.x*line4.x + line1.y*line2.x*line3.x - line2.y*line1.x*line3.x;

	if (x_denominator == 0)
		cross_point.x = 0;
	else
		cross_point.x = x_member / x_denominator;
	//���ƺ�����x�ķ�Χ���߶��ڣ��������ֱ���ཻ���ж���
	if (cross_point.x < line3.x || cross_point.x > line4.x)
	{
		cross_point.x = 0;
	}
	//

	y_denominator = line4.y*line2.x - line4.y*line1.x - line3.y*line2.x + line1.x*line3.y
		- line2.y*line4.x + line2.y*line3.x + line1.y*line4.x - line1.y*line3.x;

	y_member = -line3.y*line4.x*line2.y + line4.y*line3.x*line2.y + line3.y*line4.x*line1.y - line4.y*line3.x*line1.y
		+ line1.y*line2.x*line4.y - line1.y*line2.x*line3.y - line2.y*line1.x*line4.y + line2.y*line1.x*line3.y;

	if (y_denominator == 0)
		cross_point.y = 0;
	else
		cross_point.y = y_member / y_denominator;

	return cross_point;  //ƽ�з���(0,0)
}


//�������
double cal_area(int xup_length, int xbottom_length, int y_height)
{
	double area = 0.5*(xup_length + xbottom_length)*y_height;
	return area;
}
