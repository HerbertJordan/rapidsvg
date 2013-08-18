// Petter Strandmark 2013.

#pragma once

#include <vector>
#include "shape.h"

namespace rapidsvg {

// Represents a line in the SVG file.
class Polygon : public Shape
{
public:
	Polygon() { }

	std::vector<std::pair<float, float> > points;

	virtual void draw() const;

	// Parses a style string and modifies the polygon.
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
