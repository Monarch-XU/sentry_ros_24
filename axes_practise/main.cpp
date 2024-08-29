#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

//测距公式：l（识别到的矩形的高的像素长度） =  K/x（矩形到镜头的距离，单位cm）
//-> x = K/l
//K：4220.28506+-20.20322
#define K 4240.285

#define ks 0.0203125

void on_Trackbar(int i, void *pVoid){
}

int main() {
    VideoCapture v(0);
    if(v.isOpened()){cout << "open successfully!" << endl;}
    else{cout << "please check the video!" << endl;}

    namedWindow("Trackbars",1000);
    int hmin=194,smin=71,vmin=0,hmax=255,smax=255,vmax=123;

    createTrackbar("hmin","Trackbars", &hmin,255,on_Trackbar);
    createTrackbar("hmax","Trackbars", &hmax,255,on_Trackbar);
    createTrackbar("smin","Trackbars", &smin,255,on_Trackbar);
    createTrackbar("smax","Trackbars", &smax,255,on_Trackbar);
    createTrackbar("vmin","Trackbars", &vmin,255,on_Trackbar);
    createTrackbar("vmax","Trackbars", &vmax,255,on_Trackbar);

    int count = 0;
    float l_average = 0;
    float x_average = 0;
    float x_av = 0;
    float y_av = 0;
    while (true){
        Mat img;
        v >> img;
//        cout<<img.cols<<"   "<<img.rows<<endl;
        Mat mask;
        Scalar red_lower(hmin,smin,vmin);
        Scalar red_upper(hmax,smax,vmax);
        inRange(img,red_lower,red_upper,mask);
        Mat k = getStructuringElement(0,Size(13,13));
        morphologyEx(mask,mask,MORPH_CLOSE,k);
        vector<vector<Point>> contours;
        vector<Vec4i> hierarchy;
        findContours(mask,contours,hierarchy,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);

        Mat src_copy;
        img.copyTo(src_copy);

        if (!contours.empty()){
            for (int i = 0; i < contours.size(); ++i) {
                RotatedRect rr = minAreaRect(contours[i]);
                Point2f point[4];
                rr.points(point);
                double l1 = pow(point[0].x-point[1].x,2)+pow(point[0].y-point[1].y,2);
                l1 = pow(l1,0.5);
                double l2 = pow(point[1].x-point[2].x,2)+pow(point[1].y-point[2].y,2);
                l2 = pow(l2,0.5);

                bool length_width_ratio_ok = false;
                if ((l1/l2<=0.9&&l1/l2>=0.5)||(l2/l1<=0.9&&l2/l1>=0.5)) length_width_ratio_ok = true;
                bool size_ok = false;
                if (l1*l2>=3000&&l1*l2<=200000) size_ok = true;

                if (length_width_ratio_ok&&size_ok) {


                    line(src_copy, point[0], point[1], Scalar(255, 0, 0), 3);
                    line(src_copy, point[1], point[2], Scalar(255, 0, 0), 3);
                    line(src_copy, point[2], point[3], Scalar(255, 0, 0), 3);
                    line(src_copy, point[3], point[0], Scalar(255, 0, 0), 3);
                    string center_txt = "center:";
                    center_txt += to_string(rr.center.x) + "," + to_string(rr.center.y);
                    putText(src_copy,center_txt ,Point(rr.center.x,rr.center.y),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0),4);
                    putText(src_copy,center_txt ,Point(rr.center.x,rr.center.y),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,255,255),2);
                    string size_txt = "size:";
                    size_txt += to_string(l1*l2);
                    putText(src_copy,size_txt ,Point(rr.center.x,rr.center.y-20),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,0,0),4);
                    putText(src_copy,size_txt ,Point(rr.center.x,rr.center.y-20),FONT_HERSHEY_SIMPLEX,0.5,Scalar(0,255,255),2);
                    circle(src_copy,rr.center,2,Scalar(255,0,0),-1);

                    count++;
                    l_average += min(l1,l2);
                    float x = K/min(l1,l2);
                    x_av += rr.center.x * ks;
                    y_av += rr.center.y * ks;
                    x_average += x;

                    if (count%20==0){
                        l_average /= (float)count;
                        x_average /= (float)count;
                        x_av /= (float)count;
                        y_av /= (float)count;
                        count = 0;
//                        cout<<"size_average: "<<l_average<<endl;
//                        cout<<"x_average: "<<x_average<<endl;
                        printf("x=%.1f,y=%.1f,z=%.1f\n",x_av,y_av,x_average);
                        l_average = 0;
                        x_average = 0;
                    }



                }
            }




        }

        imshow("video",img);
        imshow("mask",mask);
        imshow("src_copy",src_copy);
        waitKey(1);

    }
    return 0;
}
