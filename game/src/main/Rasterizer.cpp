#include "Rasterizer.h"

#include <cmath>
#include <cassert>
#include <algorithm>

#ifndef _MSC_VER
#include <stdint.h>
#else
#include "stdint.h"
#endif

#include <memory.h>

////////////////////////////////////////////////////////////////////////////////
// utility functions

namespace {
	inline int min(int a, int b, int c) {
		return std::min(std::min(a, b), c);
	}

	inline int max(int a, int b, int c) {
		return std::max(std::max(a, b), c);
	}

	inline void compute_plane(int v0x, int v0y, int v1x, int v1y, int v2x, int v2y, int z0, int z1, int z2, int64_t plane[4]) {
		const int px = v1x - v0x;
		const int py = v1y - v0y;
		const int pz = z1 - z0;

		const int qx = v2x - v0x;
		const int qy = v2y - v0y;
		const int qz = z2 - z0;

		/* Crossproduct "(a,b,c) := dv1 x dv2" is orthogonal to plane. */
		const int64_t a = (int64_t)py * qz - (int64_t)pz * qy;
		const int64_t b = (int64_t)pz * qx - (int64_t)px * qz;
		const int64_t c = (int64_t)px * qy - (int64_t)py * qx;
		/* Point on the plane = "r*(a,b,c) + w", with fixed "r" depending
		 on the distance of plane from origin and arbitrary "w" parallel
		 to the plane. */
		/* The scalar product "(r*(a,b,c)+w)*(a,b,c)" is "r*(a^2+b^2+c^2)",
		 which is equal to "-d" below. */
		const int64_t d = -(a * v0x + b * v0y + c * z0);

		plane[0] = a;
		plane[1] = b;
		plane[2] = c;
		plane[3] = d;
	}

	inline int solve_plane(int x, int y, const int64_t plane[4]) {
		assert(plane[2] != 0);
		return (int)((plane[3] + plane[0] * x + plane[1] * y) / -plane[2]);
	}

	template<int denominator>
	inline void floor_divmod(int numerator, int &floor, int &mod) {
		assert(denominator > 0);
		if(numerator >= 0) {
			// positive case, C is okay
			floor = numerator / denominator;
			mod = numerator % denominator;
		} else {
			// Numerator is negative, do the right thing
			floor = -((-numerator) / denominator);
			mod = (-numerator) % denominator;
			if(mod) {
				// there is a remainder
				floor--;
				mod = denominator - mod;
			}
		}
	}

	// Fixed point division
	template<int p>
	inline int32_t fixdiv(int32_t a, int32_t b) {
#if 0
		return (int32_t)((((int64_t)a) << p) / b);
#else
		// The following produces the same results as the above but gcc 4.0.3
		// generates fewer instructions (at least on the ARM processor).
		union {
			int64_t a;
			struct {
				int32_t l;
				int32_t h;
			};
		} x;

		x.l = a << p;
		x.h = a >> (sizeof(int32_t) * 8 - p);
		return (int32_t)(x.a / b);
#endif
	}

	// Perform a fixed point multiplication using a 64-bit intermediate result to
	// prevent overflow problems.
	template<int p>
	inline int32_t fixmul(int32_t a, int32_t b) {
		return (int32_t)(((int64_t)a * b) >> p);
	}
} // end anonymous namespace

////////////////////////////////////////////////////////////////////////////////

Rasterizer::Rasterizer() :
	fragment_shader_(0) {
	rendertarget_params_.count = 0;
}

bool Rasterizer::setup_valid() {
	return rendertarget_params_.count >= 1 && fragment_shader_ != 0;
}

void Rasterizer::clear() {
	for(int i = 0; i < rendertarget_params_.count; ++i)
		clear(i);
}

void Rasterizer::clear(int target) {
	assert(target <= rendertarget_params_.count);
	rendertargets_[target]->clear(0, 0, rendertarget_params_.minwidth, rendertarget_params_.minheight);

	// notify shader about clear (might want to update internal data structutes)
	if(fragment_shader_)
		fragment_shader_->clear(target, 0, 0, rendertarget_params_.minwidth, rendertarget_params_.minheight);
}

