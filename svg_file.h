// Petter Strandmark 2013.

#ifndef RAPIDSVG_SVG_FILE_H
#define RAPIDSVG_SVG_FILE_H

#include <string>
#include <vector>

#include "color.h"
#include "line.h"
#include "path.h"
#include "polygon.h"
#include "ellipse.h"
#include "text.h"

namespace rapidsvg {

// Represents a line in the SVG file.
class SVGFile
{
public:
	SVGFile();

	// Load a file from file.
	void load(const std::string& filename);
	void reload();
	void clear();

	double get_width() { return width; }
	double get_height() { return height; }

	// Lines in the SVG.
	std::vector<Line> lines;

	// Paths in the SVG
	std::vector<Path> paths;

	// Polygons in the SVG.
	std::vector<Polygon> polygons;

	// Ellipses in the SVG
	std::vector<Ellipse> ellipses;

	// Texts in the SVG
	std::vector<Text> texts;

private:
	std::string filename;
	double width, height;
};

void parse_color(const char* color, Color& c);

}

#endif
