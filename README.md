The Median was used as a background subtractor. This repository includes 2 types of foreground analyse. It is created in the first type by basic frame and median background differences. In the second, it was applied this [article](https://www.ijcst.org/Volume6/Issue10/p2_6_10.pdf). It was offered in this article; that if we apply an edge algorithm on the frame and background and find differenence image, we can handle illumination changes in the frame like leaf move or shadow.

## Clone the repository
```
https://github.com/kondukberna/BackgroundSubtraction.git
```

## Build
```
mkdir build && build
cmake ..
make
```

## Run
For basic foreground
```
./bgsubtraction -image_path video/video03.avi -folder_path folders
```
If you want to try edge you have to add -edge flag.  
```
./bgsubtraction -image_path video/video03.avi -folder_path folders -edge
```

## Result

Frame

![](https://github.com/kondukberna/BackgroundSubtraction/blob/main/folders/Frame/233.jpg)

Basic Foreground

![](https://github.com/kondukberna/BackgroundSubtraction/blob/main/folders/FG/233.jpg)

Using Edge Foreground

![](https://github.com/kondukberna/BackgroundSubtraction/blob/main/folders/FG_Edge/233.jpg)
