#ifndef RASTERIZER_H_
#define RASTERIZER_H_

#include <vector>
#include <string>
#include <map>
#include <set>
#include "math/Vector.h"
#include "math/Matrix.h"
#include "math/fixed.h"

typedef engine::math::Vector2 vec2;
typedef engine::math::Vector3 vec3;
typedef engine::math::Vector4 vec4;

typedef engine::math::Matrix3 mat3;
typedef engine::math::Matrix4 mat4;

struct VertexPointer {
	struct Pointer {
		unsigned int mode;
		unsigned int stride;
		void* ptr;
	};

	std::map<std::string, Pointer> pointers;
};

class VaryingPointer {
	unsigned int varyingCount;
	std::vector<std::vector<vec4> > varyings;
public:
	VaryingPointer(unsigned int vertexCount) :
		varyingCount(0), varyings(vertexCount + 1) {
	}

	void setVarying(unsigned int varyingIndex, unsigned int varyingNumber, const vec4& v) {
		varyingCount = std::max(varyingCount, varyingNumber + 1);

		varyings[varyingIndex].resize(varyingCount);
		varyings[varyingIndex][varyingNumber] = v;
	}

	vec4 getVarying(unsigned int varyingIndex, unsigned int varyingNumber) {
		return varyings[varyingIndex][varyingNumber];
	}

	unsigned int count() const {
		return varyingCount;
	}
};

class VertexData {
	VertexPointer* vertexPointer;
	VaryingPointer* varyingPointer;

	unsigned int vertexIndex;
	unsigned int varyingIndex;
public:
	VertexData(VertexPointer* _vertexPointer, VaryingPointer* _varyingPointer, unsigned int _vertexIndex, unsigned int _varyingIndex) :
		vertexPointer(_vertexPointer), varyingPointer(_varyingPointer), vertexIndex(_vertexIndex), varyingIndex(_varyingIndex) {
	}

	vec4 attrib(const std::string& attrib) const {
		VertexPointer::Pointer pointer = vertexPointer->pointers[attrib];

		unsigned char* ptr = (unsigned char*) pointer.ptr;

		float v[4] = {0, 0, 0, 1};

		float* ptr2 = (float*)(ptr + (vertexIndex * pointer.stride));

		for(unsigned int i = 0; i < pointer.mode; i++) {
			v[i] = ptr2[i];
		}

		return vec4(v[0], v[1], v[2], v[3]);
	}

	void varying(unsigned int varyingNumber, const vec4& v) {
		varyingPointer->setVarying(varyingIndex, varyingNumber, v);
	}
};

class FragmentData {
	const std::vector<vec4>& varyingData;
public:
	vec4 color;

	FragmentData(const std::vector<vec4>& _varying) :
		varyingData(_varying) {
	}

	vec4 varying(unsigned int i) const {
		return varyingData[i];
	}

	void fragColor(const vec4& _color) {
		color = _color;
	}

	void fragData(unsigned int buffer, const vec4& color) {

	}

	void fragDepth(double z) {
	}
};

class VertexProgram {
public:
	virtual ~VertexProgram() { }
	virtual void process(VertexData& data) = 0;
};

class FragmentProgram {
public:
	virtual ~FragmentProgram() { }
	virtual void process(FragmentData& data) = 0;
};

struct TriangleData {
	unsigned int vertexCount;
	VaryingPointer* varyingPointer;
	unsigned int i0;
	unsigned int i1;
	unsigned int i2;

	vec4 getVarying(unsigned int vertexIndex, unsigned int varying) const {
		return varyingPointer->getVarying(vertexIndex, varying);
	}

	unsigned int varyingCount() const {
		return varyingPointer->count();
	}
};

typedef float Real;
typedef float1616 Fixed;
//typedef fixedNbits<11> Fixed;

class Edge {
public:
	Real X1, Y1, X2, Y2;
	Real Z1, Z2;
	unsigned int i0;
	unsigned int i1;

