#ifndef _DUNE_MATH_TOOLS_H_
#define _DUNE_MATH_TOOLS_H_

namespace dune
{
namespace math
{

	const double PI = 3.1415926;
	const double TWO_PI = 6.2831852;

	double minAngleDistance(double a1, double a2)
	{
		double diff = std::fabs(a1 - a2);
		if (diff > PI)
		{
			return fabs(diff - TWO_PI);
		}
		else
		{
			return diff;
		}
	}
}

}


#endif
