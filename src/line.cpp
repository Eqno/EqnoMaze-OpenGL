#include "../include/line.h"
#include "../include/utils.h"

void getLine(double x1, double y1, double x2, double y2, std::vector <Point> &res)
{
	double dx = x2-x1, dy = y2-y1, step = std::max(abs(dx), abs(dy)),
		xi = dx / step, yi = dy / step;
	for (; step --> 0; x1+=xi, y1+=yi) res.push_back((Point) {x1, 0, y1});
}

void getLine(const Point &a, const Point &b, std::vector <Point> &res)
{ getLine(a.x, a.z, b.x, b.z, res); }