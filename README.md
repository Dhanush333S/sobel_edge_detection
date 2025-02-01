For Images of type PGM :

> gcc main.c -lm

Then it asks file name and enter the file name 

> Eg : lena.pgm

For Images of type JPG: 

Some installation needs to be done :

> sudo apt install libjpeg-dev

Run :
 
> gcc ForJpg.c -o sobel_jpg -ljpeg -lm

then run the sobel_jpg file : 

> Eg : sobel_jpg lena.jpg output.jpg

The steps in the algo is as follows :

1. IMG -> GRAYSCALE IMG

2. GRAYSCALE IMG -> Transformed using two Kernels in 2 direction axis as in the code Gx and Gy Kernels

3. Output the file in the required format. 

PGM is easier to read and write but can't be viewed as Graphical object.

So , second version with JPG read and write is implemented with help of lib. 