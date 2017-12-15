
#include "_slicAlgorithm.hpp"

using namespace cv;
using namespace cv::slicNora;
// using namespace std;

//vector <Info> files;
vector <Meta> metadata;


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
    
    int algorithmy = cmd.get<int>("algorithm");
    int region_size = 5;
    //int ruler = 5;
    int min_element_size = 5;
    int num_iterations = 3;
    bool nofileSpecified = imagesFile.empty();
    if (nofileSpecified){
        imagesFile = "../meta.csv";
    }

    VideoCapture cap;
    Mat input_image;
    int label;

    try
    {
        // array of files names
        readCsv(imagesFile);
        return 0;
    }
    catch (cv::Exception &e)
    {
        cerr << "Error opening file \"" << imagesFile << "\". Reason: " << e.msg << endl;
        // nothing more we can do
        exit(1);
    }

    namedWindow(window_name, 0);
    Mat result;
    int display_mode = 2;
    int superpixelCount = 0;
    for (int i = 0 ; i < metadata.size(); i++)
    {
        Mat frame;
        string resultName = "../results/image" + to_string(i)+".jpg";

        label = metadata[i].label;
        input_image = imread(metadata[i].filename);
        input_image.copyTo(frame);
        
        Mat converted, blurred;
        medianBlur(frame, blurred , 5);
        Ptr<SuperpixelSlic> slic = 
        createSuperpixelSlic(blurred, algorithmy + Slic, region_size);
         
        slic->iterate(num_iterations);

        cout << "Slic" << (algorithmy ? 'O' : ' ')
             << " "<< slic->getNumberOfSuperpixels() << " superpixels" << endl;

        Mat labels, uniforms;

        slic->getUniforms(uniforms);
        slic->getLabels(labels);
        superpixelCount = slic->saveCentroids(label, superpixelCount);

        imshow("uniform", uniforms);
        imwrite(resultName, uniforms );
        //imshow("blurred", blurred);

        int c = waitKey(1) & 0xff;
        
        if (c == 'q' || c == 'Q' || c == 27 )
            break;
        else if (c == ' ')
            display_mode = (display_mode + 1) % 3;
          
    }

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
            element.filename = filefullname;
            element.label = atoi(classlabel.c_str());
            
            element.range[0].exist = (atoi(rangeStart[0].c_str())==9999)?false:true;
            if (element.range[0].exist){
                element.range[0].start = atoi(rangeStart[0].c_str());
                element.range[0].end = atoi(rangeEnd[0].c_str());
            }

            element.range[1].exist = (atoi(rangeStart[1].c_str())==9999)?false:true;
            if (element.range[1].exist){
                element.range[1].start = atoi(rangeStart[1].c_str());
                element.range[1].end = atoi(rangeEnd[1].c_str());
            }

            element.range[2].exist = (atoi(rangeStart[2].c_str())==9999)?false:true;
            if (element.range[2].exist){

                element.range[2].start = atoi(rangeStart[2].c_str());
                element.range[2].end = atoi(rangeEnd[2].c_str());
            }

            element.range[3].exist = (atoi(rangeStart[3].c_str())==9999)?false:true;
            if (element.range[3].exist){
                element.range[3].start = atoi(rangeStart[3].c_str());
                element.range[3].end = atoi(rangeEnd[3].c_str());
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

