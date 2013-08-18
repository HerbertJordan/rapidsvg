// Herbert Jordan 2013.

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "glut.h"
#include "ellipse.h"
#include "svg_file.h"

namespace rapidsvg {

void Ellipse::draw() const {
	const float DEG2RAD = 3.1415/180;

	// render area
	if (fill) {
		glBegin(GL_TRIANGLE_FAN);
		glColor3f(fillColor.r, fillColor.g, fillColor.b);

		// start with center point
		glVertex2f(cx, cy);

		// and first point
		glVertex2f(cx, cy + ry);

		// generate points on surface
		for(int i=0;i<360;i+=5) {
			float rad = i*DEG2RAD;
			glVertex2f(cos(rad)*rx + cx, sin(rad)*ry + cy);
		}

		// the first point again to close the fan
		glVertex2f(cx, cy + ry);

		glEnd();
	}

	// render borderline
	if (borderLine) {
		glBegin(GL_LINE_LOOP);
		glColor3f(strokeColor.r, strokeColor.g, strokeColor.b);
		// generate points on surface
		for(int i=0;i<360;i+=5) {
			float rad = i*DEG2RAD;
			glVertex2f(cos(rad)*rx + cx, sin(rad)*ry + cy);
		}
		glEnd();
	}
}

void Ellipse::parse_style_entry(char* style)
{
//	using namespace std;
//
//	char* name = style;
//	char* value = style;
//	while (*(++style)) {
//		if (*style == ':') {
//			*style = '\0';
//			value = style + 1;
//			break;
//		}
//	}
//
//	if (strcmp(name, "stroke-width") == 0) {
//		this->width = float(std::atof(value));
//	}
//	else if (strcmp(name, "stroke") == 0) {
//		parse_color(value, &this->r, &this->g, &this->b);
//	}
}

void Ellipse::parse_style(char* style) {
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
