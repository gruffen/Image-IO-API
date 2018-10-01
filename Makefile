main:  
	g++ -std=c++11 -stdlib=libc++ ReadWriteImage.cpp -I/usr/local/include -L/usr/local/lib -ljpeg -lpng -ltiff -o ReadWriteImage
