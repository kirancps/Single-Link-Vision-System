#include "stdafx.h"
#include "Fruit.h"


Fruit::Fruit()
{
	setType("null");
	setColour(Scalar(0, 0, 0));
}

Fruit::Fruit(string name) {

	setType(name);

	if (name == "Object") {

		//TODO: use "calibration mode" to find HSV min
		//and HSV max values

		setHSVmin(Scalar(0, 2, 0));
		setHSVmax(Scalar(7, 247, 255));

		//BGR value for Green:
		setColour(Scalar(0, 255, 255));

	}
	if (name == "banana") {

		//TODO: use "calibration mode" to find HSV min
		//and HSV max values

		setHSVmin(Scalar(0, 0, 0));
		setHSVmax(Scalar(255, 255, 255));

		//BGR value for Yellow:
		setColour(Scalar(0, 255, 255));

	}
	if (name == "cherry") {

		//TODO: use "calibration mode" to find HSV min
		//and HSV max values

		setHSVmin(Scalar(0, 0, 0));
		setHSVmax(Scalar(255, 255, 255));

		//BGR value for Red:
		setColour(Scalar(0, 0, 255));

	}



}


int Fruit::getXPos() {

	return Fruit::xPos;

}

void Fruit::setXPos(int x) {

	Fruit::xPos = x;

}

int Fruit::getYPos() {

	return Fruit::yPos;

}

void Fruit::setYPos(int y) {

	Fruit::yPos = y;

}

Scalar Fruit::getHSVmin() {

	return Fruit::HSVmin;

}
Scalar Fruit::getHSVmax() {

	return Fruit::HSVmax;
}

void Fruit::setHSVmin(Scalar mini) {

	Fruit::HSVmin = mini;
}


void Fruit::setHSVmax(Scalar maxi) {

	Fruit::HSVmax = maxi;
}



Fruit::~Fruit()
{
}
