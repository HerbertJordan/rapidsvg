// Herbert Jordan 2013

#pragma once

#include "color.h"

namespace rapidsvg {

	struct Shape {

		Color strokeColor;
		Color fillColor;

		bool fill;
		bool borderLine;

		Shape() : fill(true), borderLine(true) {}
		virtual ~Shape() {}

		// request this shape to be drawn (using the global GL state)
		virtual void draw() const =0;

	};

} // end namespace rapidsvg
