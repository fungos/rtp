#ifndef PIXEL_H
#define PIXEL_H

#include <math.h>
#include <algorithm>
#include <QColor>

union PixelNormalized
{
	struct
	{
		double L;
		double a;
		double b;
	};
	struct
	{
		double x;
		double y;
		double z;
	};
	struct
	{
		double r;
		double g;
		double blue;
	};
	struct
	{
		double h;
		double s;
		double v;
	};
};

union Pixel
{
	unsigned int c;
	struct
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
	};

	Pixel() : c(0) {}
	Pixel(int v) : c(v) {}
};

static const Pixel empty;


static const double kWhiteReferenceX = 95.047f;
static const double kWhiteReferenceY = 100.f;
static const double kWhiteReferenceZ = 108.883f;

#if defined(_MSC_VER)
static const double kXyzEpsilon = double(216 / 24389.f);
static const double kXyzKappa = double(24389 / 27.f);
static const double kPi = double(3.14159265358979323846);

#define PivotRGB(n)		((n > 0.04045 ? pow((n + 0.055) / 1.055, 2.4) : n / 12.92) * 100.0)
#define PivotXYZ(n)		((n > kXyzEpsilon ? pow(n, 1 / 3.f) : (kXyzKappa * n + 16) / 116))
#define DegToRad(d)		((d * kPi / 180.f))
#define Distance(a, b)	((a - b) * (a - b))
#else
static constexpr double kXyzEpsilon = double(216 / 24389.f);
static constexpr double kXyzKappa = double(24389 / 27.f);
static constexpr double kPi = double(3.14159265358979323846);

constexpr double PivotRGB(double n) { return ((n > 0.04045 ? pow((n + 0.055) / 1.055, 2.4) : n / 12.92) * 100.0); }
constexpr double PivotXYZ(double n) { return ((n > kXyzEpsilon ? pow(n, 1 / 3.f) : (kXyzKappa * n + 16) / 116)); }
constexpr double DegToRad(double d) { return d * kPi / 180.f; }
constexpr double Distance(double a, double b) { return (a - b) * (a - b); }
#endif

inline PixelNormalized RGBtoHSV(Pixel p)
{
	PixelNormalized r, i;

	// pre-cast for comparissions
	i.r = double(p.r);
	i.g = double(p.g);
	i.b = double(p.b);

	auto max = double(std::max<unsigned char>(p.r, std::max<unsigned char>(p.g, p.b)));
	auto min = double(std::min<unsigned char>(p.r, std::min<unsigned char>(p.g, p.b)));

	r.v = max;
	auto delta = max - min;

	if (max > 0.0f)
	{
		r.s = delta / max * 255;
	}
	else
	{
		r.s = 0.0f;
		r.h = 0.0f;
		return r;
	}

	if		(i.r >= max) r.h =		 (i.g - i.b) / delta;
	else if (i.g >= max) r.h = 2.0 + (i.b - i.r) / delta;
	else				 r.h = 4.0 + (i.r - i.g) / delta;

	r.h *= 60.0;
	if (r.h < 0.0)
		r.h += 360.0;

	return r;
}

inline PixelNormalized RGBtoXYZ(Pixel p)
{
	auto r = PivotRGB(p.r / 255.f);
	auto g = PivotRGB(p.g / 255.f);
	auto b = PivotRGB(p.b / 255.f);

	PixelNormalized n;
	n.x = r * 0.412453f + g * 0.357580f + b * 0.180423f;
	n.y = r * 0.212671f + g * 0.715160f + b * 0.072169f;
	n.z = r * 0.019334f + g * 0.119193f + b * 0.950227f;

	return std::move(n);
}

inline PixelNormalized XYZtoLAB(PixelNormalized p)
{
	auto x = PivotXYZ(p.x / kWhiteReferenceX);
	auto y = PivotXYZ(p.y / kWhiteReferenceY);
	auto z = PivotXYZ(p.z / kWhiteReferenceZ);

	PixelNormalized n;
	n.L = std::max<double>(0.f, 116.f * y - 16.f);
	n.a = 500.f * (x - y);
	n.b = 200.f * (y - z);

	return std::move(n);
}

