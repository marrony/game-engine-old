#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
#include "Rasterizer2.h"

#include <iostream>

Edge::Edge(Real x1, Real y1, Real x2, Real y2, unsigned int _i0, unsigned int _i1,
		Real z1, Real z2) {
	if(y1 < y2) {
		X1 = x1;
		Y1 = y1;
		X2 = x2;
		Y2 = y2;
		i0 = _i0;
		i1 = _i1;
		Z1 = z1;
		Z2 = z2;
	} else {
		X1 = x2;
		Y1 = y2;
		X2 = x1;
		Y2 = y1;
		i0 = _i1;
		i1 = _i0;
		Z1 = z2;
		Z2 = z1;
	}
}

Span::Span(Real x1, Real z1, Real _factor1, const std::vector<vec4>* _diffs1, Real x2, Real z2, Real _factor2, const std::vector<vec4>* _diffs2) {
	if(x1 < x2) {
		factor1 = _factor1;
		X1 = x1;
		factor2 = _factor2;
		X2 = x2;
		diffs1 = _diffs1;
		diffs2 = _diffs2;
		Z1 = z1;
		Z2 = z2;
	} else {
		factor1 = _factor2;
		X1 = x2;
		factor2 = _factor1;
		X2 = x1;
		diffs1 = _diffs2;
		diffs2 = _diffs1;
		Z1 = z2;
		Z2 = z1;
	}
}

void Rasterizer2::setFrameBuffer(uint32_t *frameBuffer, unsigned int width, unsigned int height) {
	m_FrameBuffer = frameBuffer;
	m_Width = width;
	m_Height = height;

	if(depthBuffer) {
		delete[] depthBuffer;
	}

	depthBuffer = new Real[m_Width*m_Height];
}

void Rasterizer2::setPixel(unsigned int x, unsigned int y, const vec4 &color) {
	if(x >= m_Width || y >= m_Height)
		return;

	uint32_t r = (uint32_t) (color.x * 255.0f);
	uint32_t g = (uint32_t) (color.y * 255.0f);
	uint32_t b = (uint32_t) (color.z * 255.0f);
	uint32_t a = (uint32_t) (color.w * 255.0f);

	m_FrameBuffer[y * m_Width + x] = (a << 24) | (r << 16) | (g << 8) | b;
}

void Rasterizer2::clear() {
	memset(m_FrameBuffer, 0, sizeof(uint32_t) * m_Height * m_Width);

	for(unsigned int i = 0; i < m_Height * m_Width; i++) {
		depthBuffer[i] = Real(1.0);
	}
}

void calcPlane(Real x0, Real y0, Real x1, Real y1, Real x2, Real y2, Real z0, Real z1, Real z2, Real plane[4]) {
	Real v0x = x1 - x0;
	Real v0y = y1 - y0;
	Real v0z = z1 - z0;

	Real v1x = x2 - x0;
	Real v1y = y2 - y0;
	Real v1z = z2 - z0;

	/**
	 * |  i   j   k  |
	 * | v0x v0y v0z | = i*(v0y*v1z - v0z*v1y) - j*(v0x*v1z - v0z*v1x) + k*(v0x*v1y - v0y*v1x)
	 * | v1x v1y v1z |
	 */
	Real i = v0y*v1z - v0z*v1y;
	Real j = v0z*v1x - v0x*v1z;
	Real k = v0x*v1y - v0y*v1x;

	/**
	 * iX + jY + kZ + d = 0
	 * iX + jY + kZ = - d
	 */

	Real d = i*x0 + j*y0 + k*z0;

	plane[0] = i;
	plane[1] = j;
	plane[2] = k;
	plane[3] = -d;
}

Real solvePlane(Real x, Real y, const Real plane[4]) {
	/**
	 * iX + jY + kZ + d = 0
	 *
	 * iX + jY + d = -kZ
	 *
	 * (iX + jY + d) / -k = Z
	 */

	return (plane[0]*x + plane[1]*y + plane[3]) / -plane[2];
}

