#include "__eigen.hpp"

static void fisher(vector<Mat>& , vector<int>& , Mat& , int );

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
    vector<Mat> images;
    vector<int> labels;

    readCsv( images, labels);

    // Quit if there are not enough images for this demo.
    if(images.size() <= 1) {
        string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
        CV_Error(Error::StsError, error_message);
    }
    // Get the height from the first image. We'll need this
    // later in code to reshape the images to their original
    // size:
    int height = images[0].rows;

    // make the image with test no is the test sample along with the label, and remove them from both sets
    Mat testSample = images[testNo];
    int testLabel = labels[testNo];
    images.erase(images.begin()+testNo);
    labels.erase(labels.begin()+testNo);

    
    Ptr<BasicFaceRecognizer> model = EigenFrameRecognizer::create(20);
    if (train){
        model->train(images, labels);
        model->write(modelFileName);
    } else {
        cout << "Reading the model from file .."<<endl;
        model->read(modelFileName);
    }
    cout << model->getThreshold()<<"<-----threshold"<<endl;
   
    /* predict 1*/
    // int predictedLabel = -1;
    // double confidence = 0.0;
    // model->predict(testSample, predictedLabel, confidence);
    // string result_message = format("Predicted class = %d / Actual class = %d, with confidence = %f.", predictedLabel, testLabel, confidence);
    // cout << result_message <<GREEN<<testLabel<< RESET <<endl;
    
    /* predict 2
    int predictedLabel =  model->predict(testSample);
    string result_message = format("Predicted class = %d / Actual class = ", predictedLabel);
    cout << result_message <<GREEN<<testLabel<< RESET <<endl;*/
    // /* predict 3*/
    
    Ptr<StandardCollector> collector = StandardCollector::create(model->getThreshold());
    model->predict(testSample, collector);


    vector< pair<int, double> > collectorResults = collector->getResults(true);
    cout <<"There are "<< collectorResults.size()<<" results."<<endl;
    for (int i = 0; i < collectorResults.size(); i++){
        if (!(i % 10))
            cout << endl;
        cout << MAGENTA << collectorResults[i].first << RESET << ", " << GREEN << collectorResults[i].second << RESET << "\t";
    }

    Mat eigenvalues = model->getEigenValues();
    Mat W = model->getEigenVectors();
    Mat mean = model->getMean();

    /* fisher faces */
    fisher(images,labels,testSample,testLabel);

    cout <<endl;
    return 0;
}

static void fisher(vector<Mat>& images, vector<int>& labels, Mat& testSample, int testLabel){
    cout << CYAN<<"--------------------fisher-------------------"<<RESET<<endl;
    Ptr<FaceRecognizer> fmodel = FisherFaceRecognizer::create();
    fmodel->train(images, labels);

    int predictedLabel = fmodel->predict(testSample);

    String result_message = format("Predicted class = %d / Actual class = ", predictedLabel);
    cout << result_message <<GREEN<<testLabel<< RESET <<endl;

    Ptr<StandardCollector> collector = StandardCollector::create(fmodel->getThreshold());
    fmodel->predict(testSample, collector);


    vector< pair<int, double> > collectorResults = collector->getResults(true);
     for (int i = 0; i < collectorResults.size(); i++){
        if (!(i % 10))
            cout << endl;
        cout << CYAN << collectorResults[i].first << RESET << ", "  << collectorResults[i].second << RESET << "\t";
    }
}