inline PixelNormalized RGBtoLAB(Pixel p)
{
	return XYZtoLAB(RGBtoXYZ(p));
}

// https://github.com/THEjoezack/ColorMine/blob/master/ColorMine/ColorSpaces/Comparisons/CieDe2000Comparison.cs
inline double ciede2000(Pixel p1, Pixel p2 = empty)
{
	//Set weighting factors to 1
	double k_L = 1.0f;
	double k_C = 1.0f;
	double k_H = 1.0f;

	//Change Color Space to L*a*b:
	auto lab1 = RGBtoLAB(p1);
	auto lab2 = RGBtoLAB(p2);

	//Calculate Cprime1, Cprime2, Cabbar
	double c_star_1_ab = sqrt(lab1.a * lab1.a + lab1.b * lab1.b);
	double c_star_2_ab = sqrt(lab2.a * lab2.a + lab2.b * lab2.b);
	double c_star_average_ab = (c_star_1_ab + c_star_2_ab) / 2.f;

	double c_star_average_ab_pot7 = c_star_average_ab * c_star_average_ab * c_star_average_ab;
	c_star_average_ab_pot7 *= c_star_average_ab_pot7 * c_star_average_ab;

	double G = 0.5f * (1 - sqrt(c_star_average_ab_pot7 / (c_star_average_ab_pot7 + 6103515625))); //25^7
	double a1_prime = (1 + G) * lab1.a;
	double a2_prime = (1 + G) * lab2.a;

	double C_prime_1 = sqrt(a1_prime * a1_prime + lab1.b * lab1.b);
	double C_prime_2 = sqrt(a2_prime * a2_prime + lab2.b * lab2.b);
	//Angles in Degree.
	double h_prime_1 = fmod(((atan2(lab1.b, a1_prime) * 180.f / kPi) + 360.f), 360.f);
	double h_prime_2 = fmod(((atan2(lab2.b, a2_prime) * 180.f / kPi) + 360.f), 360.f);

	double delta_L_prime = lab2.L - lab1.L;
	double delta_C_prime = C_prime_2 - C_prime_1;

	double h_bar = std::abs(double(h_prime_1 - h_prime_2));
	double delta_h_prime;

	if (C_prime_1 * C_prime_2 == 0)
		delta_h_prime = 0;
	else
	{
		if (h_bar <= 180.f)
		{
			delta_h_prime = h_prime_2 - h_prime_1;
		}
		else if (h_bar > 180.f && h_prime_2 <= h_prime_1)
		{
			delta_h_prime = h_prime_2 - h_prime_1 + 360.f;
		}
		else
		{
			delta_h_prime = h_prime_2 - h_prime_1 - 360.f;
		}
	}
	double delta_H_prime = 2 * sqrt(C_prime_1 * C_prime_2) * sin(delta_h_prime * kPi / 360.f);

	// Calculate CIEDE2000
	double L_prime_average = (lab1.L + lab2.L) / 2.f;
	double C_prime_average = (C_prime_1 + C_prime_2) / 2.f;

	//Calculate h_prime_average

	double h_prime_average;
	if (C_prime_1 * C_prime_2 == 0)
		h_prime_average = 0;
	else
	{
		if (h_bar <= 180.f)
		{
			h_prime_average = (h_prime_1 + h_prime_2) / 2.f;
		}
		else if (h_bar > 180.f && (h_prime_1 + h_prime_2) < 360.f)
		{
			h_prime_average = (h_prime_1 + h_prime_2 + 360.f) / 2.f;
		}
		else
		{
			h_prime_average = (h_prime_1 + h_prime_2 - 360.f) / 2.f;
		}
	}
	double L_prime_average_minus_50_square = (L_prime_average - 50);
	L_prime_average_minus_50_square *= L_prime_average_minus_50_square;

	double S_L = 1 + ((.015f * L_prime_average_minus_50_square) / sqrt(20 + L_prime_average_minus_50_square));
	double S_C = 1 + .045f * C_prime_average;
	double T = 1
		- .17f * cos(DegToRad(h_prime_average - 30))
		+ .24f * cos(DegToRad(h_prime_average * 2))
		+ .32f * cos(DegToRad(h_prime_average * 3 + 6))
		- .2f  * cos(DegToRad(h_prime_average * 4 - 63));

	double S_H = 1 + .015f * T * C_prime_average;
	double h_prime_average_minus_275_div_25_square = (h_prime_average - 275) / 25.f;
	h_prime_average_minus_275_div_25_square *= h_prime_average_minus_275_div_25_square;
	double delta_theta = 30.f * exp(-h_prime_average_minus_275_div_25_square);

	double C_prime_average_pot_7 = C_prime_average * C_prime_average * C_prime_average;
	C_prime_average_pot_7 *= C_prime_average_pot_7 * C_prime_average;
	double R_C = 2 * sqrt(C_prime_average_pot_7 / (C_prime_average_pot_7 + 6103515625));

	double R_T = -sin(DegToRad(2 * delta_theta)) * R_C;

	double delta_L_prime_div_k_L_S_L = delta_L_prime / (S_L * k_L);
	double delta_C_prime_div_k_C_S_C = delta_C_prime / (S_C * k_C);
	double delta_H_prime_div_k_H_S_H = delta_H_prime / (S_H * k_H);

	double CIEDE2000 = sqrt(
		  delta_L_prime_div_k_L_S_L * delta_L_prime_div_k_L_S_L
		+ delta_C_prime_div_k_C_S_C * delta_C_prime_div_k_C_S_C
		+ delta_H_prime_div_k_H_S_H * delta_H_prime_div_k_H_S_H
		+ R_T * delta_C_prime_div_k_C_S_C * delta_H_prime_div_k_H_S_H
		);

	return CIEDE2000;
}

