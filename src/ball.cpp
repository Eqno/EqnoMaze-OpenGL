#include <ball.h>
#include <config.h>
#include <utils.h>

Point getPoint(double u, double v)
{
    return (Point){
        sin(pi*v)*cos(2*pi*u),
        sin(pi*v)*sin(2*pi*u),
        cos(pi*v)
    };
}
void getBall(double x, double y, double z,
    double size, std::vector <Point> &ball, int acc)
{
    double u = 0, v = 0, ustep = 1/(double)acc, vstep = ustep;
    for(int i=0; i<acc; i++)
    {
        for(int j=0; j<acc; j++)
        {
            Point a = getPoint(u, v);
            Point b = getPoint(u+ustep, v);
            Point c = getPoint(u+ustep, v+vstep);
            Point d = getPoint(u, v+vstep);
            ball.push_back((Point){(x+a.x)*size, (y+a.y)*size, (z+a.z)*size});
        	ball.push_back((Point){(x+b.x)*size, (y+b.y)*size, (z+b.z)*size});
        	ball.push_back((Point){(x+c.x)*size, (y+c.y)*size, (z+c.z)*size});
            ball.push_back((Point){(x+d.x)*size, (y+d.y)*size, (z+d.z)*size});
			u += ustep;
        }
        v += vstep;
    }
}