	Edge(Real x1, Real y1, Real x2, Real y2, unsigned int _i0, unsigned int _i1,
			Real z1, Real z2);
};

class Span {
public:
	Real X1, X2;
	Real Z1, Z2;
	Real factor1, factor2;
	const std::vector<vec4>* diffs1;
	const std::vector<vec4>* diffs2;

	Span(Real x1, Real z1, Real _factor1, const std::vector<vec4>* _diffs1, Real x2, Real z2, Real _factor2,
	      const std::vector<vec4>* _diffs2);
};

class Rasterizer2 {
protected:
	VertexProgram* vertexProgram;
	FragmentProgram* fragmentProgram;
	VertexPointer vertexPointer;

	uint32_t *m_FrameBuffer;
	Real* depthBuffer;
	unsigned int m_Width, m_Height;

	vec4 color;

	struct Edge2 {
		Fixed yini;
		int linhas;
		Real adjy;
		Real dx;
		Real dy;
		Real dxdy;
		Fixed fdxdy;
		Fixed x;
		Fixed xini;
	};

	void drawSpan(Edge2* e0, Edge2* e1, bool esqDir, int triNum, Real zPlane[4]);
	void drawTriangle(TriangleData* triangleData);
public:
	void setFrameBuffer(uint32_t *frameBuffer, unsigned int width, unsigned int height);
	void setPixel(unsigned int x, unsigned int y, const vec4 &color);
	void clear();

	//		void DrawLine(const Color &color1, float x1, float y1, const Color &color2, float x2, float y2);

	Rasterizer2(VertexProgram* vProgram, FragmentProgram* fProgram) {
		vertexProgram = vProgram;
		fragmentProgram = fProgram;
		depthBuffer = 0;
	}

	~Rasterizer2() {
		delete[] depthBuffer;
	}

	void vertexAttrib(const std::string& attrib, unsigned int mode, unsigned int stride, void* ptr) {
		VertexPointer::Pointer pointer;

		pointer.mode = mode;
		pointer.stride = stride;
		pointer.ptr = ptr;

		vertexPointer.pointers[attrib] = pointer;
	}

	void vertexStage(unsigned int* indices, unsigned int vertexCount, VaryingPointer* varyingPointer) {
		for(unsigned int i = 0; i < vertexCount; i++) {
			unsigned int index = indices[i];

			VertexData vertexData(&vertexPointer, varyingPointer, index, i);
			vertexProgram->process(vertexData);

			vec4 position = varyingPointer->getVarying(i, 0);

			position.w = 1.0 / position.w;
			position.x = position.x * position.w;
			position.y = position.y * position.w;
			position.z = position.z * position.w;

			position.x = (position.x + 1.0) * m_Width * 0.5 + 0.0;
			position.y = (position.y + 1.0) * m_Height * 0.5 + 0.0;
			position.z = (position.z * 0.5 + 0.5);

//			position.x = position.x*m_Width + position.w*0.0;
//			position.y = position.y*m_Height + position.w*0.0;

			varyingPointer->setVarying(i, 0, position);
		}
	}

	void drawTriangles(unsigned int* indices, unsigned int vertexCount) {
		VaryingPointer varyingPointer(vertexCount);

		vertexStage(indices, vertexCount,&varyingPointer);

		TriangleData triangleData;
		triangleData.vertexCount = vertexCount;
		triangleData.varyingPointer = &varyingPointer;

		double c = 1.0 / 254.0;
		color = vec4(c, c, c, c)*2.0f;

		for(unsigned int i = 0; i < vertexCount; i += 3) {
			triangleData.i0 = i + 0;
			triangleData.i1 = i + 1;
			triangleData.i2 = i + 2;

			color = vec4(0.4, 0.4, 0.4, 0.4);
			drawTriangle(&triangleData);

			color = color +  2.0f*vec4(c, c, c, c);
			break;
		}
	}
};

#endif /* RASTERIZER_H_ */