void Rasterizer::clip_rect(int x, int y, int w, int h) {
	if(rendertarget_params_.count == 0)
		return;

	clip_rect_.x0 = std::max(0, x);
	clip_rect_.y0 = std::max(0, y);
	clip_rect_.x1 = std::min(x + w, rendertarget_params_.minwidth);
	clip_rect_.y1 = std::min(y + h, rendertarget_params_.minheight);
}

////////////////////////////////////////////////////////////////////////////////
// main interface

void Rasterizer::rendertargets(int n, RenderTarget* rt[]) {
	assert(n <= MAX_RENDER_TARGETS);
	RenderTargetParams &rtp = rendertarget_params_;
	rtp.count = n;

	if(n == 0)
		return;

	rtp.minwidth = rt[0]->width();
	rtp.minheight = rt[0]->height();
	for(int i = 0; i < n; ++i) {
		rendertargets_[i] = rt[i];
		rtp.minwidth = std::min(rtp.minwidth, rt[i]->width());
		rtp.minheight = std::min(rtp.minheight, rt[i]->height());

		// cache these to avoid too many virtual function calls later
		rtp.element_size[i] = rt[i]->element_size();
		rtp.stride[i] = rt[i]->stride();
	}

	clip_rect_.x0 = 0;
	clip_rect_.y0 = 0;
	clip_rect_.x1 = rtp.minwidth;
	clip_rect_.y1 = rtp.minheight;
}

void Rasterizer::fragment_shader(FragmentShader *fs) {
	assert(fs != 0);
	fragment_shader_ = fs;
}

