// Herbert Jordan 2013

#pragma once

#include <vector>
#include <utility>

#include "shape.h"

namespace rapidsvg {

// Represents a line in the SVG file.
struct Path : public Shape {

	std::vector<std::pair<float, float>> points;
	float width;

	Path() : width(1) { }

	virtual void draw() const;

	// Parses a style string and modifies the line.
	// Also modifies the string itself.
	void parse_style(char* style);

	// Parses a string of points and adds them
	// to the polygon.
	// Also modifies the string itself.
	void parse_points(char* style);

private:
	void parse_style_entry(char* style);
};

}

