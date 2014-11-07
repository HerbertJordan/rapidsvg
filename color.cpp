// Herbert Jordan 2013

#include "color.h"

namespace rapidsvg {

	const Color NONE;

	Color::operator bool() const {
		return this != &NONE;
	}

	// some color constants
	const Color BLACK(0,0,0);
	const Color WHITE(255,255,255);

} // end namespace rapidsvg
