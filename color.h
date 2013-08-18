// Herbert Jordan 2013

#pragma once

#include <initializer_list>

namespace rapidsvg {

	struct Color {
		float r,g,b;
		Color() : r(0), g(0), b(0) {}
		Color(float r, float g, float b) : r(r), g(g), b(b) {}
		Color(const Color& other) : r(other.r), g(other.g), b(other.b) {}
	};

	// some color constants
	extern const Color BLACK;
	extern const Color WHITE;

} // end namespace rapidsvg
