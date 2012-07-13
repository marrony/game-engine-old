
#ifndef RASTERIZER_EE4F8987_BDA2_434d_A01F_BB3446E535C3
#define RASTERIZER_EE4F8987_BDA2_434d_A01F_BB3446E535C3

class Rasterizer {
public:
	// some constants
	static const int MAX_RENDER_TARGETS = 2;
	static const int MAX_VARYING = 8;
	static const int BLOCK_SIZE = 8;

public:
	// Type definitions
	struct Vertex {
		int x, y; // in 28.4 fixed point
		int z; // range from 0 to 0x7fffffff
		int w; // in 16.16 fixed point
		int varyings[MAX_VARYING];
	};

	// holds the pointers to each render target.
	// These will be passed to the fragment shader which then can write to
	// the pointed to location. Note: Only the first n pointers will be
	// valid where n is the current number of render targets
	struct BufferPointers {
		void* ptr[MAX_RENDER_TARGETS];
	};

	// This is the data the fragment shader gets
	struct FragmentData {
		int z;
		int varying[MAX_VARYING];
	};

	typedef FragmentData PixelBlock[BLOCK_SIZE][BLOCK_SIZE] ;

	class RenderTarget {
	public:
		virtual int width() = 0;
		virtual int height() = 0;
		virtual int stride() = 0;
		virtual void *buffer_pointer() = 0;
		virtual int element_size() = 0;
		virtual void clear(int x, int y, int w, int h) = 0;
	};

	class FragmentShader {
	public:
		// This provides a means for an early depth test.
		// x and y are the coordinates of the upper left corner of the current block.
		// If the shader somewhere stores the minimum z of each block that value
		// can be compared to the parameter z.
		// returns false when the depth test failed. In this case the whole block
		// can be culled.
		virtual bool early_depth_test(int x, int y, int z) { return true; }

		// This notifies the shader of any render target clears.
		// This is meant to be used in conjunction with the early depth test to update
		// any buffers used
		virtual void clear(int target, int x, int y, int w, int h) {}

		// To compute the mipmap level of detail one needs the derivativs in x and y of
		// the texture coordinates. These can be computed from the values in the pixel
		// block since all the fragment values have alredy been computed for this block
		// when this is called
		virtual void prepare_for_block(int x, int y, PixelBlock b) {}

		// This tells the rasterizer how many varyings this fragment shader needs
		virtual int  varying_count() = 0;

		// This is called once for each visible fragment inside the triangle
		// x and y are the coordinates within the block [0, BLOCK_SIZE[
		// the pixel block is indexed with p[y][x] !!!
		virtual void shade(const BufferPointers&, const PixelBlock& b, int x, int y) = 0;
	};

private:
	// Variables
	struct RenderTargetParams {
		int count;
		int minwidth, minheight;

		// cache these params to avoid too
		// many virtual function calls
		int stride[MAX_RENDER_TARGETS];
		int element_size[MAX_RENDER_TARGETS];
	} rendertarget_params_;

	RenderTarget *rendertargets_[MAX_RENDER_TARGETS];
	FragmentShader *fragment_shader_;

	struct {
		int x0, y0, x1, y1;
	} clip_rect_;

private:
	bool setup_valid();

public:
	// constructor
	Rasterizer();

public:
	// main interface

	// Set the render targets.
	// This resets the clipping rectangle
	void rendertargets(int n, RenderTarget* rt[]);

	// set the fragment shader
	void fragment_shader(FragmentShader *fs);

	void clear();
	void clear(int target);

	void clip_rect(int x, int y, int w, int h);

	// The triangle must be counter clockwise in screen space in order to be
	// drawn.
	void draw_triangle(const Vertex &v1, const Vertex &v2, const Vertex &v3);
};

#endif

