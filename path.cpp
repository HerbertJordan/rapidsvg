// Herbert Jordan 2013.

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "glut.h"
#include "path.h"
#include "svg_file.h"

namespace rapidsvg {


void Path::draw() const {

	// render line
	if (borderLine) {
		glBegin(GL_LINE_STRIP);
		glColor3f(strokeColor.r, strokeColor.g, strokeColor.b);
		for (auto& point : points) {
			glVertex2f(point.first, point.second);
		}
		glEnd();
	}

}

void Path::parse_style_entry(char* style)
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

void Path::parse_style(char* style)
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

void Path::parse_points(char* points)
{
	/**
	 * This is the most horrible code i ever wrote - do not read it, replace it asap
	 */

	//std::cout << "Parsing points: " << points << "\n";
	char* start = points;
	int pair_pos = 0;
	std::pair<float, float> point;

	// erase letters
	char* pos = points;
	while(*pos) {
		if (*pos == 'M' || *pos == 'C') *pos = ' ';
		pos++;
	}
	//std::cout << "Parsing points: " << points << "\n";

	// move to start
	while(*points==' ') {
		points++;
	}

	while (*points) {
		if (*points == '\0' || *points == ',' || *points == ' ' || *points == 'M' || *points == 'C') {
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

	// add final point
	point.second = float(std::atof(start));
	this->points.push_back(point);

	/*
	std::cout << "Path: ";
	for(auto& cur : this->points) {
		std::cout << "(" << cur.first << "," << cur.second << "), ";
	}
	std::cout << "\n";
	*/
}

}
