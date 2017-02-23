#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

using namespace std;
using namespace cv;

//TODO: rotate face to fit

void pasteFace(Mat input_image, Mat replace_face, int face_x, int face_y, int face_height, int face_width);
bool closeToWhite(int x);

String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
String replace_face_name = "the_face.jpg";
String window_title = "Window";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

int main(int argc, char ** argv)
{
    Mat input_image;
    Mat replace_face;
    srand(time(NULL));

    if(argc < 2)
    {
        printf("No argument provided\n");
        return -1;
    }

    if(!face_cascade.load(face_cascade_name))
    {
        printf("Error loading face cascade\n");
        return -1;
    }

    if(!eyes_cascade.load(eyes_cascade_name))
    {
        printf("Error loading eyes cascade\n");
        return -1;
    }

    input_image = imread(argv[1]);
    if(!input_image.data)
    {
        printf("Error loading input image\n");
        return -1;
    }
    String in_name(argv[1]);
    String output_name = "outputs/" + in_name.substr(0, in_name.find(".", 0));
    output_name += ".ott.jpg";

    replace_face = imread(replace_face_name);
    if(!replace_face.data)
    {
        printf("Error loading replacement face\n");
        return -1;
    }

    std::vector<Rect> faces;
    Mat input_gray;
    cvtColor(input_image, input_gray, COLOR_BGR2GRAY);
    equalizeHist(input_gray, input_gray);

    face_cascade.detectMultiScale(input_gray, faces, 1.1, 2, 0|CASCADE_SCALE_IMAGE, Size(30, 30));

    for (size_t i = 0; i < faces.size(); i++)
    {
        Point center( faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2 );
        // ellipse(input_image, center, Size(faces[i].width/2, faces[i].height/2), 0, 0, 360, Scalar(255, 0, 255 ), 4, 8, 0);

        Mat faceROI = input_gray(faces[i]);
        std::vector<Rect> eyes;

        //Find the eyes
        eyes_cascade.detectMultiScale(faceROI, eyes, 1.1, 2, 0 |CASCADE_SCALE_IMAGE, Size(30, 30));

        //find midpoint of face to paste new face on
        circle(input_image, center, 5, Scalar(255, 0, 0), 4, 8, 0);
        pasteFace(input_image, replace_face, center.x, center.y, faces[i].width, faces[i].height);
    }

    imwrite(output_name, input_image);

    return 0;
}

//puts our target face at position (face_x, face_y), with height and width specified
void pasteFace(Mat input_image, Mat replace_face, int face_x, int face_y, int face_height, int face_width)
{
    face_width *= 2.5; //woo magic numbers
    face_height *= 2.5;

    resize(replace_face, replace_face, Size(face_height, face_width));
    int place_x = face_x - (face_height/2);
    int place_y = face_y - (face_width/2);

    // cerr << "Face location: " << face_x << ", " << face_y << endl;
    // cerr << "Face size: " << face_height << ", " << face_width << endl;
    // cerr << "Final location: " << place_x << ", " << place_y << endl;

    for(int i = 0; i < face_width; i++)
    {
        for(int j = 0; j < face_height; j++)
        {
            Vec3b color = replace_face.at<Vec3b>(Point(i,j));
            // if(color.val[0]!=255 && color.val[1]!=255 && color.val[2]!=255)
            if(notCloseToWhite(color.val[0])&&notCloseToWhite(color.val[1])&&notCloseToWhite(color.val[2]))
            {
                input_image.at<Vec3b>(Point(i+face_x-face_width/2, j+face_y-face_height/2)) = color;
            }
        }
    }
}

//check if color is close to white, within a threshold
bool notCloseToWhite(int x)
{
    int thresh = 4;
    if(abs(x - 255) >= thresh)
    {
        return true;
    }
    return false;
}
