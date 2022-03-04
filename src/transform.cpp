#include <vector>

#include <utils.h>
#include <transform.h>

void transform(std::vector <Point> &v, 
	double a, double b, double c, double d, double e, double f,
	double g, double h, double i, double l, double m, double n, 
	double p, double q, double r, double s)
{
	for (size_t j=0; j<v.size(); j++)
	{
		double x = v[j].x*a+v[j].y*d+v[j].z*g+l;
		double y = v[j].x*b+v[j].y*e+v[j].z*h+m;
		double z = v[j].x*c+v[j].y*f+v[j].z*i+n;
		double e = v[j].x*p+v[j].y*q+v[j].z*r+s;
		x /= e, y /= e, z /= e;
		v[j].x = x, v[j].y = y, v[j].z = z;
	}
}
void translateX(std::vector <Point> &v, double l)
{ transform(v, 1, 0, 0, 0, 1, 0, 0, 0, 1, l, 0, 0, 0, 0, 0, 1); }
void translateY(std::vector <Point> &v, double m)
{ transform(v, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, m, 0, 0, 0, 0, 1); }
void translateZ(std::vector <Point> &v, double n)
{ transform(v, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, n, 0, 0, 0, 1); }
void translate(std::vector <Point> &v, double l, double m, double n)
{ transform(v, 1, 0, 0, 0, 1, 0, 0, 0, 1, l, m, n, 0, 0, 0, 1); }

void scale(std::vector <Point> &v, double s)
{ transform(v, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1/s); }
void scaleX(std::vector <Point> &v, double a)
{ transform(v, a, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1); }
void scaleY(std::vector <Point> &v, double e)
{ transform(v, 1, 0, 0, 0, e, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1); }
void scaleZ(std::vector <Point> &v, double i)
{ transform(v, 1, 0, 0, 0, 1, 0, 0, 0, i, 0, 0, 0, 0, 0, 0, 1); }
void scale(std::vector <Point> &v, double a, double e, double i)
{ transform(v, a, 0, 0, 0, e, 0, 0, 0, i, 0, 0, 0, 0, 0, 0, 1); }

void rotateRX(std::vector <Point> &v, double angle)
{
	double c = cos(angle), s = sin(angle);
	transform(v, 1, 0, 0, 0, c, s, 0, -s, c, 0, 0, 0, 0, 0, 0, 1);
}
void rotateX(std::vector <Point> &v, double angle)
{ rotateRX(v, angle); }
void rotateRY(std::vector <Point> &v, double angle)
{
	double c = cos(angle), s = sin(angle);
	transform(v, c, 0, -s, 0, 1, 0, s, 0, c, 0, 0, 0, 0, 0, 0, 1);
}
void rotateY(std::vector <Point> &v, double angle)
{ rotateRY(v, angle); }
void rotateRZ(std::vector <Point> &v, double angle)
{
	double c = cos(angle), s = sin(angle);
	transform(v, c, s, 0, -s, c, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1);
}
void rotateZ(std::vector <Point> &v, double angle)
{ rotateRZ(v, angle); }
void rotateDX(std::vector <Point> &v, double angle)
{ rotateX(v, angle*atan(1)/45); }
void rotateDY(std::vector <Point> &v, double angle)
{ rotateY(v, angle*atan(1)/45); }

std::vector <Point> initLeftLeg, initRightLeg, initLeftArm, initRightArm;
void initInd(const std::vector <Point> &leftLeg, const std::vector <Point> &rightLeg,
	const std::vector <Point> &leftArm, const std::vector <Point> &rightArm)
{ initLeftLeg = leftLeg, initRightLeg = rightLeg, initLeftArm = leftArm, initRightArm = rightArm; }
void rotateIndLeftLeg(std::vector <Point> &v, double angle)
{ 
	std::vector <Point> res(initLeftLeg);
	rotateY(res, angle*atan(1)/45);
	v = res;
}
void rotateIndRightLeg(std::vector <Point> &v, double angle)
{ 
	std::vector <Point> res(initRightLeg);
	rotateY(res, angle*atan(1)/45);
	v = res;
}
void rotateIndLeftArm(std::vector <Point> &v, double angle)
{ 
	std::vector <Point> res(initLeftArm);
	rotateY(res, angle*atan(1)/45);
	v = res;
}
void rotateIndRightArm(std::vector <Point> &v, double angle)
{ 
	std::vector <Point> res(initRightArm);
	rotateY(res, angle*atan(1)/45);
	v = res;
}
void swingIndArm(double angle, double depth)
{
	translateY(initLeftArm, depth);
	translateY(initRightArm, depth);
	rotateX(initLeftArm, angle*atan(1)/45);
	rotateX(initRightArm, -angle*atan(1)/45);
	translateY(initLeftArm, -depth);
	translateY(initRightArm, -depth);
}
void swingIndLeg(double angle, double depth)
{
	translateY(initLeftLeg, depth);
	translateY(initRightLeg, depth);
	rotateX(initLeftLeg, angle*atan(1)/45);
	rotateX(initRightLeg, -angle*atan(1)/45);
	translateY(initLeftLeg, -depth);
	translateY(initRightLeg, -depth);
}
void bodyTranslateY(double delta)
{
	translateY(initLeftLeg, delta);
	translateY(initRightLeg, delta);
	translateY(initLeftArm, delta);
	translateY(initRightArm, delta);
}

void rotateDZ(std::vector <Point> &v, double angle)
{ rotateZ(v, angle*atan(1)/45); }
void symX(std::vector <Point> &v)
{ transform(v, 1, 0, 0, 0, -1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1); }
void symY(std::vector <Point> &v)
{ transform(v, -1, 0, 0, 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1); }
void symZ(std::vector <Point> &v)
{ transform(v, -1, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1); }
void symXOY(std::vector <Point> &v)
{ transform(v, 1, 0, 0, 0, 1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1); }
void symYOZ(std::vector <Point> &v)
{ transform(v, -1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1); }
void symZOX(std::vector <Point> &v)
{ transform(v, 1, 0, 0, 0, -1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1); }
void symO(std::vector <Point> &v)
{ transform(v, -1, 0, 0, 0, -1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 1); }

void shearX(std::vector <Point> &v, double d, double g)
{ transform(v, 1, 0, 0, d, 1, 0, g, 0, 1, 0, 0, 0, 0, 0, 0, 1); }
void shearY(std::vector <Point> &v, double b, double h)
{ transform(v, 1, b, 0, 0, 1, 0, 0, h, 1, 0, 0, 0, 0, 0, 0, 1); }
void shearZ(std::vector <Point> &v, double c, double f)
{ transform(v, 1, 0, c, 0, 1, f, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1); }
void shear(std::vector <Point> &v, double d, double g, double b, double h, double c, double f)
{ transform(v, 1, b, c, d, 1, f, g, h, 1, 0, 0, 0, 0, 0, 0, 1); }
