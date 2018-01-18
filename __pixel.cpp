
#include "_slicAlgorithm.hpp"

using namespace cv;
using namespace cv::slicNora;
// using namespace std;

//vector <Info> files;
vector <Meta> metadata;
static void processVideo(int,int,int);
static int  superpixelFrame(Mat, vector<vector<float> >&, int , int, int);
static void saveSegmentaion( string filename, string videofile, float average[], int label, int starts, int ends, int); 
static void saveAverage(Meta meta, int range, int label);
static int filesExist;

static void readCsv(const string& filename, char separator = ',') ;
static const char *window_name = "Slic Superpixels";
static const char *keys =
    "{h help      | | help menu}"
    "{f file     | | file name}"
    "{a algorithm |1| Slic(0),Slico(1),MSlic(2)}";


int main(int argc, char **argv)
{
    CommandLineParser cmd(argc, argv, keys);
    if (cmd.has("help"))
    {
        cmd.about("This program demonstrates Slic superpixels using OpenCV class SuperpixelSlic.\n"
                  "If no image file is supplied, try to open a webcam.\n"
                  "Use [space] to toggle output mode, ['q' or 'Q' or 'esc'] to exit.\n");
        cmd.printMessage();
        return 0;
    }
    string imagesFile = cmd.get<String>("file");

    bool nofileSpecified = imagesFile.empty();
    if (nofileSpecified){
        imagesFile = "../meta.csv";
    }

    try
    {
        // array of files names to be processed
        readCsv(imagesFile);
    }

    catch (cv::Exception &e)
    {
        cerr << "Error opening file \"" << imagesFile << "\". Reason: " << e.msg << endl;
        // nothing more we can do
        exit(1);
    }

    int total = 0, range = 0;//0,1,2,3
    string scene("_d1");
    
    for (int i = 0 ; i < metadata.size(); i++){
        filesExist = i;

    

        // if no string equals to whatever in scene string, pass
        if (metadata[i].filename.find(scene) == string::npos)
            continue;

        int label = metadata[i].label;

        /* calc total number of frames being processed */
        //int frameCount = cap.get(CV_CAP_PROP_FRAME_COUNT);
        // cout << metadata[i].filename<<" has "<< frameCount << endl;
        //total += frameCount;

        if (metadata[i].range[range].exist)
            saveAverage(metadata[i], range,label);
        
    }
    cout << "There are " << total <<"frames"<<endl;
    

    //processVideo();

    return 0;
}


static void readCsv(const string& filename, char separator ) {
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(Error::StsBadArg, error_message);
    }
    // Info element;
    Meta element;
    int m = 0;
    string line, filefullname, classlabel, rangeStart[4], rangeEnd[4];
    while (getline(file, line)) {
      
        stringstream currentLine(line);
        getline(currentLine, classlabel, separator);
        getline(currentLine, filefullname, separator);
        getline(currentLine, rangeStart[0], separator);
        getline(currentLine, rangeEnd[0], separator);

        getline(currentLine, rangeStart[1], separator);
        getline(currentLine, rangeEnd[1], separator);

        getline(currentLine, rangeStart[2], separator);
        getline(currentLine, rangeEnd[2], separator);

        getline(currentLine, rangeStart[3], separator);
        getline(currentLine, rangeEnd[3]);

        if(!filefullname.empty() && !classlabel.empty()) {
            element.filename = "../video/"+filefullname+".avi";
            element.label = atoi(classlabel.c_str());
            
            element.range[0].exist = (atoi(rangeStart[0].c_str())==9999)?false:true;
            if (element.range[0].exist){
                element.range[0].start = atoi(rangeStart[0].c_str())-1;
                element.range[0].end = atoi(rangeEnd[0].c_str())-1;
            }

            element.range[1].exist = (atoi(rangeStart[1].c_str())==9999)?false:true;
            if (element.range[1].exist){
                element.range[1].start = atoi(rangeStart[1].c_str())-1;
                element.range[1].end = atoi(rangeEnd[1].c_str())-1;
            }

            element.range[2].exist = (atoi(rangeStart[2].c_str())==9999)?false:true;
            if (element.range[2].exist){

                element.range[2].start = atoi(rangeStart[2].c_str())-1;
                element.range[2].end = atoi(rangeEnd[2].c_str())-1;
            }

            element.range[3].exist = (atoi(rangeStart[3].c_str())==9999)?false:true;
            if (element.range[3].exist){
                element.range[3].start = atoi(rangeStart[3].c_str())-1;
                element.range[3].end = atoi(rangeEnd[3].c_str())-1;
            }

            metadata.push_back(element);
        }
    }

    cout << metadata.size()<<endl;
    for (int i = 0; i< 5; i++){
        cout << metadata[i].label<<"\t"<<metadata[i].range[0].start<<"->"<<metadata[i].range[0].end
        <<"\t\t"<<metadata[i].range[1].start<<"->"<<metadata[i].range[1].end
        <<"\t\t"<<metadata[i].range[2].start<<"->"<<metadata[i].range[2].end
        <<"\t\t"<<metadata[i].range[3].start<<"->"<<metadata[i].range[3].end
        <<"\t\t"<<metadata[i].filename<<endl;
    }
}

