// Herbert Jordan 2013.

#pragma once

#include <vector>
#include "shape.h"

namespace rapidsvg {

// Represents a ellipse in the SVG file.
struct Ellipse : public Shape {

	float cx, cy; // center point
	float rx, ry; // major / minor radius

	Ellipse() : cx(0), cy(0), rx(0), ry(0) { }

	virtual void draw() const;

	// Parses a style string and modifies the polygon.
	// Also modifies the string itself.
	void parse_style(char* style);

private:
	void parse_style_entry(char* style);
};

} // end namespace rapidsvg
