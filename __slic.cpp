
#include "_slicAlgorithm.hpp"

using namespace cv;
using namespace cv::slicNora;
// using namespace std;

//vector <Info> files;
vector <Meta> metadata;
static void processVideo(int,int,int);
static int  superpixelFrame(Mat, vector<vector<float> >&, int , int, int);
static void saveSegmentaion( string filename, string videofile, float average[], int label, int starts, int ends, int, int); 
static int superpCount = 0;

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
    int regionSize = 8;
    //int ruler = 5;
    int min_element_size = 0;
    int iterationCount = 3;
    bool nofileSpecified = imagesFile.empty();
    if (nofileSpecified){
        imagesFile = "../meta.csv";
    }

    

    try
    {
        // array of files names
        readCsv(imagesFile);
    }
    catch (cv::Exception &e)
    {
        cerr << "Error opening file \"" << imagesFile << "\". Reason: " << e.msg << endl;
        // nothing more we can do
        exit(1);
    }
    int total = 0; 
    string scene("_d1");
    for (int i = 0 ; i < metadata.size(); i++){
        VideoCapture cap(metadata[i].filename);
        cout << metadata[i].filename<<endl;
        if (metadata[i].filename.find(scene) == string::npos)
            continue;
        int label = metadata[i].label;
        int frameCount = cap.get(CV_CAP_PROP_FRAME_COUNT);
        // cout << metadata[i].filename<<" has "<< frameCount << endl;
        total += frameCount;
        // first range..
        // if (metadata[i].range[0].exist){
        //     vector <vector<float> > frames;

        //     int position = metadata[i].range[0].start;
        //     cap.set(CV_CAP_PROP_POS_FRAMES,position );
        //     Mat frame;
        //     int count = 0;

        //     // go over the range of frames and get the average superpixeled scene
        //     while (position <= metadata[i].range[0].end){
                
        //         if (!cap.read(frame)) // if not success, break loop, read() decodes and captures the next frame.
        //         {
        //             cout << "\n Cannot read the video file. \n";
        //             break;
        //         } 
                
        //         count++;
        //         //cout << "reading: " << metadata[i].filename << "\t "<<metadata[i].label<<"\t" << position << endl;
        //         int currentspCount = superpixelFrame(frame, frames, algorithmy, iterationCount, regionSize);
                
        //         // check if all frame has the same number of superpixels.
        //         if ((currentspCount != superpCount)&& superpCount){
        //             cout << "error, number of superpixels is irregular."<<endl;
        //             exit(1);
        //         }
        //         superpCount = currentspCount;
        //         //imshow("video", frame);
        //         position++;
        //         // if (waitKey(30) == 27 ) // Wait for 'esc' key press to exit
        //         // {
        //         //     break;
        //         // }
        //     }

        //     if (count){//some scene, save it
        //     // average vector
        //     cout <<"averaging .. "<<superpCount<<" "<<frames.size()<<" "<<frames[0].size()<<endl;
        //     int vectorLength = frames[0].size();
        //     int numOfFrames = frames.size();
            
        //     // it is not a single superpixel vector, but all
        //     float average[vectorLength], diffAverage[vectorLength];
        //     // reset cells
        //     for (int i = 0; i < vectorLength; i++){
        //         average[i] = 0;
        //         diffAverage[i] = 0;
        //     }


        //     //average it
        //     for (int i = 0; i < frames.size(); i++){
        //         vector <float> diffVector;
        //         for (int j=0; j< frames[i].size(); j++){
        //             average[j] = average[j] + frames.at(i).at(j);
        //             if (i){
        //                 float diff = frames.at(i).at(j)- frames.at(i-1).at(j);
        //                 //diffVector.push_back(diff);
        //                 //cout << diff<<",";
        //                 diffAverage[j] = diffAverage[j] + diff;
        //             }
                    
                    
        //         }
        //         //cout <<endl<<endl;
        //         //differences.push_back(diffVector);
        //     }

        //     //calculate difference WROG!!!!!!!!!!!!!!!!!!!!
        //     // for (int i = 1; i < vectorLength; i++)
        //     //     difference[i] = average[i] - average[i-1];

        //     //standardize the average vector (To do: calc how to standardize difference vector)
        //     for (int i = 0; i < vectorLength ; i++){
        //         average[i]= average[i]/numOfFrames;
        //         diffAverage[i] = diffAverage[i]/(numOfFrames-1);

        //         if ((i%5)==0){
        //             average[i] = (average[i]*100) /  frame.rows;
        //             diffAverage[i] = (diffAverage[i]*1000) / frame.rows;
        //         }
        //         else if ((i%5)==1){
        //             average[i] = (average[i]*100) /  frame.cols;
        //             diffAverage[i] = (diffAverage[i]*1000) / frame.cols;
        //         }
        //         else{ 
        //             average[i] = (average[i]*100) /  256;//colors
        //             diffAverage[i] = (diffAverage[i]*1000) / 256;
        //         }
                    
        //     }
        //     // save label
        //     // save averaged vector, if i is zero create files
        //     saveSegmentaion("cents",average, label, vectorLength, i);
        //     saveSegmentaion("diff",diffAverage, label, vectorLength, i);

        //     }
        // }
        //last range..
        if (metadata[i].range[3].exist){
            vector <vector<float> > frames;

            int position = metadata[i].range[3].start;
            cap.set(CV_CAP_PROP_POS_FRAMES,position );
            Mat frame;
            int count = 0;

            // go over the range of frames and get the average superpixeled scene
            while (position <= metadata[i].range[3].end){
                
                if (!cap.read(frame)) // if not success, break loop, read() decodes and captures the next frame.
                {
                    cout << "\n Cannot read the video file. \n";
                    break;
                } 
                
                count++;
                //cout << "reading: " << metadata[i].filename << "\t "<<metadata[i].label<<"\t" << position << endl;
                int currentspCount = superpixelFrame(frame, frames, algorithmy, iterationCount, regionSize);
                
                // check if all frame has the same number of superpixels.
                if ((currentspCount != superpCount)&& superpCount){
                    cout << "error, number of superpixels is irregular."<<endl;
                    exit(1);
                }
                superpCount = currentspCount;
                //imshow("video", frame);
                position++;
                // if (waitKey(30) == 27 ) // Wait for 'esc' key press to exit
                // {
                //     break;
                // }
            }

            if (count){//some scene, save it
            // average vector
            cout <<"averaging .. "<<superpCount<<" "<<frames.size()<<" "<<frames[0].size()<<endl;
            int vectorLength = frames[0].size();
            int numOfFrames = frames.size();
            
            // it is not a single superpixel vector, but all
            float average[vectorLength], diffAverage[vectorLength];
            // reset cells
            for (int i = 0; i < vectorLength; i++){
                average[i] = 0;
                diffAverage[i] = 0;
            }


            //average it
            for (int i = 0; i < frames.size(); i++){
                vector <float> diffVector;
                for (int j=0; j< frames[i].size(); j++){
                    average[j] = average[j] + frames.at(i).at(j);
                    if (i){
                        float diff = frames.at(i).at(j)- frames.at(i-1).at(j);
                        //diffVector.push_back(diff);
                        //cout << diff<<",";
                        diffAverage[j] = diffAverage[j] + diff;
                    }
                    
                    
                }
                //cout <<endl<<endl;
                //differences.push_back(diffVector);
            }

            //calculate difference WROG!!!!!!!!!!!!!!!!!!!!
            // for (int i = 1; i < vectorLength; i++)
            //     difference[i] = average[i] - average[i-1];

            //standardize the average vector (To do: calc how to standardize difference vector)
            for (int i = 0; i < vectorLength ; i++){
                average[i]= average[i]/numOfFrames;
                diffAverage[i] = diffAverage[i]/(numOfFrames-1);

                if ((i%5)==0){
                    average[i] = (average[i]*100) /  frame.rows;
                    diffAverage[i] = (diffAverage[i]*1000) / frame.rows;
                }
                else if ((i%5)==1){
                    average[i] = (average[i]*100) /  frame.cols;
                    diffAverage[i] = (diffAverage[i]*1000) / frame.cols;
                }
                else{ 
                    average[i] = (average[i]*100) /  256;//colors
                    diffAverage[i] = (diffAverage[i]*1000) / 256;
                }
                    
            }
            // save label
            // save averaged vector, if i is zero create files
            saveSegmentaion("cents",metadata[i].filename, average, label, metadata[i].range[3].start, metadata[i].range[3].end, vectorLength, i);
            saveSegmentaion("diff",metadata[i].filename, diffAverage, label, metadata[i].range[3].start, metadata[i].range[3].end, vectorLength, i);

            }
        }

        
    }
    cout << "There are " << total <<"frames"<<endl;
    

    //processVideo();

    return 0;
}
//////////////////////////////////////////////////////////////////////////////
static int  superpixelFrame(Mat frame, vector<vector<float> >& frames, int algorithmy, int iterationCount, int regionSize){
    namedWindow(window_name, 0);
    Mat result;
    int display_mode = 2;
    int superpixelCount = 0;

    Mat converted, blurred;
    medianBlur(frame, blurred, 5);
    Ptr<SuperpixelSlic> slic =
        createSuperpixelSlic(blurred, algorithmy + Slic, regionSize);

    slic->iterate(iterationCount);

    //cout << "Slic" << (algorithmy ? 'O' : ' ')
         //<< " " << slic->getNumberOfSuperpixels() << " superpixels" << endl;

    Mat labels, uniforms;
    int count  =0 ;
    slic->getUniforms(uniforms);
    frames.push_back(slic->frameCentroids(count));
    // slic->getLabels(labels);
    //superpixelCount = slic->saveCentroids(label, superpixelCount);

    imshow("uniform", uniforms);
    //imwrite(resultName, uniforms );
    //imshow("blurred", blurred);

    int c = waitKey(1) & 0xff;
    return count ;
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


static void saveSegmentaion( string filename, string videofile, float average[], int label,int start, int end, int vectorLength, int filesExist){
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

