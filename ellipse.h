// Herbert Jordan 2013.

#pragma once

#include <vector>

namespace rapidsvg {

// Represents a ellipse in the SVG file.
struct Ellipse {

	float cx, cy; // center point
	float rx, ry; // major / minor radius

	Ellipse() : cx(0), cy(0), rx(0), ry(0) { }

	// Parses a style string and modifies the polygon.
	// Also modifies the string itself.
	void parse_style(char* style);

private:
	void parse_style_entry(char* style);
};

} // end namespace rapidsvg
