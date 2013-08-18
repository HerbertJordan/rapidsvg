// Petter Strandmark 2013.

#ifndef RAPIDSVG_POLYGON_H
#define RAPIDSVG_POLYGON_H

#include <vector>

namespace rapidsvg {

// Represents a line in the SVG file.
class Polygon
{
public:
	Polygon() : sr(0), sg(0), sb(0), fr(0), fg(0), fb(0), stroke(true), fill(true)
	{ }
	std::vector<std::pair<float, float> > points;
	float sr, sg, sb;			// stroke colour
	float fr, fg, fb;			// fill colour
	bool stroke, fill;

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

#endif