#if 1
void Rasterizer::draw_triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3) {
	if(!setup_valid())
	return;

	int64_t zPlane[4];
	int64_t wPlane[4];
	int64_t vPlane[MAX_VARYING][4];

	compute_plane(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y, v1.z, v2.z, v3.z, zPlane);
	compute_plane(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y, // interpolate 1/w across triangle
			fixdiv<16> (1 << 16, v1.w),
			fixdiv<16> (1 << 16, v2.w),
			fixdiv<16> (1 << 16, v3.w), wPlane);

	int varying_count = fragment_shader_->varying_count();
	for(int i = 0; i < varying_count; ++i) {
		compute_plane(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y,
				fixdiv<16> (v1.varyings[i], v1.w),
				fixdiv<16> (v2.varyings[i], v2.w),
				fixdiv<16> (v3.varyings[i], v3.w), vPlane[i]);
	}

	// Deltas
	const int DX12 = v1.x - v2.x;
	const int DX23 = v2.x - v3.x;
	const int DX31 = v3.x - v1.x;

	const int DY12 = v1.y - v2.y;
	const int DY23 = v2.y - v3.y;
	const int DY31 = v3.y - v1.y;

	// Fixed-point deltas
	const int FDX12 = DX12 << 4;
	const int FDX23 = DX23 << 4;
	const int FDX31 = DX31 << 4;

	const int FDY12 = DY12 << 4;
	const int FDY23 = DY23 << 4;
	const int FDY31 = DY31 << 4;

	// Bounding rectangle
	int minx = (min(v1.x, v2.x, v3.x) + 0xF) >> 4;
	int maxx = (max(v1.x, v2.x, v3.x) + 0xF) >> 4;
	int miny = (min(v1.y, v2.y, v3.y) + 0xF) >> 4;
	int maxy = (max(v1.y, v2.y, v3.y) + 0xF) >> 4;

	// consider clipping rectangle
	minx = std::max(minx, clip_rect_.x0);
	maxx = std::min(maxx, clip_rect_.x1);
	miny = std::max(miny, clip_rect_.y0);
	maxy = std::min(maxy, clip_rect_.y1);

	// Start in corner of 8x8 block
	minx &= ~(BLOCK_SIZE - 1);
	miny &= ~(BLOCK_SIZE - 1);

	BufferPointers buffers;

	for(int i = 0; i < rendertarget_params_.count; ++i)
	buffers.ptr[i] = (char*)rendertargets_[i]->buffer_pointer() + miny * rendertargets_[i]->stride();

	// Half-edge constants
	int C1 = DY12 * v1.x - DX12 * v1.y;
	int C2 = DY23 * v2.x - DX23 * v2.y;
	int C3 = DY31 * v3.x - DX31 * v3.y;

	// Correct for fill convention
	if(DY12 < 0 || (DY12 == 0 && DX12 > 0))
	C1++;
	if(DY23 < 0 || (DY23 == 0 && DX23 > 0))
	C2++;
	if(DY31 < 0 || (DY31 == 0 && DX31 > 0))
	C3++;

	// Loop through blocks
	for(int y = miny; y < maxy; y += BLOCK_SIZE) {
		for(int x = minx; x < maxx; x += BLOCK_SIZE) {
			// Corners of block
			int x0 = x << 4;
			int x1 = (x + BLOCK_SIZE - 1) << 4;
			int y0 = y << 4;
			int y1 = (y + BLOCK_SIZE - 1) << 4;

			// Evaluate half-space functions
			bool a00 = C1 + DX12 * y0 - DY12 * x0 > 0;
			bool a10 = C1 + DX12 * y0 - DY12 * x1 > 0;
			bool a01 = C1 + DX12 * y1 - DY12 * x0 > 0;
			bool a11 = C1 + DX12 * y1 - DY12 * x1 > 0;
			int a = (a00 << 0) | (a10 << 1) | (a01 << 2) | (a11 << 3);

			bool b00 = C2 + DX23 * y0 - DY23 * x0 > 0;
			bool b10 = C2 + DX23 * y0 - DY23 * x1 > 0;
			bool b01 = C2 + DX23 * y1 - DY23 * x0 > 0;
			bool b11 = C2 + DX23 * y1 - DY23 * x1 > 0;
			int b = (b00 << 0) | (b10 << 1) | (b01 << 2) | (b11 << 3);

			bool c00 = C3 + DX31 * y0 - DY31 * x0 > 0;
			bool c10 = C3 + DX31 * y0 - DY31 * x1 > 0;
			bool c01 = C3 + DX31 * y1 - DY31 * x0 > 0;
			bool c11 = C3 + DX31 * y1 - DY31 * x1 > 0;
			int c = (c00 << 0) | (c10 << 1) | (c01 << 2) | (c11 << 3);

			// Skip block when outside an edge
			if(a == 0x0 || b == 0x0 || c == 0x0)
			continue;

#define CLIP_TEST(X, Y) 				((X) >= clip_rect_.x0 && (X) < clip_rect_.x1 && 				(Y) >= clip_rect_.y0 && (Y) < clip_rect_.y1)

			// test for the clipping rectangle
			bool clip00 = CLIP_TEST(x, y);
			bool clip10 = CLIP_TEST(x + 7, y);
			bool clip01 = CLIP_TEST(x, y + 7);
			bool clip11 = CLIP_TEST(x + 7, y + 7);

			// skip block if all is clippled
			if(!clip00 && !clip10 && !clip01 && !clip11)
			continue;
			bool clip_all_in = clip00 && clip10 && clip01 && clip11;

			//! compute attribute interpolants at corners
			FragmentData f00;
			FragmentData f10;
			FragmentData f01;
			FragmentData f11;

			int xx1 = (x + BLOCK_SIZE) << 4;
			int yy1 = (y + BLOCK_SIZE) << 4;
			f00.z = solve_plane(x0, y0, zPlane);
			f10.z = solve_plane(xx1, y0, zPlane);
			f01.z = solve_plane(x0, yy1, zPlane);
			f11.z = solve_plane(xx1, yy1, zPlane);

			if(!fragment_shader_->early_depth_test(x, y, std::min(std::min(std::min(f00.z, f10.z), f01.z), f11.z)))
			continue;

			int w00 = fixdiv<16> (1 << 16, solve_plane(x0, y0, wPlane));
			int w10 = fixdiv<16> (1 << 16, solve_plane(xx1, y0, wPlane));
			int w01 = fixdiv<16> (1 << 16, solve_plane(x0, yy1, wPlane));
			int w11 = fixdiv<16> (1 << 16, solve_plane(xx1, yy1, wPlane));

			for(int i = 0; i < varying_count; ++i) {
				f00.varying[i] = fixmul<16> (solve_plane(x0, y0, vPlane[i]), w00);
				f10.varying[i] = fixmul<16> (solve_plane(xx1, y0, vPlane[i]), w10);
				f01.varying[i] = fixmul<16> (solve_plane(x0, yy1, vPlane[i]), w01);
				f11.varying[i] = fixmul<16> (solve_plane(xx1, yy1, vPlane[i]), w11);
			}

			//! compute attribute step y left and right
			struct varying_step_t {
				struct step_info_t {
					int step;
					int rem;
					int error_term;
					step_info_t() :
					error_term(0) {
					}

					int dostep() {
						int r = step;
						error_term += rem;
						if(error_term >= BLOCK_SIZE) {
							error_term -= BLOCK_SIZE;
							r++;
						}
						return r;
					}
				};

				step_info_t z;
				step_info_t varying[MAX_VARYING];

				varying_step_t(FragmentData& p1, FragmentData& p2, int vc) {
					floor_divmod<BLOCK_SIZE> (p2.z - p1.z, z.step, z.rem);
					for(int i = 0; i < vc; ++i) {
						floor_divmod<BLOCK_SIZE> (p2.varying[i] - p1.varying[i], varying[i].step, varying[i].rem);
					}
				}
			};

			varying_step_t step_left(f00, f01, varying_count);
			varying_step_t step_right(f10, f11, varying_count);

			BufferPointers block_buffers = buffers;

#define RENDER_TARGET_LOOP 				for (int i = 0; i < rendertarget_params_.count; ++i)

#define STEP_POINTERS_BY_ELEMENTSIZE(VAR, FACTOR) 				{ 					RENDER_TARGET_LOOP 					(char*&)VAR.ptr[i] += FACTOR * rendertarget_params_.element_size[i]; 				}

#define	STEP_POINTERS_BY_STRIDE(VAR) 				{ 					RENDER_TARGET_LOOP 					(char*&)VAR.ptr[i] += rendertarget_params_.stride[i]; 				}

#define STEP_FRAGMENTDATA(FDVAR, STEPVAR) 				{ 					FDVAR.z += STEPVAR.z.dostep(); 					for (int i = 0; i < varying_count; ++i) 						FDVAR.varying[i] += STEPVAR.varying[i].dostep(); 				}

			// only copy the neccessary varyings
#define EFFICIENT_COPY(SRC, DST) 				{ 					DST.z = SRC.z; 					for (int i = 0; i < varying_count; ++i) 						DST.varying[i] = SRC.varying[i]; 				}

#define BLOCK_BEGIN 				fragment_shader_->prepare_for_block(x, y, pixel_block); 				for (int iy = 0; iy < BLOCK_SIZE; ++iy) { 					BufferPointers inner = block_buffers; 					STEP_POINTERS_BY_ELEMENTSIZE(inner, x); 					for (int ix = 0; ix < BLOCK_SIZE; ++ix) {

#define BLOCK_END 						STEP_POINTERS_BY_ELEMENTSIZE(inner, 1); 					} 					STEP_POINTERS_BY_STRIDE(block_buffers); 				}

			PixelBlock pixel_block;

			bool skip_flag[BLOCK_SIZE][BLOCK_SIZE];
			memset(skip_flag, 0, sizeof(skip_flag));

			if(!clip_all_in) {
				for(int iy = 0; iy < BLOCK_SIZE; ++iy)
				for(int ix = 0; ix < BLOCK_SIZE; ++ix)
				if(!CLIP_TEST(ix + x, iy + y))
				skip_flag[iy][ix] = true;
			}

			// Accept whole block when totally covered
			if(a == 0xF && b == 0xF && c == 0xF) {
				// first compute all fragment data
				for(int iy = 0; iy < BLOCK_SIZE; iy++) {
					//! compute attribute step x for this scanline
					varying_step_t stepx(f00, f10, varying_count);
					FragmentData fragment_data = f00;

					for(int ix = 0; ix < BLOCK_SIZE; ix++) {
						EFFICIENT_COPY(fragment_data, pixel_block[iy][ix]);
						STEP_FRAGMENTDATA(fragment_data, stepx);
					}

					//! step left and right attrib y
					STEP_FRAGMENTDATA(f00, step_left);
					STEP_FRAGMENTDATA(f10, step_right);
				}

				//! fragment_shader_block (can now use derivatives of attributes)
				if(clip_all_in) {
					BLOCK_BEGIN
					fragment_shader_->shade(inner, pixel_block, ix, iy);
					BLOCK_END
				} else {
					BLOCK_BEGIN
					if(!skip_flag[iy][ix])
					fragment_shader_->shade(inner, pixel_block, ix, iy);
					BLOCK_END
				}
			} else {// Partially covered block
				int CY1 = C1 + DX12 * y0 - DY12 * x0;
				int CY2 = C2 + DX23 * y0 - DY23 * x0;
				int CY3 = C3 + DX31 * y0 - DY31 * x0;

				for(int iy = 0; iy < BLOCK_SIZE; iy++) {
					int CX1 = CY1;
					int CX2 = CY2;
					int CX3 = CY3;

					//! compute attribute step x for this scanline
					varying_step_t stepx(f00, f10, varying_count);

					FragmentData fragment_data = f00;

					for(int ix = 0; ix < BLOCK_SIZE; ix++) {
						if(!(CX1 > 0 && CX2 > 0 && CX3 > 0))
						skip_flag[iy][ix] = true;

						// we still need to do this since the fragment shader might want
						// to compute the derivative of attibutes
						EFFICIENT_COPY(fragment_data, pixel_block[iy][ix]);

						CX1 -= FDY12;
						CX2 -= FDY23;
						CX3 -= FDY31;

						STEP_FRAGMENTDATA(fragment_data, stepx);
					}

					CY1 += FDX12;
					CY2 += FDX23;
					CY3 += FDX31;

					//! step left and right attrib y
					STEP_FRAGMENTDATA(f00, step_left);
					STEP_FRAGMENTDATA(f10, step_right);
				}

				//! fragment_shader_block (can now use derivatives of attributes)
				BLOCK_BEGIN
				if(!skip_flag[iy][ix])
				fragment_shader_->shade(inner, pixel_block, ix, iy);
				BLOCK_END
			}
		}

		for(int i = 0; i < rendertarget_params_.count; ++i)
		(char*&)buffers.ptr[i] += BLOCK_SIZE * rendertargets_[i]->stride();

	}
}
#elif 0
void Rasterizer::draw_triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3) {
	float y1 = v1.y / 16.0f;
	float y2 = v2.y / 16.0f;
	float y3 = v3.y / 16.0f;

	float x1 = v1.x / 16.0f;
	float x2 = v2.x / 16.0f;
	float x3 = v3.x / 16.0f;

	// Bounding rectangle
	int minx = (int) min(x1, x2, x3);
	int maxx = (int) max(x1, x2, x3);
	int miny = (int) min(y1, y2, y3);
	int maxy = (int) max(y1, y2, y3);

	unsigned int* colorBuffer = (unsigned int*) rendertargets_[0]->buffer_pointer();

	(char*&) colorBuffer += miny * rendertargets_[0]->stride();

	// Scan through bounding rectangle
	for(int y = miny; y < maxy; y++) {
		for(int x = minx; x < maxx; x++) {
			// When all half-space functions positive, pixel is in triangle
			float a = (x1 - x2) * (y - y1) - (y1 - y2) * (x - x1);
			float b = (x2 - x3) * (y - y2) - (y2 - y3) * (x - x2);
			float c = (x3 - x1) * (y - y3) - (y3 - y1) * (x - x3);

			if(a > 0 && b > 0 && c > 0) {
				colorBuffer[x] = 0x00FFFFFF; // White
			}
		}

		(char*&) colorBuffer += rendertargets_[0]->stride();
	}
}
#elif 0
void Rasterizer::draw_triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3) {
	float y1 = v1.y / 16.0f;
	float y2 = v2.y / 16.0f;
	float y3 = v3.y / 16.0f;

	float x1 = v1.x / 16.0f;
	float x2 = v2.x / 16.0f;
	float x3 = v3.x / 16.0f;

	// Deltas
	float Dx12 = x1 - x2;
	float Dx23 = x2 - x3;
	float Dx31 = x3 - x1;

	float Dy12 = y1 - y2;
	float Dy23 = y2 - y3;
	float Dy31 = y3 - y1;

	// Bounding rectangle
	int minx = (int) min(x1, x2, x3);
	int maxx = (int) max(x1, x2, x3);
	int miny = (int) min(y1, y2, y3);
	int maxy = (int) max(y1, y2, y3);

	unsigned int* colorBuffer = (unsigned int*) rendertargets_[0]->buffer_pointer();

	(char*&) colorBuffer += miny * rendertargets_[0]->stride();

	// Constant part of half-edge functions
	float C1 = Dy12 * x1 - Dx12 * y1;
	float C2 = Dy23 * x2 - Dx23 * y2;
	float C3 = Dy31 * x3 - Dx31 * y3;

	float Cy1 = C1 + Dx12 * miny - Dy12 * minx;
	float Cy2 = C2 + Dx23 * miny - Dy23 * minx;
	float Cy3 = C3 + Dx31 * miny - Dy31 * minx;

	// Scan through bounding rectangle
	for(int y = miny; y < maxy; y++) {
		// Start value for horizontal scan
		float Cx1 = Cy1;
		float Cx2 = Cy2;
		float Cx3 = Cy3;

		for(int x = minx; x < maxx; x++) {
			if(Cx1 > 0 && Cx2 > 0 && Cx3 > 0) {
				colorBuffer[x] = 0x00FFFFFF;
			}

			Cx1 -= Dy12;
			Cx2 -= Dy23;
			Cx3 -= Dy31;
		}

		Cy1 += Dx12;
		Cy2 += Dx23;
		Cy3 += Dx31;

		(char*&) colorBuffer += rendertargets_[0]->stride();
	}
}
#elif 0
void Rasterizer::draw_triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3) {
	// 28.4 fixed-point coordinates
	const int Y1 = round(/*16.0f **/v1.y);
	const int Y2 = round(/*16.0f **/v2.y);
	const int Y3 = round(/*16.0f **/v3.y);

	const int X1 = round(/*16.0f **/v1.x);
	const int X2 = round(/*16.0f **/v2.x);
	const int X3 = round(/*16.0f **/v3.x);

	// Deltas
	const int DX12 = X1 - X2;
	const int DX23 = X2 - X3;
	const int DX31 = X3 - X1;

	const int DY12 = Y1 - Y2;
	const int DY23 = Y2 - Y3;
	const int DY31 = Y3 - Y1;

	// Fixed-point deltas
	const int FDX12 = DX12 << 4;
	const int FDX23 = DX23 << 4;
	const int FDX31 = DX31 << 4;

	const int FDY12 = DY12 << 4;
	const int FDY23 = DY23 << 4;
	const int FDY31 = DY31 << 4;

	// Bounding rectangle
	int minx = (min(X1, X2, X3) + 0xF) >> 4;
	int maxx = (max(X1, X2, X3) + 0xF) >> 4;
	int miny = (min(Y1, Y2, Y3) + 0xF) >> 4;
	int maxy = (max(Y1, Y2, Y3) + 0xF) >> 4;

	unsigned int* colorBuffer = (unsigned int*) rendertargets_[0]->buffer_pointer();

	(char*&) colorBuffer += miny * rendertargets_[0]->stride();

	// Half-edge constants
	int C1 = DY12 * X1 - DX12 * Y1;
	int C2 = DY23 * X2 - DX23 * Y2;
	int C3 = DY31 * X3 - DX31 * Y3;

	// Correct for fill convention
	C1 += (DY12 < 0 || (DY12 == 0 && DX12 > 0));
	C2 += (DY23 < 0 || (DY23 == 0 && DX23 > 0));
	C3 += (DY31 < 0 || (DY31 == 0 && DX31 > 0));

	int CY1 = C1 + DX12 * (miny << 4) - DY12 * (minx << 4);
	int CY2 = C2 + DX23 * (miny << 4) - DY23 * (minx << 4);
	int CY3 = C3 + DX31 * (miny << 4) - DY31 * (minx << 4);

	for(int y = miny; y < maxy; y++) {
		int CX1 = CY1;
		int CX2 = CY2;
		int CX3 = CY3;

		for(int x = minx; x < maxx; x++) {
			if(CX1 > 0 && CX2 > 0 && CX3 > 0) {
				colorBuffer[x] = 0x00FFFFFF;
			}

			CX1 -= FDY12;
			CX2 -= FDY23;
			CX3 -= FDY31;
		}

		CY1 += FDX12;
		CY2 += FDX23;
		CY3 += FDX31;

		(char*&) colorBuffer += rendertargets_[0]->stride();
	}
}
#else
void Rasterizer::draw_triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3) {

	const int W1 = fixdiv<16>(1 << 16, v1.w);
	const int W2 = fixdiv<16>(1 << 16, v2.w);
	const int W3 = fixdiv<16>(1 << 16, v3.w);

	int64_t planeW[4];
	compute_plane(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y, W1, W2, W3, planeW);

	int64_t planeV0[4];
	const int V1 = fixdiv<16>(0, v1.w);
	const int V2 = fixdiv<16>(100, v2.w);
	const int V3 = fixdiv<16>(200, v3.w);
	compute_plane(v1.x, v1.y, v2.x, v2.y, v3.x, v3.y, V1, V2, V3, planeV0);

	// 28.4 fixed-point coordinates
	const int Y1 = v1.y;
	const int Y2 = v2.y;
	const int Y3 = v3.y;

	const int X1 = v1.x;
	const int X2 = v2.x;
	const int X3 = v3.x;

	// Deltas
	const int DX12 = X1 - X2;
	const int DX23 = X2 - X3;
	const int DX31 = X3 - X1;

	const int DY12 = Y1 - Y2;
	const int DY23 = Y2 - Y3;
	const int DY31 = Y3 - Y1;

	// Fixed-point deltas
	const int FDX12 = DX12 << 4;
	const int FDX23 = DX23 << 4;
	const int FDX31 = DX31 << 4;

	const int FDY12 = DY12 << 4;
	const int FDY23 = DY23 << 4;
	const int FDY31 = DY31 << 4;

	// Bounding rectangle
	int minx = (min(X1, X2, X3) + 0xF) >> 4;
	int maxx = (max(X1, X2, X3) + 0xF) >> 4;
	int miny = (min(Y1, Y2, Y3) + 0xF) >> 4;
	int maxy = (max(Y1, Y2, Y3) + 0xF) >> 4;

	// Block size, standard 8x8 (must be power of two)
	const int BLOCK_SIZE = 8;

	// Start in corner of 8x8 block
	minx &= ~(BLOCK_SIZE - 1);
	miny &= ~(BLOCK_SIZE - 1);

	unsigned int* colorBuffer = (unsigned int*)rendertargets_[0]->buffer_pointer();

	(char*&)colorBuffer += miny * rendertargets_[0]->stride();

	// Half-edge constants
	int C1 = DY12 * X1 - DX12 * Y1;
	int C2 = DY23 * X2 - DX23 * Y2;
	int C3 = DY31 * X3 - DX31 * Y3;

	// Correct for fill convention
	C1 += (DY12 < 0 || (DY12 == 0 && DX12 > 0));
	C2 += (DY23 < 0 || (DY23 == 0 && DX23 > 0));
	C3 += (DY31 < 0 || (DY31 == 0 && DX31 > 0));

	// Loop through blocks
	for(int y = miny; y < maxy; y += BLOCK_SIZE) {
		for(int x = minx; x < maxx; x += BLOCK_SIZE) {

			// Corners of block
			int x0 = x << 4;
			int x1 = (x + BLOCK_SIZE - 1) << 4;
			int y0 = y << 4;
			int y1 = (y + BLOCK_SIZE - 1) << 4;

			// Evaluate half-space functions
			bool a00 = C1 + DX12 * y0 - DY12 * x0 > 0;
			bool a10 = C1 + DX12 * y0 - DY12 * x1 > 0;
			bool a01 = C1 + DX12 * y1 - DY12 * x0 > 0;
			bool a11 = C1 + DX12 * y1 - DY12 * x1 > 0;
			int a = (a00 << 0) | (a10 << 1) | (a01 << 2) | (a11 << 3);

			bool b00 = C2 + DX23 * y0 - DY23 * x0 > 0;
			bool b10 = C2 + DX23 * y0 - DY23 * x1 > 0;
			bool b01 = C2 + DX23 * y1 - DY23 * x0 > 0;
			bool b11 = C2 + DX23 * y1 - DY23 * x1 > 0;
			int b = (b00 << 0) | (b10 << 1) | (b01 << 2) | (b11 << 3);

			bool c00 = C3 + DX31 * y0 - DY31 * x0 > 0;
			bool c10 = C3 + DX31 * y0 - DY31 * x1 > 0;
			bool c01 = C3 + DX31 * y1 - DY31 * x0 > 0;
			bool c11 = C3 + DX31 * y1 - DY31 * x1 > 0;
			int c = (c00 << 0) | (c10 << 1) | (c01 << 2) | (c11 << 3);

			// Skip block when outside an edge
			if(a == 0x0 || b == 0x0 || c == 0x0) {
				continue;
			}

//			const int w00 = fixdiv<16>(1 << 16, solve_plane(x0, y0, planeW));
//			const int w10 = fixdiv<16>(1 << 16, solve_plane(x1, y0, planeW));
//			const int w01 = fixdiv<16>(1 << 16, solve_plane(x0, y1, planeW));
//			const int w11 = fixdiv<16>(1 << 16, solve_plane(x1, y1, planeW));
//
//			const int vv0 = fixmul<16>(solve_plane(x0, y0, planeV0), w00);
//			const int vv1 = fixmul<16>(solve_plane(x1, y0, planeV0), w10);
//			const int vv2 = fixmul<16>(solve_plane(x0, y1, planeV0), w01);
//			const int vv3 = fixmul<16>(solve_plane(x1, y1, planeV0), w11);

			unsigned int *buffer = colorBuffer;

			// Accept whole block when totally covered
			if(a == 0xF && b == 0xF && c == 0xF) {
				for(int iy = 0; iy < BLOCK_SIZE; iy++) {
					for(int ix = x; ix < x + BLOCK_SIZE; ix++) {

						const int w00 = fixdiv<16>(1 << 16, solve_plane(ix, iy+y, planeW));
						const int vv0 = fixmul<16>(solve_plane(ix, iy+y, planeV0), w00);

						buffer[ix] = 0x00007F00; // Green
					}

					(char*&)buffer += rendertargets_[0]->stride();
				}
			} else { // Partially covered block
				int CY1 = C1 + DX12 * y0 - DY12 * x0;
				int CY2 = C2 + DX23 * y0 - DY23 * x0;
				int CY3 = C3 + DX31 * y0 - DY31 * x0;

				for(int iy = y; iy < y + BLOCK_SIZE; iy++) {
					int CX1 = CY1;
					int CX2 = CY2;
					int CX3 = CY3;

					for(int ix = x; ix < x + BLOCK_SIZE; ix++) {
						if(CX1 > 0 && CX2 > 0 && CX3 > 0) {
							buffer[ix] = 0x0000007F; // Blue
						}

						CX1 -= FDY12;
						CX2 -= FDY23;
						CX3 -= FDY31;
					}

					CY1 += FDX12;
					CY2 += FDX23;
					CY3 += FDX31;

					(char*&)buffer += rendertargets_[0]->stride();
				}
			}
		}

		(char*&)colorBuffer += BLOCK_SIZE * rendertargets_[0]->stride();
	}
}
#endif
