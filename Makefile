CC=g++

LIBS=-L/usr/local/lib -lopencv_shape -lopencv_stitching -lopencv_objdetect -lopencv_superres -lopencv_videostab -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lopencv_ml -lopencv_imgproc -lopencv_flann -lopencv_viz -lopencv_core

CFLAGS=-I/usr/local/include/opencv -I/usr/local/include -g

all: snakes.cpp
	$(CC) $(CFLAGS) snakes.cpp -o snakes $(LIBS)

clean:
	rm -f main

