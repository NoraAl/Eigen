
# SLICO PCA
This is both segmentation and 

## The Database of Faces Copyrights
Here is the [link](http://www.cl.cam.ac.uk/research/dtg/attarchive/facedatabase.html
) for the AT&T face database.

## Project Hierarchy
Below is the hierarchy for the repository. Files start with underscores are modified library's files, while those with double underscores are the two main project's files:
```
├── CMakeFiles.txt
├── __eigen.cpp
├── __slic.cpp
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
│   ├── ..
├── images
│   ├── person1
│   │   ├── image1
│   │   ├── image2
│   │   ├── ..
│   ├── person2
│   │   ├── image1
│   │   ├── image2
│   │   ├── ..
│   ├── ..
├── results
│   ├── ..
├── readme.md
```
<hr>

## Build and Run
#### Segmentation Part
Assuming that images are located in the images directory, and hierarchy is as above; you can run the project by executing the following: 
```
cd build
cmake ..
make 

./slic ../images.csv ../output
```
The output -the csv files- will be saved into data directory.

#### PCA Part
To run the PCA part afterward on the generated csv files, you can execute the following:  
```
./eigen 
```

If `images.csv` needed to be edited, run the python command from build directory, to ensure that the hierarchy structre is the same.
**Example:**

```
python ../create_csv.py ../images
```

