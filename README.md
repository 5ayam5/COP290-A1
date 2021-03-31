# COP290 #
## 2019CS10399 Sayam Sethi ##
## 2019CS50440 Mallika Prabhakar ##

# Subtask 1 #
To run the program, do the following:
## Step 1: Check Dependencies ##
1. [CMake](https://cmake.org/install/)
2. [OpenCV](https://docs.opencv.org/master/df/d65/tutorial_table_of_content_introduction.html)
3. C++ Compiler

## Step 2: Terminal Commands ##
Run the following commands in the terminal from the directory of the project folder.
```
>> cmake CMakeLists.txt
>> make all
>> ./correctPerspective "imageName.jpg"
```

## Step 3: Select Points and Witness the Magic ##
If everything has gone well till now, you will be able to see the image in a window. Now,
1. Click on the four corners of the road/object you want to be projected upon vertically in one of the following order:
    1. `top left, top right, bottom right, bottom left`
    2. `top left, top right, bottom left, bottom right`
2. Press enter (or any key) to view the projected image. You can press escape at any time to terminate the program too, this will abort the image processing.
3. If you want to save the processed image and view the cropped image, press any key. To abort without saving the image, press escape.
4. To save the cropped image, press any key. Again, to abort without saving, press escape.

## Step 4: View the Saved Images ##
The two images will be saved as `<imageName>Projected.jpg` and `<imageName>Cropped.jpg` in the same directory as the original image.


# Subtask 2 #
To run the program, do the following:
## Step 1: Check Dependencies ##
1. [CMake](https://cmake.org/install/)
2. [OpenCV](https://docs.opencv.org/master/df/d65/tutorial_table_of_content_introduction.html)
3. C++ Compiler

## Step 2: Terminal Commands ##
Run the following commands in the terminal from the directory of the project folder.
```
>> cmake CMakeLists.txt
>> make all
>> ./computeDensities "videoName.mp4" > "outFileName.csv"
```

## Step 3: Select Points and Witness the Magic ##
If everything has gone well till now, you will be able to see the first frame in a window. Now,
1. Click on the four corners of the road/object you want to be projected upon vertically in one of the following order:
    1. `top left, top right, bottom right, bottom left`
    2. `top left, top right, bottom left, bottom right`
2. Press enter (or any key) to begin the density processing. The output will be stored in the csv file `outFileName.csv`.
3. To view the plot, run the `plot.ipynb` jupyter notebook.

# Subtask 3 #
To run the program, do the following:
## Step 1: Check Dependencies ##
1. [CMake](https://cmake.org/install/)
2. [OpenCV](https://docs.opencv.org/master/df/d65/tutorial_table_of_content_introduction.html)
3. C++ Compiler

## Step 2: Perform analysis ##
Simply run:
```
>> ./run.sh
```
And input the parent directory `../analysis` when requested for input. The code for all the methods will be run and then the utility will also be evaluated.

## Step 3: Plot the graphs ##
To plot the graphs, run `plot.ipynb` (in the `analysis` folder) and enter the file address and then `1` or `0` if you want separate plots or not for the seaparate columns, respectively.
