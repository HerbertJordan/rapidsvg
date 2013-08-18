#pragma once

#include "shape.h"

namespace rapidsvg {

// Represents a line in the SVG file.
class Line : public Shape
{
public:
	Line() : x1(0), y1(0), x2(0), y2(0),
	         width(1)
	{ }

	float x1, y1, x2, y2;
	float width;

	virtual void draw() const;

	// Parses a style string and modifies the line.
	// Also modifies the string itself.
	void parse_style(char* style);

private:
	void parse_style_entry(char* style);
};

}

