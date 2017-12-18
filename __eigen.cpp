#include "__eigen.hpp"

static void fisher(vector<Mat>& , vector<int>& , Mat& , int );
static void diffModel(vector<Mat>& , vector<int>& , Mat& , int );
static void mostFrequent( vector< pair<int, double> >&, int);



int main(int argc, const char *argv[]) {
    CommandLineParser parser(argc, argv, keys);
    parser.about("Application name v1.0.0");
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }
    // empty loadFile means trian and save model into default name 
    // existing train means train and save model into the given name 
    String modelFileName = parser.get<String>("loadFile");
    bool defaultFileName = modelFileName.empty();// no specific file given to load file from
    bool train = true;
    if (defaultFileName){
        modelFileName = parser.get<String>("train");
        defaultFileName = modelFileName.empty();//no specific file given to save model into
        if (defaultFileName){
            modelFileName = "../data/eigenfacesModel.yml";
            if ( parser.has("load"))
                train = false;
        }
    }
    String outputFolder = parser.get<string>("output");
    int testNo = parser.get<int>("test");

    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }
    
    // These vectors hold the images and corresponding labels.
    vector<Mat> images, diffImages;
    vector<int> labels;

    int maxSize = readTrainedCsv( images, diffImages, labels);

    // Quit if there are not enough images for this demo.
    if(images.size() <= 1) {
        string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
        CV_Error(Error::StsError, error_message);
    }
    int height = images[0].rows;

    // make the image with test no is the test sample along with the label, and remove them from both sets
    if (testNo > maxSize){
        string error_message = "\nPlease choose test image that is within range, reaching out of range.";
        CV_Error(Error::StsError, error_message);
    }
    Mat testSample = images[testNo];
    Mat difftestSample = diffImages[testNo];
    int testLabel = labels[testNo];
    
    images.erase(images.begin()+testNo);
    diffImages.erase(diffImages.begin()+testNo);
    labels.erase(labels.begin()+testNo);

    cout << "There are "<< BOLDCYAN << diffImages.size()<<RESET<<" training images."<<endl;

    cout << "64"<<endl;
    Ptr<BasicFaceRecognizer> model = EigenFrameRecognizer::create(20);
    if (train){
        cout << "67"<<endl;
        model->train(images, labels);
        cout << "68"<<endl;
        model->write(modelFileName);
    } else {
        cout << "Reading the model from file .."<<endl;
        model->read(modelFileName);
    }
    cout << "73"<<endl;
    Ptr<StandardCollector> collector = StandardCollector::create(model->getThreshold());
    model->predict(testSample, collector);


    vector< pair<int, double> > collectorResults = collector->getResults(true);
    mostFrequent(collectorResults, testLabel);

    Mat eigenvalues = model->getEigenValues();
    Mat W = model->getEigenVectors();
    Mat mean = model->getMean();
    
    /* fisher faces */
    diffModel(diffImages,labels,difftestSample,testLabel);
    //fisher(images,labels,testSample,testLabel);

    cout <<endl;
    return 0;
}

static void fisher(vector<Mat>& images, vector<int>& labels, Mat& testSample, int testLabel){
    cout << CYAN<<"\n--------------------fisher-------------------"<<RESET<<endl;
    
    Ptr<FaceRecognizer> fmodel = FisherFaceRecognizer::create();
    fmodel->train(images, labels);

    Ptr<StandardCollector> collector = StandardCollector::create(fmodel->getThreshold());
    fmodel->predict(testSample, collector);
    

    vector< pair<int, double> > collectorResults = collector->getResults(true);
    
    mostFrequent(collectorResults, testLabel);

    //  for (int i = 0; i < collectorResults.size(); i++){
    //     if (!(i % 10))
    //         cout << endl;
    //     cout << CYAN << collectorResults[i].first << RESET << ", "  << collectorResults[i].second << RESET << "\t";
    // }
}

static void diffModel(vector<Mat>& images, vector<int>& labels, Mat& testSample, int testLabel){
    cout << MAGENTA<<"\n--------------------Diff-------------------"<<RESET<<endl;
    
    Ptr<FaceRecognizer> fmodel = EigenFrameRecognizer::create();
    fmodel->train(images, labels);

    Ptr<StandardCollector> collector = StandardCollector::create(fmodel->getThreshold());
    fmodel->predict(testSample, collector);
    

    vector< pair<int, double> > collectorResults = collector->getResults(true);
    
    mostFrequent(collectorResults, testLabel);

    //  for (int i = 0; i < collectorResults.size(); i++){
    //     if (!(i % 10))
    //         cout << endl;
    //     cout << CYAN << collectorResults[i].first << RESET << ", "  << collectorResults[i].second << RESET << "\t";
    // }
}

static void mostFrequent( vector< pair<int, double> >& results, int label){
    cout << "Actual class: "<< BOLDGREEN << label << RESET<<" and results total size is :"<<BOLDBLUE
    <<results.size()<<RESET << endl;

    map <int, int> frequent;
    for (int i = 0; i < results.size(); i++){
        if (frequent.find(results[i].first) == frequent.end())
        {
            frequent[results[i].first] = 1;
        } else {
            frequent[results[i].first]++;
        }
    }
    cout << BOLDBLACK << "Top " << frequent.size()  << " results :"<< RESET<<endl;
    cout <<BOLDRED<< "Class" << ":\t"<<"Recall"<<RESET<<endl;
    for (map<int, int>::iterator it = frequent.begin(); it != frequent.end(); it++)
    {
        if(it->first == label)
            cout <<BOLDRED<< it->first << ":\t"<<it->second<<RESET<<endl;
        else 
            cout <<CYAN<< it->first << ":\t"<<it->second<<RESET<<endl;
        //Should output 1 4 8
    }
}
