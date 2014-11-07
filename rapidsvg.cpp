// Petter Strandmark 2013.

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef USE_OPENMP
	#include <omp.h>
#else
	#include <ctime>
	namespace 
	{
		double omp_get_wtime()
		{
			return std::time(0);
		}
	}
#endif


#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h> // glut.h includes gl.h.
#endif

#include "line.h"
#include "svg_file.h"


namespace rapidsvg {

// SVG file currently opened.
SVGFile svg_file;

// Part of the SVG currently being viewed.
float view_left   = 0.0f;
float view_right  = 1.0f;
float view_bottom = 0.0f;
float view_top    = 1.0f;

void center_display(int view_x, int view_y, float radius_x, float radius_y)
{
	float window_width  = float(glutGet(GLUT_WINDOW_WIDTH));
	float window_height = float(glutGet(GLUT_WINDOW_HEIGHT));

	float x = view_left   + float(view_x) / window_width;
	float y = view_bottom + float(view_x) / window_height;

	view_left   = x - radius_x;
	view_right  = x + radius_x;
	view_bottom = y - radius_y;
	view_top    = y + radius_y;

	glLoadIdentity ();
	glOrtho(view_left, view_right, view_bottom, view_top, 0.0, 1.0);
	glutPostRedisplay();
}

void center_display_zoom(float x, float y, float radius_x, float radius_y)
{
	//float x = (view_right + view_left) / 2.0f;
	//float y = (view_top + view_bottom) / 2.0f;

	view_left   = x - radius_x;
	view_right  = x + radius_x;
	view_bottom = y - radius_y;
	view_top    = y + radius_y;

	glLoadIdentity ();
	glOrtho(view_left, view_right, view_bottom, view_top, 0.0, 1.0);
	glutPostRedisplay();
}

void center_display_delta(int delta_view_x, int delta_view_y)
{
	float window_width  = float(glutGet(GLUT_WINDOW_WIDTH));
	float window_height = float(glutGet(GLUT_WINDOW_HEIGHT));

	float dx = (view_right - view_left) * float(delta_view_x) / window_width;
	float dy = (view_top - view_bottom) * float(delta_view_y) / window_height;

	view_left   += dx;
	view_right  += dx;
	view_bottom += dy;
	view_top    += dy;

	glLoadIdentity ();
	glOrtho(view_left, view_right, view_bottom, view_top, 0.0, 1.0);
	glutPostRedisplay();
}

bool left_button_down = false;
int left_drag_start_x = 0;
int left_drag_start_y = 0;

void mouse_move(int x, int y)
{
	if (left_button_down) {
		center_display_delta(left_drag_start_x - x, y - left_drag_start_y);
		left_drag_start_x = x;
		left_drag_start_y = y;
	}
}

void scale(float sx, float sy) {
	float radius_x = (view_right - view_left) / 2.0f;
	float radius_y = (view_top - view_bottom) / 2.0f;

	radius_x *= sx;
	radius_y *= sy;

	// compute center of scaling
	float window_width  = float(glutGet(GLUT_WINDOW_WIDTH));
	float window_height = float(glutGet(GLUT_WINDOW_HEIGHT));

	float center_x = view_left + (view_right - view_left);
	float center_y = view_top + (view_bottom - view_top);

	center_display_zoom(center_x, center_y, radius_x, radius_y);
}

void scale(float s) {
	scale(s,s);
}

void mouse(int button, int action, int x, int y)
{
	//std::cerr << "button=" << button << " action=" << action << " x=" << x << " y=" << y << '\n';

	if (button == 0 && action == 0) {
		left_button_down = true;
		left_drag_start_x = x;
		left_drag_start_y = y;
	}
	if (button == 0 && action == 1) {
		left_button_down = false;
	}

	// Mouse wheel up and down.
	if ((button == 3 || button == 4) && action == 0) {
		float radius_x = (view_right - view_left) / 2.0f;
		float radius_y = (view_top - view_bottom) / 2.0f;

		const float fac = 0.8f;
		if (button == 3) {
			radius_x *= fac;
			radius_y *= fac;
		}
		else if (button == 4) {
			radius_x /= fac;
			radius_y /= fac;
		}

		// compute center of scaling
		float window_width  = float(glutGet(GLUT_WINDOW_WIDTH));
		float window_height = float(glutGet(GLUT_WINDOW_HEIGHT));

		float relative_x = (x / window_width);
		float relative_y = (y / window_height);

		if (button == 4) {
			relative_x = 1 - relative_x;
			relative_y = 1 - relative_y;
		}

		// dampening by 50%
		relative_x = (relative_x + 0.5) / 2.0;
		relative_y = (relative_y + 0.5) / 2.0;

		float center_x = view_left + (view_right - view_left) * relative_x;
		float center_y = view_top + (view_bottom - view_top) * relative_y;

		center_display_zoom(center_x, center_y, radius_x, radius_y);
	}
}

void keyboard (unsigned char key, int x, int y)
{
	//std::cerr << "key=" << int(key) << " x=" << x << " y=" << y << '\n';

	if (key == 'r') {
		svg_file.reload();
		glutPostRedisplay();
	}

	if (key == 'x') {
		scale(1.1f, 1.0f);
		glutPostRedisplay();
	}

	if (key == 'X') {
		scale(1/1.1f,1.0f);
		glutPostRedisplay();
	}

}


void display(void)
{
	using namespace std;

	static bool first_time = true;
	double start_time, end_time;
	start_time = ::omp_get_wtime();

	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_POLYGON_SMOOTH );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );

	// render polygons
	for (auto& polygon : svg_file.polygons) {
		polygon.draw();
	}

	for (auto& ellipse : svg_file.ellipses) {
		ellipse.draw();
	}

	for (auto& line : svg_file.lines) {
		line.draw();
	}

	for (auto& path : svg_file.paths) {
		path.draw();
	}

	// use some culling for the texts ...
	if (view_bottom - view_top < 10000) {
		for (auto& text : svg_file.texts) {
			if (view_top < text.getMaxY() && text.getMinY() < view_bottom) {
				if (view_left < text.getMaxX() && text.getMinX() < view_right) {
					text.draw();
				}
			}
		}
	}


	// draw a bounding box
