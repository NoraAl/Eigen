#include "__eigen.hpp"

const String keys =
        "{help h usage ?    |   | print this message   }"
        "{load              |   | use the existing model in the data directory       }"
        "{loadFile          |   | use a given existing model}"
        "{train             |   | save training results into a file}"
        "{test              |22 | id of the image to be tested      }"
        "{output            |   | output folder for the results       }"
        
        ;


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
    
    // This is done, so that the training data (which we learn the
    // cv::BasicFaceRecognizer on) and the test data we test
    // the model with, do not overlap.
    Mat testSample = images[testNo];
    int testLabel = labels[testNo];
   
    images.erase(images.begin()+testNo);
    labels.erase(labels.begin()+testNo);

    // The following lines create an Eigenfaces model for
    // face recognition and train it with the images and
    // labels read from the given CSV file.
    // This here is a full PCA, if you just want to keep
    // 10 principal components (read Eigenfaces), then call
    // the factory method like this:
    //
    //      cv::createEigenFaceRecognizer(10);
    //
    // If you want to create a FaceRecognizer with a
    // confidence threshold (e.g. 123.0), call it with:
    //
    //      cv::createEigenFaceRecognizer(10, 123.0);
    //
    // If you want to use _all_ Eigenfaces and have a threshold,
    // then call the method like this:
    //
    //      cv::createEigenFaceRecognizer(0, 123.0);
    //
    
    InputArray src = images;
    cout<< "elements: " << src.total()<< endl;
    cout<<"vector length: "<<src.getMat(0).total()<<endl;
    
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
    
    /* predict 2*/
    int predictedLabel =  model->predict(testSample);
    string result_message = format("Predicted class = %d / Actual class = %d.", predictedLabel, testLabel);
    
    // /* predict 3*/
    // Ptr<StandardCollector> collector = StandardCollector::create(model->getThreshold());
    // model->predict(testSample, collector);
    // collector->getResults();

    cout << result_message << endl;
    // Here is how to get the eigenvalues of this Eigenfaces model:
    Mat eigenvalues = model->getEigenValues();
    // And we can do the same to display the Eigenvectors (read Eigenfaces):
    Mat W = model->getEigenVectors();
    // Get the sample mean from the training data
    Mat mean = model->getMean();
    return 0;
    // Display or save:
    if(outputFolder.empty()) {
        imshow("mean", norm_0_255(mean.reshape(1, images[0].rows)));
    } else {
        imwrite(format("%s/mean.png", outputFolder.c_str()), norm_0_255(mean.reshape(1, images[0].rows)));
    }
    // Display or save the Eigenfaces:
    for (int i = 0; i < min(10, W.cols); i++) {
        string msg = format("Eigenvalue #%d = %.5f", i, eigenvalues.at<double>(i));
        cout << msg << endl;
        // get eigenvector #i
        
        Mat ev = W.col(i).clone();
        cout << "rows: "<<W.rows<<"\t and cols: "<<W.cols<<endl;
        //cout << ev<<endl;
        
        // Reshape to original size & normalize to [0...255] for imshow.
        Mat grayscale = norm_0_255(ev.reshape(1, height));
        // Show the image & apply a Jet colormap for better sensing.
        Mat cgrayscale;
        applyColorMap(grayscale, cgrayscale, COLORMAP_JET);
        // Display or save:
        if(argc == 2) {
            imshow(format("eigenface_%d", i), cgrayscale);
        } else {
            imwrite(format("%s/eigenface_%d.png", outputFolder.c_str(), i), norm_0_255(cgrayscale));
        }
    }
    // Display or save the image reconstruction at some predefined steps:
    // for(int num_components = min(W.cols, 10); num_components < min(W.cols, 300); num_components+=15) {
    //     // slice the eigenvectors from the model
    //     Mat evs = Mat(W, Range::all(), Range(0, num_components));
    //     Mat projection = LDA::subspaceProject(evs, mean, images[0].reshape(1,1));
    //     Mat reconstruction = LDA::subspaceReconstruct(evs, mean, projection);
    //     // Normalize the result:
    //     reconstruction = norm_0_255(reconstruction.reshape(1, images[0].rows));
    //     // Display or save:
    //     if(argc == 2) {
    //         imshow(format("eigenface_reconstruction_%d", num_components), reconstruction);
    //     } else {
    //         imwrite(format("%s/eigenface_reconstruction_%d.png", output_folder.c_str(), num_components), reconstruction);
    //     }
    // }
    // Display if we are not writing to an output folder:
    if(argc == 2) {
        waitKey(0);
    }
    return 0;
}
