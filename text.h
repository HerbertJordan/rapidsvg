// Petter Strandmark 2013.

#pragma once

#include <vector>
#include <string>
#include "shape.h"

namespace rapidsvg {

// Represents a line in the SVG file.
class Text : public Shape
{
public:
	Text() : x(0), y(0) { }

	float x;				// position X
	float y;				// position Y
	std::string text;

	virtual void draw() const;

	// Parses a style string and modifies the polygon.
	// Also modifies the string itself.
	void parse_style(char* style);

	// Parses a string of points and adds them
	// to the polygon.
	// Also modifies the string itself.
	void parse_points(char* style);

	float getMinX() const;
	float getMinY() const;
	float getMaxX() const;
	float getMaxY() const;

private:
	void parse_style_entry(char* style);
};

}
