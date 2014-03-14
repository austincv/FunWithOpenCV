#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdlib.h>
#include <iostream>

using namespace cv;
using namespace std;
int main(int argc, char* argv[])
{

    double outputFps = 1; //the frame rate of the output required
    double inputFps;
    Size inputSize;
    int codec;

    Mat src; //input frame

    VideoCapture inputVideo(argv[1]); // open the video file for reading

        if ( !inputVideo.isOpened() )  // if not success, exit program
        {
             cout << "Cannot open the video file" << endl;
             return -1;
        }

    inputFps = inputVideo.get(CV_CAP_PROP_FPS); //get the frames per second of the video
    codec = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));  // Get Codec Type- Int form
    inputSize = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                  (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));

    //video output 
    VideoWriter outputVideo; //open output
    //outputVideo.open("ROI4.mp4", ex=-1, cap.get(CV_CAP_PROP_FPS), S, true);
    //outputVideo.open(argv[2], CV_FOURCC('D', 'I', 'V', 'X'), inputVideo.get(CV_CAP_PROP_FPS), Size(1300,600), true);
    outputVideo.open(argv[2], codec, outputFps, inputSize, true);

        if (!outputVideo.isOpened())
        {
            cout  << "Could not open the output video for write: " << endl;
            return -1;
        }

    for(;;) //Show the image captured in the window and repeat
    {
        inputVideo >> src;              // read
        if (src.empty()) break;         // check if at end

       //outputVideo.write(res); //save or
       outputVideo << src;
    }

    waitKey(0);
    return 0;
}