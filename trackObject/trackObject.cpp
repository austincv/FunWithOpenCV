#include <stdio.h>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;

void readme();
int trackObj(Mat &, Mat &);
static void onMouse(int event, int x, int y, int, void*);

/* Global variables */

Mat frame;
int ROI_width = 200;
int ROI_height = 200;
Rect ROI;
int frameWidth;
int frameHeight;
bool snapObjectFrame = true;

/** @function main */
int main( int argc, char** argv )
{
   VideoCapture capture(0); // open the default camera

   Mat img_object;
   Mat img_scene;


   if( capture.isOpened())
   {

     //find the video size
     frameWidth = (int) capture.get(CV_CAP_PROP_FRAME_WIDTH);
     frameHeight = (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT);
     //initialze ROI to the middle of the frame
     ROI = Rect(frameWidth/2 - ROI_width/2, frameHeight/2 - ROI_height/2 ,ROI_width,ROI_height);

     //the infinite loop where we do all the tracking
     while( true )
     {

          capture >> frame; //get a new frame from camera
	  
          cvtColor( frame, img_scene, CV_BGR2GRAY );
          
          if(snapObjectFrame)
	 {
		img_object = img_scene(ROI).clone(); 
		snapObjectFrame = false;
	 }

	  trackObj(img_object,img_scene);
          imshow("webcam", frame);
	  //-- use mouse to select object to track
	  setMouseCallback("webcam", onMouse, NULL);

     if(waitKey(30) == 115) //wait for 's' key press for 30 ms. If 's' key is pressed, stop
	break;
      }
    }
    else
    {
	readme();
    }

  waitKey(0);
  return 0;
}



/** @function trackObj */
int trackObj(Mat &img_object, Mat &img_scene)
{


  if( !img_object.data || !img_scene.data )
  { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

  //-- Step 1: Detect the keypoints using SURF Detector
  int minHessian = 400;

  SurfFeatureDetector detector( minHessian );

  std::vector<KeyPoint> keypoints_object, keypoints_scene;

  detector.detect( img_object, keypoints_object );
  detector.detect( img_scene, keypoints_scene );

  //-- Step 2: Calculate descriptors (feature vectors)
  SurfDescriptorExtractor extractor;

  Mat descriptors_object, descriptors_scene;

  extractor.compute( img_object, keypoints_object, descriptors_object );
  extractor.compute( img_scene, keypoints_scene, descriptors_scene );

  //-- Step 3: Matching descriptor vectors using FLANN matcher
  FlannBasedMatcher matcher;
  std::vector< DMatch > matches;
  matcher.match( descriptors_object, descriptors_scene, matches );

  double max_dist = 0; double min_dist = 100;

  //-- Quick calculation of max and min distances between keypoints
  for( int i = 0; i < descriptors_object.rows; i++ )
  { double dist = matches[i].distance;
    if( dist < min_dist ) min_dist = dist;
    if( dist > max_dist ) max_dist = dist;
  }

  printf("-- Max dist : %f \n", max_dist );
  printf("-- Min dist : %f \n", min_dist );

  //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
  std::vector< DMatch > good_matches;

  for( int i = 0; i < descriptors_object.rows; i++ )
  { if( matches[i].distance <= max(2*min_dist, 0.02) )
     { good_matches.push_back( matches[i]); }
  }

  Mat img_matches;
  drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
               good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
               vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

  //-- Localize the object
  std::vector<Point2f> obj;
  std::vector<Point2f> scene;

  for( int i = 0; i < good_matches.size(); i++ )
  {
    //-- Get the keypoints from the good matches
    obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
    scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
  }

 if(good_matches.size()>=4) //-- find homography only if we have 4 or more points
 {
	  Mat H = findHomography( obj, scene, CV_RANSAC );

	  //-- Get the corners from the image_1 ( the object to be "detected" )
	  std::vector<Point2f> obj_corners(4);
	  obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
	  obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );
	  std::vector<Point2f> scene_corners(4);

	  perspectiveTransform( obj_corners, scene_corners, H);

	  //-- Draw lines between the corners (the mapped object in the scene - image_2 )
	  line( img_matches, scene_corners[0] + Point2f( img_object.cols, 0), scene_corners[1] + Point2f( img_object.cols, 0), Scalar(0, 255, 0), 4 );
	  line( img_matches, scene_corners[1] + Point2f( img_object.cols, 0), scene_corners[2] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
	  line( img_matches, scene_corners[2] + Point2f( img_object.cols, 0), scene_corners[3] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
	  line( img_matches, scene_corners[3] + Point2f( img_object.cols, 0), scene_corners[0] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
 }

  //-- Show detected matches
  imshow( "Object Tracking", img_matches );

}
  /** @function readme */
  void readme()
  { std::cout << " Connect WebCam and make sure it is working | Usage: ./trackObject " << std::endl; }

  /** @function mouse click */

//function for mouse 

static void onMouse(int event, int x, int y, int, void*)
{
 if( event != EVENT_LBUTTONDOWN ) { return; }

   ROI = Rect(x - ROI_width/2, y - ROI_height/2 ,ROI_width,ROI_height);
   snapObjectFrame = true;

 return;
}

