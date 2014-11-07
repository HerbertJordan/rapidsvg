// Petter Strandmark 2013.

#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <stdexcept>

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

#include <rapidxml.hpp>

#include "svg_file.h"

namespace rapidsvg {

// Reads a binary file into a vector of char.
void read_file_data(const std::string& file_name, std::vector<char>* data)
{
	std::fstream fin(file_name, std::ios::binary | std::ios::in );
	if (!fin) {
		throw std::runtime_error("Could not open file.");
	}
	fin.seekg(0, std::ios::end);
	size_t file_size = fin.tellg();
	fin.seekg(0, std::ios::beg);
	data->resize(file_size + 1);
	if (!fin.read(&data->at(0), file_size)) {
		throw std::runtime_error("Failed to read file.");
	}
}

int hex_to_dec(char d1)
{
	if ('0' <= d1 && d1 <= '9') {
		return d1 - '0';
	}
	switch (d1) {
	case 'a': case 'A': return 10;
	case 'b': case 'B': return 11;
	case 'c': case 'C': return 12;
	case 'd': case 'D': return 13;
	case 'e': case 'E': return 14;
	case 'f': case 'F': return 15;
	}
	std::string err = "Invalid hex digit :";
	err += d1;
	throw std::runtime_error(err);
}

void parse_color(const char* color, Color& c) {

	if (std::strcmp(color, "none") == 0) {
		c = NONE;
	}
	else if (std::strcmp(color, "black") == 0) {
		c = BLACK;
	}
	else if (std::strcmp(color, "white") == 0) {
		c = WHITE;
	}
	else if (std::strcmp(color, "red") == 0) {
		c = (Color){255,0,0};
	}
	else if (std::strcmp(color, "green") == 0) {
		c = (Color){0,255,0};
	}
	else if (std::strcmp(color, "blue") == 0) {
		c = (Color){0,0,255};
	}
	else if (std::strcmp(color, "yellow") == 0) {
		c = (Color){255,255,0};
	}
	else if (strlen(color) == 7 && color[0] == '#') {
		// Hexadecimal color.
		c.r = float(15 * hex_to_dec(color[1]) + hex_to_dec(color[2])) / 255.0f;
		c.g = float(15 * hex_to_dec(color[3]) + hex_to_dec(color[4])) / 255.0f;
		c.b = float(15 * hex_to_dec(color[5]) + hex_to_dec(color[6])) / 255.0f;
	}
	else if (strlen(color) == 5 && color[0] == '#' && color[1] == ' ') {
		// ?
		c.r = 0;
		c.g = 0;
		c.b = 0;
	}
	else {
		std::string err = "Invalid color : ";
		err += color;
		throw std::runtime_error(err);
	}
}

SVGFile::SVGFile() :
	width(0),
	height(0)
{
}

void SVGFile::clear()
{
	this->lines.clear();
	this->polygons.clear();
}

void SVGFile::reload()
{
	if (this->filename.length() > 0) {
		this->load(this->filename);
	}
	else {
		throw std::runtime_error("No file previously loaded.");
	}
}

void SVGFile::load(const std::string& input_filename)
{
	using namespace std;
	using namespace rapidxml;
	double start_time, end_time;

	this->filename = input_filename;
	this->clear();

	start_time = ::omp_get_wtime();
	std::vector<char> data;
	read_file_data(filename, &data);

	end_time = ::omp_get_wtime();
	std::cerr << "Read file in " << end_time - start_time << " seconds.\n";

	start_time = ::omp_get_wtime();
	xml_document<> doc;
	data.push_back(0);
	doc.parse<0>(&data[0]);
	end_time = ::omp_get_wtime();
	std::cerr << "Parsed XML in " << end_time - start_time << " seconds.\n";

	start_time = ::omp_get_wtime();

	xml_node<>* svg = doc.first_node("svg");
	if (!svg) {
		throw std::runtime_error("No <svg> node.");
	}

	this->width = 1;
	this->height = 1;

	for (auto attr = svg->first_attribute(); attr;
	          attr = attr->next_attribute())
	{
		if (strcmp(attr->name(), "width") == 0) {
			this->width = float(atof(attr->value()));
		}
		else if (strcmp(attr->name(), "height") == 0) {
			this->height = float(atof(attr->value()));
		}
	}

	// Queue of nodes we need to explore.
	queue<xml_node<>*> nodes;
	nodes.push(svg);

	// Process the queue.
	while ( !nodes.empty()) {
		auto node = nodes.front();
		nodes.pop();

		// For each child of this node.
		for (auto child = node->first_node(); child;
		          child = child->next_sibling()) {
			if (strcmp(child->name(), "g") == 0) {
				// Found a group; add it to queue.
				nodes.push(child);
			}
			else if (strcmp(child->name(), "line") == 0) {
				// Add line to the collection of lines.
				lines.push_back(Line());
				Line& line = lines.back();

				// To through the line attributes.
				for (xml_attribute<> *attr = child->first_attribute();
						attr; attr = attr->next_attribute())
				{
					if (strcmp(attr->name(), "x1") == 0) {
						line.x1 = float(atof(attr->value()));
					}
					else if (strcmp(attr->name(), "x2") == 0) {
						line.x2 = float(atof(attr->value()));
					}
					else if (strcmp(attr->name(), "y1") == 0) {
						line.y1 = float(atof(attr->value()));
					}
					else if (strcmp(attr->name(), "y2") == 0) {
						line.y2 = float(atof(attr->value()));
					}
					else if (strcmp(attr->name(), "style") == 0) {
						// Process this style string.
						line.parse_style(attr->value());
					}
				}
			}
			else if (strcmp(child->name(), "polygon") == 0) {
				// Add line to the collection of lines.
				polygons.push_back(Polygon());
				Polygon& polygon = polygons.back();

				// Go through the line attributes.
				for (xml_attribute<> *attr = child->first_attribute();
						attr; attr = attr->next_attribute())
				{
					if (strcmp(attr->name(), "points") == 0) {
						polygon.parse_points(attr->value());
					}
					else if (strcmp(attr->name(), "style") == 0) {
						// Process this style string.
						polygon.parse_style(attr->value());
					}
					else if (strcmp(attr->name(), "fill") == 0) {
						// Process fill attribute
						if (strcmp(attr->value(), "none") == 0) {
							polygon.fill = false;
						} else {
							polygon.fill = true;
							parse_color(attr->value(), polygon.fillColor);
						}
					}
					else if (strcmp(attr->name(), "stroke") == 0) {
						// Process fill attribute
						if (strcmp(attr->value(), "none") == 0) {
							polygon.borderLine = false;
						} else {
							polygon.borderLine = true;
							parse_color(attr->value(), polygon.strokeColor);
						}
					}
				}
			}
			else if (strcmp(child->name(), "ellipse") == 0) {
				// Add ellipse to the vector of ellipses
				ellipses.push_back(Ellipse());
				Ellipse& ellipse = ellipses.back();

				// Go through the ellipse attributes.
				for (xml_attribute<> *attr = child->first_attribute();
						attr; attr = attr->next_attribute())
				{
					if (strcmp(attr->name(), "cx") == 0) {
						ellipse.cx = float(atof(attr->value()));
					}
					else if (strcmp(attr->name(), "cy") == 0) {
						ellipse.cy = float(atof(attr->value()));
					}
					else if (strcmp(attr->name(), "rx") == 0) {
						ellipse.rx = float(atof(attr->value()));
					}
					else if (strcmp(attr->name(), "ry") == 0) {
						ellipse.ry = float(atof(attr->value()));
					}
					else if (strcmp(attr->name(), "style") == 0) {
						// Process this style string.
						ellipse.parse_style(attr->value());
					}
					else if (strcmp(attr->name(), "fill") == 0) {
						// Process fill attribute
						if (strcmp(attr->value(), "none") == 0) {
							ellipse.fill = false;
						} else {
							ellipse.fill = true;
							parse_color(attr->value(), ellipse.fillColor);
						}
					}
					else if (strcmp(attr->name(), "stroke") == 0) {
						// Process fill attribute
						if (strcmp(attr->value(), "none") == 0) {
							ellipse.borderLine = false;
						} else {
							ellipse.borderLine = true;
							parse_color(attr->value(), ellipse.strokeColor);
						}
					}
				}
			}
			else if (strcmp(child->name(), "path") == 0) {
				// Add line to the collection of lines.
				paths.push_back(Path());
				Path& path = paths.back();

				// Go through the line attributes.
				for (xml_attribute<> *attr = child->first_attribute();
						attr; attr = attr->next_attribute())
				{
					if (strcmp(attr->name(), "d") == 0) {
						path.parse_points(attr->value());
					}
					else if (strcmp(attr->name(), "style") == 0) {
						// Process this style string.
						path.parse_style(attr->value());
					}
					else if (strcmp(attr->name(), "stroke") == 0) {
						// Process fill attribute
						if (strcmp(attr->value(), "none") == 0) {
							path.borderLine = false;
						} else {
							path.borderLine = true;
							parse_color(attr->value(), path.strokeColor);
						}
					}
				}
			}
			else if (strcmp(child->name(), "text") == 0) {
				// add a text to the collection of texts
				texts.push_back(Text());
				Text& text = texts.back();

				//std::cout << "Node Value: " << child->value() << "\n";
				text.text = child->value();

				// Go through the text attributes.
				for (xml_attribute<> *attr = child->first_attribute();
						attr; attr = attr->next_attribute())
				{
					if (strcmp(attr->name(), "x") == 0) {
						text.x = float(atof(attr->value()));
					}
					else if (strcmp(attr->name(), "y") == 0) {
						text.y = float(atof(attr->value()));
					}
					else if (strcmp(attr->name(), "style") == 0) {
						// Process this style string.
						text.parse_style(attr->value());
					}
					else if (strcmp(attr->name(), "stroke") == 0) {
						// Process fill attribute
						if (strcmp(attr->value(), "none") == 0) {
							text.borderLine = false;
						} else {
							text.borderLine = true;
							parse_color(attr->value(), text.strokeColor);
						}
					}
				}

				// <text text-anchor="middle" x="1458.5" y="-151.4" font-family="Times Roman,serif" font-size="14.00">s4 sub s5</text>

			}
		}
	}
	end_time = ::omp_get_wtime();
	std::cerr << "Walked XML in " << end_time - start_time << " seconds.\n";
	
	std::cerr << "SVG is " << this->width << " x " << this->height << "\n";
	std::cerr << "Found " << lines.size() << " lines.\n";
	std::cerr << "Found " << paths.size() << " paths.\n";
	std::cerr << "Found " << polygons.size() << " polygons.\n";
	std::cerr << "Found " << ellipses.size() << " ellipses.\n";
	std::cerr << "Found " << texts.size() << " texts.\n";
}

}
