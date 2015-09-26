#include <iostream>	// for standard I/O
#include <string>   // for strings

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat)
#include <opencv2/videoio/videoio.hpp>  // Video write

using namespace std;
using namespace cv;

static void help()
{
    cout
        << "------------------------------------------------------------------------------" << endl
        << "This program shows FOURCC codec of the input video"                             << endl
        << "Usage:"                                                                         << endl
        << "./video-write inputvideoName"                                                   << endl
        << "------------------------------------------------------------------------------" << endl
        << endl;
}

int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        cout << "Input parameter error" << endl;
        help();
        return -1;
    }

    const string source      = argv[1];           // the source file name
    const bool askOutputType = argv[3][0] =='Y';  // If false it will use the inputs codec type

    VideoCapture inputVideo(source);              // Open input
    if (!inputVideo.isOpened())
    {
        cout  << "Could not open the input video: " << source << endl;
        return -1;
    }

    int ex = static_cast<int>(inputVideo.get(CAP_PROP_FOURCC));     // Get Codec Type- Int form

    // Transform from int to char via Bitwise operators
    char EXT[] = {(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0};
    cout << "The FOURCC codec is : "<< EXT << endl;
    return 0;
}
