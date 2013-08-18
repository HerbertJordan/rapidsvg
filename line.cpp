// Petter Strandmark 2013.

#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include <cmath>

#include "glut.h"
#include "line.h"
#include "svg_file.h"

namespace rapidsvg {

namespace {

	// Draws a line between (x1,y1) - (x2,y2) with a start thickness of t1 and
	// end thickness t2.
	void draw_line(float x1, float y1, float x2, float y2, float t1, float t2)
	{
		float angle = std::atan2(y2 - y1, x2 - x1);
		float t2sina1 = t1 / 2 * std::sin(angle);
		float t2cosa1 = t1 / 2 * std::cos(angle);
		float t2sina2 = t2 / 2 * std::sin(angle);
		float t2cosa2 = t2 / 2 * std::cos(angle);

		glBegin(GL_TRIANGLES);
		glVertex2f(x1 + t2sina1, y1 - t2cosa1);
		glVertex2f(x2 + t2sina2, y2 - t2cosa2);
		glVertex2f(x2 - t2sina2, y2 + t2cosa2);
		glVertex2f(x2 - t2sina2, y2 + t2cosa2);
		glVertex2f(x1 - t2sina1, y1 + t2cosa1);
		glVertex2f(x1 + t2sina1, y1 - t2cosa1);
		glEnd();
	}

}

void Line::draw() const {
	glColor3d(strokeColor.r, strokeColor.g, strokeColor.b);
	draw_line(x1, y1, x2, y2, width, width);
}

void Line::parse_style_entry(char* style)
{
	using namespace std;

	char* name = style;
	char* value = style;
	while (*(++style)) {
		if (*style == ':') {
			*style = '\0';
			value = style + 1;
			break;
		}
	}

	if (strcmp(name, "stroke-width") == 0) {
		this->width = float(std::atof(value));
	}
	else if (strcmp(name, "stroke") == 0) {
		parse_color(value, strokeColor);
	}
}

void Line::parse_style(char* style)
{
	char* start = style;
	while (true) {
		if (*style == '\0') {
			if (*start) {
				parse_style_entry(start);
			}
			return;
		}
		else if (*style == ';') {
			*style = '\0';
			parse_style_entry(start);
			start = style + 1;
		}
		style++;
	}
}

}
