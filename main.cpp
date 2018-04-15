#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>

#define tkType 6

using namespace cv;
using namespace std;

Rect2d bbox;
Mat frame;
bool drawing_box=false;
bool gotBB=false;


//bounding box mouse callback
void mouseHandler(int event, int x, int y, int flags, void *param){
  switch( event ){
  case CV_EVENT_MOUSEMOVE:
    if (drawing_box){
        bbox.width = x-bbox.x;
        bbox.height = y-bbox.y;
    }
    break;//bounding box的大小
  case CV_EVENT_LBUTTONDOWN:
    drawing_box = true;
    bbox = Rect( x, y, 0, 0 );
    break;//bounding box的开始位置
  case CV_EVENT_LBUTTONUP:
    drawing_box = false;
    if( bbox.width < 0 ){
        bbox.x += bbox.width;
        bbox.width *= -1;
    }
    if( bbox.height < 0 ){
        bbox.y += bbox.height;
        bbox.height *= -1;
    }
    gotBB = true;
    break;//bounding box获取成功检查
  }
}
// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()
 
int main(int argc, char **argv)
{
    // List of tracker types in OpenCV 3.2
    // NOTE : GOTURN implementation is buggy and does not work.
    string trackerTypes[7] = {"BOOSTING", "MIL", "KCF", "TLD","MEDIANFLOW", "GOTURN","MOSSE"};
    // vector <string> trackerTypes(types, std::end(types));
 
    // Create a tracker
    string trackerType = trackerTypes[tkType];
 
    Ptr<Tracker> tracker;
 
    #if (CV_MINOR_VERSION < 3)
    {
        tracker = Tracker::create(trackerType);
    }
    #else
    {
        if (trackerType == "BOOSTING")
            tracker = TrackerBoosting::create();
        if (trackerType == "MIL")
            tracker = TrackerMIL::create();
        if (trackerType == "KCF")
            tracker = TrackerKCF::create();
        if (trackerType == "TLD")
            tracker = TrackerTLD::create();
        if (trackerType == "MEDIANFLOW")
            tracker = TrackerMedianFlow::create();
        if (trackerType == "GOTURN")
            tracker = TrackerGOTURN::create();
	if (trackerType == "MOSSE")
	    tracker = TrackerMOSSE::create();
    }
    #endif
    
    // Read video
    VideoCapture video(0);
    double fps=video.get(CV_CAP_PROP_FPS); //获取视频帧率  
    double pauseTime=1000/fps; //两幅画面中间间隔  
    // Exit if video is not opened
    if(!video.isOpened())
    {
        cout << "Could not read video file" << endl;
        return 1;
         
    }
    
    namedWindow("Tracking");
    setMouseCallback("Tracking",mouseHandler);
    
    
    bool ok = video.read(frame);
    //在此循环进行框选目标
    while(video.read(frame))
    {
      //没有此处的判断会出现不能进入该循环执行的情况waitKey()
      if(!frame.data || waitKey(pauseTime) == 27){break;}
      rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
      imshow("Tracking", frame);
      if(gotBB){break;}//得到目标后跳出框选循环
    }
    cout<<"Tracking bounding box:"<<bbox.x<<endl;
    cout<<"Tracking bounding box:"<<bbox.y<<endl;
    
    setMouseCallback("Tracking",NULL);
    
    tracker->init(frame, bbox);
    
    while(video.read(frame))
    {     
        // Start timer
        
        
        double timer = (double)getTickCount();
         
        // Update the tracking result
        bool ok = tracker->update(frame, bbox);
         
        // Calculate Frames per second (FPS)
        float fps = getTickFrequency() / ((double)getTickCount() - timer);
         
        if (ok)
        {
            // Tracking success : Draw the tracked object
            rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
        }
        else
        {
            // Tracking failure detected.
            putText(frame, "Tracking failure detected", Point(100,80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
        }
         
        // Display tracker type on frame
        putText(frame, trackerType + " Tracker", Point(100,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50),2);
         
        // Display FPS on frame
        putText(frame, "FPS : " + SSTR(int(fps)), Point(100,50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);
 
        // Display frame.
        imshow("Tracking", frame);
         
        // Exit if ESC pressed.
        int k = waitKey(1);
        if(k == 27)
        {
            break;
        }
 
    }
}
