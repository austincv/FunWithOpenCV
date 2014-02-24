#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

// Written for the competition on 8th Nov

Mat src_shrink; Mat src_gray; Mat src_origin; Mat src_background;
int thresh = 100;
int max_thresh = 255;
int blur_thresh = 7;
int max_blur_thresh = 15;
RNG rng(12345);

int from_picture = 1;
int back_loaded = 0;

	//define color limits
	int crange=25;
	int red =237;
	int green =27;
	int blue =36;

	int rH=red+crange;
	int rL=red-crange;
	int gH=green + crange;
	int gL=green - crange;
	int bH=blue + crange;
	int bL=blue - crange;
	

Mat r(800,600,CV_8UC1);
Mat g(800,600,CV_8UC1);
Mat b(800,600,CV_8UC1);

Mat rr(800,600,CV_8UC1);
Mat gg(800,600,CV_8UC1);
Mat bb(800,600,CV_8UC1);

Mat backR(800,600,CV_8UC1);
Mat backG(800,600,CV_8UC1);
Mat backB(800,600,CV_8UC1);

Mat output(800,600,CV_8UC1);




/// Function header
void draw_contour(int, void* );
static void onMouse(int, int, int, int, void*);

/** @function main */
int main( int argc, char** argv )
{

 CvCapture* capture;

   //Read the video stream
   capture = cvCaptureFromCAM( -1 );
   if( capture )
   {
     while( true )
     {
   src_origin = cvQueryFrame( capture );
  /// Load source image and convert it to gray
   
   //if loading from picture
   if (from_picture)
   src_origin = imread("squares.jpg",CV_LOAD_IMAGE_COLOR);

  /// Shrink
  resize(src_origin, src_shrink, Size(800,600), 0, 0,CV_INTER_AREA);

  /// Convert image to gray and blur it
  cvtColor( src_shrink, src_gray, CV_BGR2GRAY );
  blur( src_gray, src_gray, Size(3,3) );

  /// Create Window
  char* source_window = "Source";
  namedWindow( source_window, CV_WINDOW_AUTOSIZE );
  imshow( source_window, src_shrink );
  //thresh = 30;
  createTrackbar( " Blur thresh:", "Source", &blur_thresh, max_blur_thresh , draw_contour );
  createTrackbar( " color range :", "Source", &crange, max_thresh, draw_contour );
  //createTrackbar( " R :", "Source", &red, max_thresh, draw_contour );
  //createTrackbar( " G :", "Source", &green, max_thresh, draw_contour );
  //createTrackbar( " B :", "Source", &blue, max_thresh, draw_contour );
  setMouseCallback("Source",onMouse, 0);

   // Apply the classifier to the frame
       if( !src_origin.empty() )
       {   draw_contour( 0, 0 ); }
       else
       { printf(" --(!) No captured frame -- Break!"); break; }

       int c = waitKey(10);
       if( (char)c == 'c' ) { break; }
       if( (char)c == 'm' ) { from_picture = 0; }
       if( (char)c == 'p' ) { from_picture = 1; }
       if( (char)c == 'i' ) 
	{ src_background = src_shrink.clone();
	  back_loaded = 1;}
      }
   };

  return(0);
}
//function for mouse 

static void onMouse(int event, int x, int y, int, void*)
{
  if(event != EVENT_LBUTTONDOWN)
    return;

 
   //if loading from picture
   if (from_picture)
   src_origin = imread("squares.jpg",CV_LOAD_IMAGE_COLOR);

  /// Shrink
  resize(src_origin, src_shrink, Size(800,600), 0, 0,CV_INTER_AREA);

  	vector<Mat> channels(3); 
	split(src_shrink,channels); //use this for color comparison

	r = channels[2].clone();
	g = channels[1].clone();
	b = channels[0].clone();

  red = (int)r.at<unsigned char>(y,x);
  green = (int)g.at<unsigned char>(y,x);
  blue = (int)b.at<unsigned char>(y,x);
}


/** @function thresh_callback */
void draw_contour(int, void* )
{
  

  // Find colors
	vector<Mat> channels(3);
        vector<Mat> colorBGR(3);
	vector<Mat> backBGR(3);

	Mat src_blur = src_shrink.clone();
        
	medianBlur ( src_shrink, src_blur, blur_thresh*2+1 );	

	split(src_blur,channels); //use this for color comparison
        split(src_shrink,colorBGR); //use this for modifying and displaying in the end
	split(src_background,backBGR); //use this for background

	r = channels[2].clone();
	g = channels[1].clone();
	b = channels[0].clone();

	rr = colorBGR[2].clone();
	gg = colorBGR[1].clone();
	bb = colorBGR[0].clone();



	if (1) //classify colors
	{
	  for(int y =1;y<src_shrink.rows-1;y++)
	  {
		for(int x=0;x<src_shrink.cols;x++)
		{
		//define color
		int rH=red + crange;
		int rL=red - crange;
		int gH=green + crange;
		int gL=green - crange;
		int bH=blue + crange;
		int bL=blue - crange;

		if((int)r.at<unsigned char>(y,x) > rL && (int)r.at<unsigned char>(y,x) < rH &&
		   (int)g.at<unsigned char>(y,x) > gL && (int)g.at<unsigned char>(y,x) < gH &&
		   (int)b.at<unsigned char>(y,x) > bL && (int)b.at<unsigned char>(y,x) < bH && back_loaded == 1)
			{
			     channels[2].at<unsigned char>(y,x)=backBGR[2].at<unsigned char>(y,x);
			     channels[1].at<unsigned char>(y,x)=backBGR[1].at<unsigned char>(y,x);
			     channels[0].at<unsigned char>(y,x)=backBGR[0].at<unsigned char>(y,x);		
			}
		else
			{
			     channels[2].at<unsigned char>(y,x)=colorBGR[2].at<unsigned char>(y,x);
			     channels[1].at<unsigned char>(y,x)=colorBGR[1].at<unsigned char>(y,x);
			     channels[0].at<unsigned char>(y,x)=colorBGR[0].at<unsigned char>(y,x);		
			}

                }//color ends

	  }
	}
 
 
  /// Draw all contours
  //Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
  Mat invisible = src_shrink.clone();

  namedWindow( "Invisible", CV_WINDOW_AUTOSIZE );
  imshow( "Invisible", invisible);


  // Show the colors
  	namedWindow("colors", CV_WINDOW_AUTOSIZE );
        merge(channels,src_shrink);
	imshow("colors",src_shrink);

  //namedWindow( "src_origin", CV_WINDOW_AUTOSIZE );
  //imshow( "src_origin", src_origin);
}