//	glBegin(GL_POLYGON);
//	glColor3d(0,100,100);
//	glVertex2f(0,0);
//	glVertex2f(0,svg_file.get_width());
//	glVertex2f(svg_file.get_height(),svg_file.get_width());
//	glVertex2f(svg_file.get_height(),0);
//	glEnd();


	glDisable(GL_BLEND); //restore blending options

	end_time = ::omp_get_wtime();
	if (first_time) {
		std::cerr << "Rendered in " << end_time - start_time << " seconds.\n";
		first_time = false;
	}

	glFlush();
	glutSwapBuffers();
}

void main_function(int argc, char** argv)
{
	using namespace std;

	if (argc <= 1) {
		svg_file.load("example.svg");
	}
	else {
		svg_file.load(argv[1]);
	}

	// get image dimensions
	float width = svg_file.get_width();
	float height = svg_file.get_height();

	float window_width = 2000;
	float window_height = 1200;

//	float window_width = 2000;
//	float window_height = window_width / width   * height;

std::cout << "ImgWidth:  " << width << "\n";
std::cout << "ImgHeight: " << height << "\n";

	height = width / window_width * window_height;


std::cout << "Width:     " << width << "\n";
std::cout << "Height:    " << height << "\n";
std::cout << "WndWidth:  " << window_width << "\n";
std::cout << "WndHeight: " << window_height << "\n";


	// From the beginning, look at the entire SVG.
	view_left   = 0;
	view_right  = width;
	view_bottom = 0;
	view_top    = -height;


//	// adjust image view-ratios to fit window size ratio
//	float wnd_ratio = window_width / window_height;
//
//	float img_ratio = width / height;
//
//	if ((view_right * img_ratio) > height) {
//		view_top = -view_right / img_ratio;
//	} else {
//		view_right = view_right * img_ratio;
//	}
//
//	// centering image
//	float offset_width = (width - view_right)/2;
//	view_left += offset_width;
//	view_right -= offset_width;
//
//	float offset_height = (height - (-view_top))/2;
//	view_bottom -= offset_height;
//	view_top += offset_height;


	// Start OpenGL.
	glutInit(&argc,argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA );
	glutInitWindowSize(window_width,window_height);
	glutCreateWindow("RapidSVG");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(mouse_move);
	glLoadIdentity ();
	glOrtho(view_left, view_right, view_bottom, view_top, 0.0, 1.0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glutMainLoop();
}

}

int main(int argc, char** argv)
{
	if (argc == 1) {
		std::cerr << "Usage: " << argv[0] << " <filename>\n";
		return 0;
	}
	try {
		rapidsvg::main_function(argc, argv);
	}
	catch (std::exception& e) {
		std::cerr << "ERROR: " << e.what() << std::endl;
		std::cerr << "Press enter.";
		std::cin.get();
		return 1;
	}
}
