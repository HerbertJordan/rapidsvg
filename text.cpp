// Petter Strandmark 2013.

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "glut.h"
#include "text.h"
#include "svg_file.h"

namespace rapidsvg {


void Text::draw() const {

	const std::size_t MAX = 1250;

	// test whether there is something to do
	if (text.empty()) return;

	// print the text
	glColor3f(BLACK.r, BLACK.g, BLACK.b);

	const char* str = text.c_str();
	std::size_t len = text.size();

	if (len > MAX) {
		len = MAX + 3;
	}

	glPushMatrix();
	glTranslatef(x-(len/2 * 7),y,0);
	glScalef(0.1, -0.1, 0.1);
	for(std::size_t i = 0; i<len; i++) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
	}

	if (len > MAX) {
		glutStrokeCharacter(GLUT_STROKE_ROMAN, '.');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, '.');
		glutStrokeCharacter(GLUT_STROKE_ROMAN, '.');
	}

	glPopMatrix();
}

void Text::parse_style_entry(char* style)
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

void Text::parse_style(char* style)
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

float Text::getMinX() const {
	return x-(text.size()/2 * 7);
}

float Text::getMinY() const {
	return y;
}

float Text::getMaxX() const {
	return x+(text.size()/2 * 7);
}

float Text::getMaxY() const {
	return y + 10;
}


}
