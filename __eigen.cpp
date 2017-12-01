#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "_framerec.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace cv;
using namespace std;
static Mat norm_0_255(InputArray _src) {
    Mat src = _src.getMat();
    // Create and return normalized image:
    Mat dst;
    switch(src.channels()) {
        case 1:
            cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
            break;
        case 3:
            cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
            break;
        default:
            src.copyTo(dst);
            break;
    }
    return dst;
}

static void read_csv( vector<Mat>& images, vector<int>& labels, char separator = ',') {
    ifstream metafile("../data/__metadata.csv", ifstream::in);
    string value; int columns;
    if (getline(metafile, value, ',')){
        columns = atoi(value.c_str());
    }
    cout << columns<<endl;

    ifstream xfile("../data/_x.csv", ifstream::in);
    ifstream yfile("../data/_y.csv", ifstream::in);
    ifstream c1file("../data/_color1.csv", ifstream::in);
    ifstream c2file("../data/_color2.csv", ifstream::in);
    ifstream c3file("../data/_color3.csv", ifstream::in);
    ifstream labelsfile("../data/_labels.csv", ifstream::in);
    
    if (!xfile || !yfile || !c1file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(Error::StsBadArg, error_message);
    }

    string xline,yline,c1line,c2line,c3line;
    
    while (getline(xfile, xline) && getline(yfile, yline) && getline(c1file, c1line) && getline(c2file, c2line)&& getline(c3file, c3line))
    {
        // Now inside each line we need to seperate the cols

        Mat image (5,columns, CV_32F);//**
        int label;
        int j = 0;

        stringstream xstream(xline), ystream(yline), c1stream(c1line), c2stream(c2line), c3stream(c3line) ;
        
        string x,y,c1,c2,c3,l;
        getline(labelsfile, l);
        
            
        label = atoi(l.c_str());
       
        while (getline(xstream, x, ','))
        {
            
            if (!getline(ystream,y))
                ;//error
                
            if (!getline(c1stream,c1))
                ;//error
                //cout <<"c1"<<endl;
            if (!getline(c2stream,c2))
                ;//error
                //cout <<"c2"<<endl;
            if (!getline(c3stream,c3))
                ;//error
                //cout <<"c3--"<<j<<endl;
            
            
            
            image.at<float>(0,j) = atof(x.c_str());
            image.at<float>(1,j) = atof(y.c_str());
            image.at<float>(2,j) = atof(c1.c_str());
            image.at<float>(3,j) = atof(c2.c_str());
            image.at<float>(4,j) = atof(c3.c_str());

            j++;
        }
        // add the row to the complete data vector
        images.push_back(image);
        labels.push_back(label);
    
    }
    xfile.close(); yfile.close(); c1file.close(); c2file.close(); c3file.close(); labelsfile.close();

}


int main(int argc, const char *argv[]) {
    // Check for valid command line arguments, print usage
    // if no arguments were given.
    // if (argc < 2) {
    //     cout << "usage: " << argv[0] << " <csv.ext> <output_folder> " << endl;
    //     exit(1);
    // }
    string output_folder = ".";
    if (argc == 2) {
        output_folder = string(argv[2]);
        cout<<output_folder<<endl;
    }
    
    // These vectors hold the images and corresponding labels.
    vector<Mat> images;
    vector<int> labels;

    read_csv( images, labels);
   
    // cout << images[0]<<endl;
    // cout << images[1]<<endl;
    // cout << images[0].cols<<" "<<images[0].rows<<"  "<< images.size()<<endl;
     

    // Quit if there are not enough images for this demo.
    if(images.size() <= 1) {
        string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
        CV_Error(Error::StsError, error_message);
    }
    // Get the height from the first image. We'll need this
    // later in code to reshape the images to their original
    // size:
    int height = images[0].rows;
    // The following lines simply get the last images from
    // your dataset and remove it from the vector. This is
    // done, so that the training data (which we learn the
    // cv::BasicFaceRecognizer on) and the test data we test
    // the model with, do not overlap.
    
    
    // Mat testSample = images[images.size() - 1];
    // int testLabel = labels[labels.size() - 1];
    // images.pop_back();
    // labels.pop_back();
    int testNo = 337;
    Mat testSample = images[testNo];
    int testLabel = labels[testNo];
    // vector<int>::iterator it = labels.begin()+testNo;
    // cout << *it<<"  "<<testLabel<<endl;
    
    
    // return 0;
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
    
    Ptr<BasicFaceRecognizer> model = EigenFrameRecognizer::create();
    model->train(images, labels);
    
    // The following line predicts the label of a given
    // test image:
    int predictedLabel = model->predict(testSample);
    //
    // To get the confidence of a prediction call the model with:
    //
    //      int predictedLabel = -1;
    //      double confidence = 0.0;
    //      model->predict(testSample, predictedLabel, confidence);
    //
    string result_message = format("Predicted class = %d / Actual class = %d.", predictedLabel, testLabel);
    cout << result_message << endl;
    // Here is how to get the eigenvalues of this Eigenfaces model:
    Mat eigenvalues = model->getEigenValues();
    // And we can do the same to display the Eigenvectors (read Eigenfaces):
    Mat W = model->getEigenVectors();
    // Get the sample mean from the training data
    Mat mean = model->getMean();
    // Display or save:
    if(argc == 2) {
        imshow("mean", norm_0_255(mean.reshape(1, images[0].rows)));
    } else {
        imwrite(format("%s/mean.png", output_folder.c_str()), norm_0_255(mean.reshape(1, images[0].rows)));
    }
    // Display or save the Eigenfaces:
    for (int i = 0; i < min(10, W.cols); i++) {
        string msg = format("Eigenvalue #%d = %.5f", i, eigenvalues.at<double>(i));
        cout << msg << endl;
        // get eigenvector #i
        
        Mat ev = W.col(i).clone();
        cout << "rows: "<<W.rows<<"\t and cols: "<<W.cols<<endl;
        //cout << ev<<endl;
        return 0;
        // Reshape to original size & normalize to [0...255] for imshow.
        Mat grayscale = norm_0_255(ev.reshape(1, height));
        // Show the image & apply a Jet colormap for better sensing.
        Mat cgrayscale;
        applyColorMap(grayscale, cgrayscale, COLORMAP_JET);
        // Display or save:
        if(argc == 2) {
            imshow(format("eigenface_%d", i), cgrayscale);
        } else {
            imwrite(format("%s/eigenface_%d.png", output_folder.c_str(), i), norm_0_255(cgrayscale));
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
