// Petter Strandmark 2013.

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "glut.h"
#include "polygon.h"
#include "svg_file.h"

namespace rapidsvg {


void Polygon::draw() const {

	// render area
	if (fill) {
		glBegin(GL_POLYGON);
		glColor3f(fillColor.r, fillColor.g, fillColor.b);
		for (auto& point : points) {
			glVertex2f(point.first, point.second);
		}
		glEnd();
	}

	// render borderline
	if (borderLine) {
		glBegin(GL_LINE_LOOP);
		glColor3f(strokeColor.r, strokeColor.g, strokeColor.b);
		for (auto& point : points) {
			glVertex2f(point.first, point.second);
		}
		glEnd();
	}

}

void Polygon::parse_style_entry(char* style)
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

	if (strcmp(name, "fill") == 0) {
		parse_color(value, fillColor);
	}
	else if (strcmp(name, "stroke") == 0) {
		parse_color(value, strokeColor);
	}
}

void Polygon::parse_style(char* style)
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

void Polygon::parse_points(char* points)
{
	char* start = points;
	int pair_pos = 0;
	std::pair<float, float> point;
	while (*points) {
		if (*points == '\0' || *points == ',' || *points == ' ') {
			if (*points != '\0') {
				*points = '\0';
				points++;
			}

			if (pair_pos == 0) {
				point.first = float(std::atof(start));
				pair_pos = 1;
			}
			else {
				point.second = float(std::atof(start));
				this->points.push_back(point);
				pair_pos = 0;
			}

			start = points;
		}
		else {
			points++;
		}
	}
}

}
