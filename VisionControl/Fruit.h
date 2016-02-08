#pragma once

#include <string>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;
class Fruit
{
public:
	Fruit();
	~Fruit();
	Fruit(string name);

	int getXPos();
	void setXPos(int x);

	int getYPos();
	void setYPos(int y);

	Scalar getHSVmin();
	Scalar getHSVmax();

	void setHSVmin(Scalar mini);
	void setHSVmax(Scalar maxi);

	string getType() { return type; }
	void setType(string t) { type = t; }

	Scalar getColour() {
		return Colour;
	}
	void setColour(Scalar c) {

		Colour = c;
	}

private:

	int xPos, yPos;
	string type;
	Scalar HSVmin, HSVmax;
	Scalar Colour;
};