inline double cie1976(Pixel p1, Pixel p2 = empty)
{
	//Change Color Space to L*a*b:
	auto a = RGBtoLAB(p1);
	auto b = RGBtoLAB(p2);

	auto differences = Distance(a.L, b.L) + Distance(a.a, b.a) + Distance(a.b, b.b);
	return differences;
	//return sqrt(differences);
}

// http://www.compuphase.com/cmetric.htm
inline double cmetric(Pixel p1, Pixel p2 = empty)
{
	long rmean = ((long)p1.r + (long)p2.r) / 2.f;
	long r = (long)p1.r - (long)p2.r;
	long g = (long)p1.g - (long)p2.g;
	long b = (long)p1.b - (long)p2.b;
	return (((512 + rmean) * r * r) >> 8) + (4 * g * g) + (((767 - rmean) * b * b) >> 8);
}

inline double rtm_distance(Pixel p1, Pixel p2 = empty)
{
	static double f[] = {0.114, 0.587, 0.299};
	static const int kWeight = 10;

	double x[3] = {p1.r * f[0], p1.g * f[1], p1.b * f[2]};
	double y[3] = {p2.r * f[0], p2.g * f[1], p2.b * f[2]};
	double t = 0;
	double lx = 0;
	double ly = 0;

	for (int i = 0; i < 3; ++i)
	{
		double xi = x[i];
		double yi = y[i];
		double d = xi - yi;

		t += d * d;
		lx += xi;
		ly += yi;
	}

	double l = lx - ly;
	return t + l * l * kWeight;
}

inline double hue_distance(Pixel p1, Pixel p2 = empty)
{
	auto h1 = RGBtoHSV(p1);
	auto h2 = RGBtoHSV(p2);
	return std::abs(double(h2.h - h1.h));
}

#endif // PIXEL_H