void Rasterizer2::drawTriangle(TriangleData* triangleData) {
	const vec4 v0 = triangleData->getVarying(triangleData->i0, 0);
	const vec4 v1 = triangleData->getVarying(triangleData->i1, 0);
	const vec4 v2 = triangleData->getVarying(triangleData->i2, 0);

	Real zPlane[4]	;

	calcPlane(v0.x, v0.y, v1.x, v1.y, v2.x, v2.y, v0.z, v1.z, v2.z, zPlane);

#define SUB_PIXEL_BITS 4

	Fixed y0 = Fixed(v0.y - 0.5, SUB_PIXEL_BITS);
	Fixed y1 = Fixed(v1.y - 0.5, SUB_PIXEL_BITS);
	Fixed y2 = Fixed(v2.y - 0.5, SUB_PIXEL_BITS);

	vec4 vmax;
	vec4 vmed;
	vec4 vmin;
	Fixed ymax, ymed, ymin;
	Fixed xmax, xmed, xmin;

	Real bf = 1.0;

	int qualIf = -1;

	/**
	 * A ordem dos vertices eh v0 -> v1 -> v2
	 */

	if(y0 >= y1) {
		if(y1 >= y2) {
			//y0 >= y1 >= y2

			vmax = v0; vmed = v1; vmin = v2;
			ymax = y0; ymed = y1; ymin = y2;

//			bf = -1.0;

			qualIf = 0;
		} else if(y2 >= y0) {
			//y2 >= y0 >= y1

			vmax = v2; vmed = v0; vmin = v1;
			ymax = y2; ymed = y0; ymin = y1;

//			bf = -1.0;

			qualIf = 1;
		} else {
			//y0 >= y2 >= y1

			vmax = v0; vmed = v2; vmin = v1;
			ymax = y0; ymed = y2; ymin = y1;

			bf = -1.0;

			qualIf = 2;
		}
	} else {
		if(y0 >= y2) {
			//y1 >= y0 >= y2

			vmax = v1; vmed = v0; vmin = v2;
			ymax = y1; ymed = y0; ymin = y2;

			bf = -1.0;

			qualIf = 3;
		} else if(y2 >= y1) {
			//y2 >= y1 >= y0

			vmax = v2; vmed = v1; vmin = v0;
			ymax = y2; ymed = y1; ymin = y0;

			bf = -1.0;

			qualIf = 4;
		} else {
			//y1 >= y2 >= y0

			vmax = v1; vmed = v2; vmin = v0;
			ymax = y1; ymed = y2; ymin = y0;

//			bf = -1.0;

			qualIf = 5;
		}
	}

	xmax = Fixed(vmax.x + 0.5, SUB_PIXEL_BITS);
	xmed = Fixed(vmed.x + 0.5, SUB_PIXEL_BITS);
	xmin = Fixed(vmin.x + 0.5, SUB_PIXEL_BITS);

	Edge2 emax, etop, ebottom;

	emax.yini = Fixed(ceil(ymin));
	emax.linhas = ceil(ymax - emax.yini);
	emax.adjy = Real(emax.yini - ymin);
	emax.dx = Real(xmax - xmin);
	emax.dy = Real(ymax - ymin);
	emax.dxdy = emax.dx / emax.dy;
	emax.fdxdy = Fixed(emax.dxdy);
	emax.x = xmin;
	emax.xini = emax.x + Fixed(emax.adjy * emax.dxdy);

	if(emax.linhas <= 0) {
		return;
	}

	etop.yini = Fixed(ceil(ymed));
	etop.linhas = ceil(ymax - etop.yini);
	etop.adjy = Real(etop.yini - ymed);
	etop.dx = Real(xmax - xmed);
	etop.dy = Real(ymax - ymed);
	etop.dxdy = etop.dx / etop.dy;
	etop.fdxdy = Fixed(etop.dxdy);
	etop.x = xmed;
	etop.xini = etop.x + Fixed(etop.adjy * etop.dxdy);

	ebottom.yini = Fixed(ceil(ymin));
	ebottom.linhas = ceil(ymed - ebottom.yini);
	ebottom.adjy = Real(ebottom.yini - ymin);
	ebottom.dx = Real(xmed - xmin);
	ebottom.dy = Real(ymed - ymin);
	ebottom.dxdy = ebottom.dx / ebottom.dy;
	ebottom.fdxdy = Fixed(ebottom.dxdy);
	ebottom.x = xmin;
	ebottom.xini = ebottom.x + Fixed(ebottom.adjy * ebottom.dxdy);

	Real area = emax.dx*ebottom.dy - emax.dy*ebottom.dx;

	/* TODO
	if(std::isinf(area) || std::isnan(area) || area*area <= 0.00005) {
		return;
	}*/

	std::cout << "area: " << area << std::endl;
	std::cout << "qual if: " << qualIf << std::endl;
	std::cout << "bf: " << bf << std::endl;
	std::cout << "ymax: " << (float)ymax << " ymed: " << (float)ymed << " ymin: " << (float)ymin << std::endl;

	std::cout << vmax.x << ", " << vmax.y << ", " << vmax.z << std::endl;
	std::cout << vmed.x << ", " << vmed.y << ", " << vmed.z << std::endl;
	std::cout << vmin.x << ", " << vmin.y << ", " << vmin.z << std::endl;

	vec3 vvmax = vec3(vmax.x, vmax.y, vmax.z);
	vec3 vvmed = vec3(vmed.x, vmed.y, vmed.z);
	vec3 vvmin = vec3(vmin.x, vmin.y, vmin.z);
	vec3 cross = engine::math::cross(vvmax - vvmed, vvmin - vvmed);
	double dist = engine::math::dot(cross, vvmax);

	std::cout << "dist: " << dist << std::endl;

	Real cull = -1.0;
	if(area * bf * cull < 0) {
		std::cout << "face culled" << std::endl;
		return;
	}

	if(area * bf < 0) {
		std::cout << "facing: -1" << std::endl;
	} else {
		std::cout << "facing: +1" << std::endl;
	}

	std::cout << std::endl;

	bool esqDir = area < 0;

	drawSpan(&emax, &ebottom, esqDir, 0, zPlane);
	double c = 0.5;
	color = color +  2.0f*vec4(c, c, c, c);
	drawSpan(&emax, &etop, esqDir, 1, zPlane);
}