static void saveSegmentaion( string filename, string videofile, float average[], int label,int start, int end, int vectorLength){
    ofstream centroidsfile;
    string prefix = "../data/"+filename;
    cout << RED<<filesExist<<RESET<<endl;
    if (!filesExist){
        
        ofstream labelsfile(prefix+"labels.csv", ofstream::out );
        labelsfile << label << "," << videofile<<","<< start<<","<<end<< endl;
        labelsfile.close();

        ofstream metafile(prefix+"meta.csv", ofstream::out );
        metafile << vectorLength << "," << endl;
        metafile.close();

        centroidsfile.open(prefix+".csv", ofstream::out);
    } else {

        ofstream labelsfile(prefix+"labels.csv", ofstream::out | ofstream::app);
        labelsfile << label << "," << videofile<<","<< start<<","<<end<< endl;
        labelsfile.close();

        ofstream metafile(prefix+"meta.csv", ofstream::out | ofstream::app);
        metafile << vectorLength << "," << endl;
        metafile.close();

        centroidsfile.open(prefix+".csv", ofstream::out | ofstream::app);

    }

    for (int i = 0; i < vectorLength; i++)
    {
        // feature scaling by dividing by the max value, the min is zero
        centroidsfile << average[i] << ",";
    }
    centroidsfile << endl;
    centroidsfile.close();
}

void saveAverage(Meta meta, int range, int label)
{
    VideoCapture cap(meta.filename);
    cout << meta.filename<<endl;

    // vector to read each frame in the scene; to average them later on
    vector< Mat > frames;

    int position = meta.range[range].start;
    cap.set(CV_CAP_PROP_POS_FRAMES, position);

    // 'frame' holds the data of a single frame (pixels vs superpixels)
    Mat frame;
    cout << frame.channels()<<", "<<frame.depth()<<" ... "<<endl;

    // go over the range of frames and push pack frame info to the frames (pixels vs superpixels)
    while (position <= meta.range[range].end)
    {
        if (!cap.read(frame)) // if not successful, break loop, read() decodes and captures the next frame.
        {
            // cout << "\n Cannot read the video file. \n";
            break;
        }
        //cout << "reading a new sequence of frames..\n";
        if (frame.type() == CV_8UC3){
            //convert to grayscale
            cvtColor(frame, frame, COLOR_RGB2GRAY);
        } else if (frame.type() != CV_8UC1){
            cout << "error!"<< endl;
            exit(1);
        }
        frames.push_back(frame);
        imshow("movie", frame);
       
        waitKey(1);
    }
       
        

    // if no frames in current scene, do nothing
    if (frames.size() == 0){
        cout << "Empty Scene.."<< endl;
        return;
    }

    // average vector
    cout << "averaging .. "  << frames.size() << " " << frames[0].size() << endl;
    int numOfFrames = frames.size();

    Mat sum(frames[0].rows, frames[0].cols, CV_32F, Scalar(0));//overflow and floating point
    Mat diffs(frames[0].rows, frames[0].cols, CV_32F, Scalar(0));//overflow and floating point
    //average it
    Mat current(sum.size(), sum.type());
    Mat next(sum.size(), diffs.type());
    Mat result(sum.size(), diffs.type());
    for (int i = 0; i < frames.size(); i++)
    {
        
        frames[i].convertTo(current, sum.type());
        sum = sum + current;
        
        
        if (i < frames.size()-1){
            frames[i+1].convertTo(next, diffs.type());
            absdiff(next,current,result);
            diffs = diffs + result;
        }
        
    //waitKey(0);
    }
    

    // no need for averaging min max will do it, save 'view' image
    double min, max;
    minMaxLoc(sum, &min, &max);
    Mat view;
    if (min != max)
        sum.convertTo(view, CV_8U,255.0/(max-min),-255.0*min/(max-min));
    imshow("view",view);


    // Mat denom(frames[0].rows, frames[0].cols, CV_32F, Scalar(frames.size()));
    // Mat average(frames[0].rows, frames[0].cols, CV_32F, Scalar(0));
    // divide(sum,denom,average);

    minMaxLoc(diffs, &min, &max);
    Mat v;
    if (min != max)
        diffs.convertTo(v, CV_8U,255.0/(max-min),-255.0*min/(max-min));
    imshow("v",v);
   
    
    waitKey(0);
    
    // save label
    // save averaged vector, if i is zero create files
    // saveSegmentaion("cents", meta.filename, average, label, meta.range[3].start, meta.range[3].end, vectorLength);
    // saveSegmentaion("diff", meta.filename, diffAverage, label, meta.range[3].start, meta.range[3].end, vectorLength);
}
