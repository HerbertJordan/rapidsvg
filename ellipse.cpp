// Herbert Jordan 2013.

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

#include "ellipse.h"
#include "svg_file.h"

namespace rapidsvg {

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
