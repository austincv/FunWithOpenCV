#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <stdlib.h>
#include <iostream>

using namespace cv;
using namespace std;
int main(int argc, char* argv[])
{

    double outputFps; //the frame rate of the output required
    double inputFps;
    Size inputSize;
    Size outputSize;
    int codec;
    long unsigned int frameCount = 0;
    int reduceFrame = 1; //reduce the number of frames by this factor
    double shrinkFactor = 0.5;

    Mat src; //input frame
    Mat res; //output frame

    VideoCapture inputVideo(argv[1]); // open the video file for reading

        if ( !inputVideo.isOpened() )  // if not success, exit program
        {
             cout << "Cannot open the video file" << endl;
             return -1;
        }

    inputFps = inputVideo.get(CV_CAP_PROP_FPS); //get the frames per second of the video
    outputFps = inputFps/reduceFrame;
    codec = static_cast<int>(inputVideo.get(CV_CAP_PROP_FOURCC));  // Get Codec Type- Int form
    inputSize = Size((int) inputVideo.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                  (int) inputVideo.get(CV_CAP_PROP_FRAME_HEIGHT));
    outputSize = Size((int) inputSize.width * shrinkFactor,
                  (int) inputSize.height * shrinkFactor);

    //video output 
    VideoWriter outputVideo; //open output
    //outputVideo.open("ROI4.mp4", ex=-1, cap.get(CV_CAP_PROP_FPS), S, true);
    //outputVideo.open(argv[2], CV_FOURCC('D', 'I', 'V', 'X'), inputVideo.get(CV_CAP_PROP_FPS), Size(1300,600), true);
    outputVideo.open(argv[2], CV_FOURCC('D', 'I', 'V', 'X'), outputFps, outputSize, true);

        if (!outputVideo.isOpened())
        {
            cout  << "Could not open the output video for write: " << endl;
            return -1;
        }


    for(;;) //Show the image captured in the window and repeat
    {

        inputVideo >> src;              // read
        if (src.empty()) break;         // check if at end
        cout<<"*";
       //outputVideo.write(res); //save or
        if ( frameCount % reduceFrame == 0)
        { 
            //do the processing
            //To shrink an image, it will generally look best with CV_INTER_AREA interpolation, whereas to enlarge an image, it will generally look best with CV_INTER_CUBIC (slow) or CV_INTER_LINEAR (faster but still looks OK).
            resize(src, res, outputSize, 0, 0, CV_INTER_AREA);
            //output the frame
            outputVideo << res;
            cout<<endl; 
        }

        frameCount++;
    }

    waitKey(0);
    return 0;
}