// COMP1521 Final Exam
// Read points and determine bounding box

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// Data type definitions

// all values are in the range 0..255
typedef unsigned char Byte;

// an (x,y) coordinate
typedef struct {
	Byte x;
	Byte y;
} Coord;

// a colour, given as 3 bytes (r,g,b)
typedef struct {
	Byte r;
	Byte g;
	Byte b;
} Color;

// a Point has a location and a colour
typedef struct {
	Coord coord;  // (x,y) location of Point
	Color color;  // colour of Point
} Point;

void boundingBox(int, Coord *, Coord *);

int main(int argc, char **argv)
{
	// check command-line arguments
	if (argc < 2) {
		fprintf(stderr, "Usage: %s PointsFile\n", argv[0]);
		exit(1);
	}

	// attempt to open specified file
	int in = open(argv[1],O_RDONLY);
	if (in < 0) {
		fprintf(stderr, "Can't read %s\n", argv[1]);
		exit(1);
	}

	// collect coordinates for bounding box
	Coord topLeft, bottomRight;
	boundingBox(in, &topLeft, &bottomRight);

	printf("TL=(%d,%d)  BR=(%d,%d)\n",
		 topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);

	// clean up
	close(in);
	return 0;
}

void boundingBox(int in, Coord *TL, Coord *BR)
{
	// TODO
	Byte smallest_x = 250, smallest_y = 255;
	Byte largest_x = 250, largest_y = 0;
	

	// data file consists of one or more points in binary form
	Point p;	
	while (read(in, &p, sizeof(p)) != 0) {
		// find largest/smallest on x plane
		if (p.coord.x > largest_x)
			largest_x = p.coord.x;
		if (p.coord.x < smallest_x) 
			smallest_x = p.coord.x;
		// find largest/smallest on y plane
		if (p.coord.y > largest_y)
			largest_y = p.coord.y;
		if (p.coord.y < smallest_y) 
			smallest_y = p.coord.y;
	}

	// assign small(x) && large(y) to top_left
	TL->x = smallest_x;
	TL->y = largest_y;
	// assign small(y) && large(x) to bottom_right
	BR->x = largest_x;
	BR->y = smallest_y;
}
