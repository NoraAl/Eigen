
# SLIC PCA
This project use slic superpixels as basis for two principle component analysis algorithms: Eigenfaces and Fisherfaces.

## Dependencies
* OpenCV 3.3, [here](https://github.com/opencv/opencv). 
* Opencv_contrib, [here](https://github.com/opencv).

## Project Hierarchy
Below is the hierarchy for the repository. Files start with underscores are modified library's files, while those with double underscores are the two main project's files:
```
├── CMakeFiles.txt
├── __eigen.cpp
├── __eigen.hpp
├── __eigenHelpers.cpp
├── __slic.cpp
├── __view.cpp
├── _eigenframes.cpp
├── _framerec.hpp
├── _private.hpp
├── _slicAlgorithm.cpp
├── _slicAlgorithm.hpp
├── slicPrecomp.hpp
├── eigenframes.cpp
├── eiten
├── eigenfaces.cpp
├── createCsv.py
├── images.csv
├── build
│   ├── slic
│   ├── eigen
│   ├── ...
├── data
│   ├── file1.csv
│   ├── file2.csv
│   ├── ...
├── images
│   ├── person1
│   │   ├── image1
│   │   ├── image2
│   │   ├── ...
│   ├── person2
│   │   ├── image1
│   │   ├── image2
│   │   ├── ...
│   ├── ...
├── results
│   ├── superpixels_image1
│   ├── superpixels_image2
│   ├── ...
├── readme.md
```
<hr>

## Build and Run
* #### Segmentation Part
Assuming that images are located in the images directory, and hierarchy is as above; you can run the project by executing the following: 
```
cd build
cmake ..
make 

./slic
```
Above runs SlicO,  and below runds Slic  
```
./slic -a=0
```

The output will be saved into data directory, as csv files.
<hr>

* ####  PCA Part
To run the PCA part afterward on the generated csv files, you can execute the following:  
```
./eigen 
```
Or for a specific face test:
```
./eigen -test=285
```

If `images.csv` needed to be edited, run the python command from build directory, to ensure that the hierarchy structre is the same.
**Example:**

```
python ../create_csv.py ../images
```
<hr>

## AT&T Face Database
Here is the [link](http://www.cl.cam.ac.uk/research/dtg/attarchive/facedatabase.html
) for the AT&T face database.