void Rasterizer2::drawSpan(Edge2* e0, Edge2* e1, bool esqDir, int triNum, Real zPlane[4]) {
	Edge2* left;
	Edge2* right;

	if(esqDir) {
		left = e0;
		right = e1;
		color = vec4(1,1,0,1);
	} else {
		left = e1;
		right = e0;
		color = vec4(1,0,1,1);
	}

	Fixed error = Fixed(ceil(left->xini)) - left->xini - Fixed::ONE;

	for(int i = 0; i < e1->linhas; i++) {

		int y = i + (int)e1->yini;

		if(y >= 0 && y < (int)m_Height) {
			int xleft = std::max((int)left->xini, 0);
			int xright = std::min((int)right->xini, (int)m_Width);

			for(int x = xleft; x < xright; x++) {

				Real z = solvePlane(x + 0.5, y + 0.5, zPlane);

				if(z >= 0 && z < depthBuffer[y * m_Width + x]) {
					depthBuffer[y * m_Width + x] = z;

					Real x0 = y*left->dxdy + (Real)left->x;
					Real x1 = y*right->dxdy + (Real)right->x;

					Real x0error = x0 - (Real)left->xini;
					Real x1error = x1 - (Real)right->xini;

//					std::cout << z << " ";

					setPixel(x, y, color);
//					setPixel(x, y, vec4(1,1,1,1));
				}
			}
		}

		left->xini += left->fdxdy;
		right->xini += right->fdxdy;
	}
}

//void Rasterizer2::DrawLine(const Color &color1, float x1, float y1, const Color &color2, float x2, float y2) {
//	float xdiff = (x2 - x1);
//	float ydiff = (y2 - y1);
//
//	if(xdiff == 0.0f && ydiff == 0.0f) {
//		SetPixel(x1, y1, color1);
//		return;
//	}
//
//	if(fabs(xdiff) > fabs(ydiff)) {
//		float xmin, xmax;
//
//		// set xmin to the lower x value given
//		// and xmax to the higher value
//		if(x1 < x2) {
//			xmin = x1;
//			xmax = x2;
//		} else {
//			xmin = x2;
//			xmax = x1;
//		}
//
//		// draw line in terms of y slope
//		float slope = ydiff / xdiff;
//		for(float x = xmin; x <= xmax; x += 1.0f) {
//			float y = y1 + ((x - x1) * slope);
//			Color color = color1 + ((color2 - color1) * ((x - x1) / xdiff));
//			SetPixel(x, y, color);
//		}
//	} else {
//		float ymin, ymax;
//
//		// set ymin to the lower y value given
//		// and ymax to the higher value
//		if(y1 < y2) {
//			ymin = y1;
//			ymax = y2;
//		} else {
//			ymin = y2;
//			ymax = y1;
//		}
//
//		// draw line in terms of x slope
//		float slope = xdiff / ydiff;
//		for(float y = ymin; y <= ymax; y += 1.0f) {
//			float x = x1 + ((y - y1) * slope);
//			Color color = color1 + ((color2 - color1) * ((y - y1) / ydiff));
//			SetPixel(x, y, color);
//		}
//	}
//}
