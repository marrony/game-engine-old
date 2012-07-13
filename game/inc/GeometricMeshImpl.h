/*
 * GeometricMeshImpl.h
 *
 *  Created on: 24/11/2010
 *      Author: marrony.neris
 */

#ifndef GEOMETRICMESHIMPL_H_
#define GEOMETRICMESHIMPL_H_

//class SphereImpl : extends engine::mesh::GeometricMesh {
//	engine::math::Vector3 createSphereBinormal(float theta, float phi) {
//		engine::math::Vector3 v;
//
//		v.x = cos(theta) * cos(phi);
//		v.z = sin(theta) * cos(phi);
//		v.y = -sin(phi);
//
//		return v;
//	}
//
//	engine::math::Vector3 createSphereTangent(float theta, float phi) {
//		engine::math::Vector3 v;
//
//		v.x = -sin(theta) * sin(phi);// + cos(theta) * cos(phi);
//		v.z = cos(theta) * sin(phi);// + sin(theta) * cos(phi);
//		v.y = 0.0;// + -sin(phi);
//
//		return v;
//	}
//
//	engine::math::Vector3 createSphereVertex(float theta, float phi) {
//		engine::math::Vector3 v;
//
//		v.x = cos(theta) * sin(phi);
//		v.z = sin(theta) * sin(phi);
//		v.y = cos(phi);
//
//		return v;
//	}
//
//	void insertFace(std::vector<unsigned int>& index, unsigned int lastVertexIndex) {
//		index.push_back(lastVertexIndex + 0);
//		index.push_back(lastVertexIndex + 2);
//		index.push_back(lastVertexIndex + 1);
//		index.push_back(lastVertexIndex + 0);
//		index.push_back(lastVertexIndex + 3);
//		index.push_back(lastVertexIndex + 2);
//	}
//
//	void buildGeometry(float size, int slicesVertical, int slicesHorizontal) {
//		std::vector<engine::math::Vector3> vertex;
//		std::vector<engine::math::Vector3> normal;
//		std::vector<engine::math::Vector3> tangent;
//		std::vector<engine::math::Vector3> binormal;
//		std::vector<engine::math::Vector3> color;
//		std::vector<engine::math::Vector2> texture;
//		std::vector<unsigned int> index;
//
//
//		const float pi = 3.1415926535897932384626433832795f;
//		const float pi2 = 2.0 * pi;
//
//		engine::math::Vector3 n, t, b, v, c;
//		engine::math::Vector2 uv;
//
//		for(int angy = 1; angy < slicesVertical; angy++) {
//			for(int angxz = 0; angxz < slicesHorizontal+1; angxz++) {
//				float theta = (pi2 / slicesHorizontal) * angxz;
//				float phi = (pi / slicesVertical) * angy;
//
//				n = createSphereVertex(theta, phi).normalize();
//				t = createSphereTangent(theta, phi).normalize();
//				b = engine::math::cross(n, t);
//				b = createSphereBinormal(theta, phi).normalize();
//
//				v.x = n.x * size;
//				v.z = n.z * size;
//				v.y = n.y * size;
//				uv.x = (angxz + 0) / (float)slicesHorizontal;
//				uv.y = (angy + 0) / (float)slicesVertical;
//
//				c.x = 1;
//				c.y = 0;
//				c.z = 0;
//
//				vertex.push_back(v);
//				normal.push_back(n);
//				tangent.push_back(t);
//				binormal.push_back(b);
//				texture.push_back(uv);
//				color.push_back(c);
//			}
//		}
//
//		for(int angy = 0; angy < slicesVertical-2; angy++) {
//			for(int angxz = 0; angxz < slicesHorizontal; angxz++) {
//				unsigned int i0 = (angy+0)*(slicesHorizontal+1) + (angxz+0);
//				unsigned int i1 = (angy+0)*(slicesHorizontal+1) + (angxz+1);
//				unsigned int i2 = (angy+1)*(slicesHorizontal+1) + (angxz+0);
//				unsigned int i3 = (angy+1)*(slicesHorizontal+1) + (angxz+1);
//
//				index.push_back(i0);
//				index.push_back(i2);
//				index.push_back(i1);
//
//				index.push_back(i1);
//				index.push_back(i2);
//				index.push_back(i3);
//			}
//		}
//
//		vertexBuffer = new engine::MemoryBuffer<engine::math::Vector3, 3>(vertex.data(), vertex.size());
//		normalBuffer = new engine::MemoryBuffer<engine::math::Vector3, 3>(normal.data(), normal.size());
//		tangentBuffer = new engine::MemoryBuffer<engine::math::Vector3, 3>(tangent.data(), tangent.size());
//		binormalBuffer = new engine::MemoryBuffer<engine::math::Vector3, 3>(binormal.data(), binormal.size());
//		colorBuffer = new engine::MemoryBuffer<engine::math::Vector3, 3>(color.data(), color.size());
//		textureBuffer = new engine::MemoryBuffer<engine::math::Vector2, 2>(texture.data(), texture.size());
//
//		indexBuffer.push_back(new engine::MemoryBuffer<unsigned int>(index.data(), index.size()));
//	}
//
//public:
//	SphereImpl(float size, int slicesVertical, int slicesHorizontal) {
//		buildGeometry(size, slicesVertical, slicesHorizontal);
//
//		updateBoundingBox();
//	}
//
//	static SphereImpl* createSphereFromSizeAndSlices(float size, int slicesVertical, int slicesHorizontal) {
//		return new SphereImpl(size, slicesVertical, slicesHorizontal);
//	}
//};
//
//class PlaneImpl : extends engine::mesh::GeometricMesh {
//
//	void insertFace(std::vector<unsigned int>& index, unsigned int lastVertexIndex) {
//		index.push_back(lastVertexIndex + 0);
//		index.push_back(lastVertexIndex + 2);
//		index.push_back(lastVertexIndex + 1);
//		index.push_back(lastVertexIndex + 0);
//		index.push_back(lastVertexIndex + 3);
//		index.push_back(lastVertexIndex + 2);
//	}
//
//	void buildGeometry(const engine::math::Vector3& dirVertical, const engine::math::Vector3& dirHorizontal, int numV, int numH, const engine::math::Vector3& ini) {
//		std::vector<engine::math::Vector3> vertex;
//		std::vector<engine::math::Vector3> normal;
//		std::vector<engine::math::Vector3> tangent;
//		std::vector<engine::math::Vector3> binormal;
//		std::vector<engine::math::Vector3> color;
//		std::vector<engine::math::Vector2> texture;
//		std::vector<unsigned int> index;
//
//		for(int u = 0; u < numV; u++) {
//			for(int v = 0; v < numH; v++) {
//				float u0 = (u+0) / (float)numV;
//				float u1 = (u+1) / (float)numV;
//
//				float v0 = (v+0) / (float)numH;
//				float v1 = (v+1) / (float)numH;
//
//				const engine::math::Vector2 tu(1, 0);
//				const engine::math::Vector2 tv(0, 1);
//
//				engine::math::Vector3 x0, x1, x2, x3;
//				engine::math::Vector2 t0, t1, t2, t3;
//				engine::math::Vector3 n, s, t;
//				engine::math::Vector3 c;
//
//				x0 = ini + (dirVertical * u0) + (dirHorizontal * v0);
//				t0 = (tu * u0) + (tv * v0);
//
//				x1 = ini + (dirVertical * u0) + (dirHorizontal * v1);
//				t1 = (tu * u0) + (tv * v1);
//
//				x2 = ini + (dirVertical * u1) + (dirHorizontal * v1);
//				t2 = (tu * u1) + (tv * v1);
//
//				x3 = ini + (dirVertical * u1) + (dirHorizontal * v0);
//				t3 = (tu * u1) + (tv * v0);
//
//				n = engine::math::cross(dirVertical, dirHorizontal).normalize(); //normal
//				s = engine::math::cross(dirHorizontal, n).normalize();           //tangent
//				t = engine::math::cross(n, s);                                   //binormal
//
//				c.x = 0;
//				c.y = 1;
//				c.z = 0;
//
//				unsigned int lastVertexIndex = vertex.size();
//
//				vertex.push_back(x0);
//				vertex.push_back(x1);
//				vertex.push_back(x2);
//				vertex.push_back(x3);
//				normal.push_back(n);
//				normal.push_back(n);
//				normal.push_back(n);
//				normal.push_back(n);
//				tangent.push_back(s);
//				tangent.push_back(s);
//				tangent.push_back(s);
//				tangent.push_back(s);
//				binormal.push_back(t);
//				binormal.push_back(t);
//				binormal.push_back(t);
//				binormal.push_back(t);
//				texture.push_back(t0);
//				texture.push_back(t1);
//				texture.push_back(t2);
//				texture.push_back(t3);
//				color.push_back(c);
//				color.push_back(c);
//				color.push_back(c);
//				color.push_back(c);
//
//				insertFace(index, lastVertexIndex);
//			}
//		}
//
//		vertexBuffer = new engine::MemoryBuffer<engine::math::Vector3, 3>(vertex.data(), vertex.size());
//		normalBuffer = new engine::MemoryBuffer<engine::math::Vector3, 3>(normal.data(), normal.size());
//		tangentBuffer = new engine::MemoryBuffer<engine::math::Vector3, 3>(tangent.data(), tangent.size());
//		binormalBuffer = new engine::MemoryBuffer<engine::math::Vector3, 3>(binormal.data(), binormal.size());
//		colorBuffer = new engine::MemoryBuffer<engine::math::Vector3, 3>(color.data(), color.size());
//		textureBuffer = new engine::MemoryBuffer<engine::math::Vector2, 2>(texture.data(), texture.size());
//
//		indexBuffer.push_back(new engine::MemoryBuffer<unsigned int>(index.data(), index.size()));
//	}
//public:
//	PlaneImpl(const engine::math::Vector3& dirVertical, const engine::math::Vector3& dirHorizontal, int numV, int numH, const engine::math::Vector3& ini = engine::math::Vector3(0, 0, 0)) {
//		buildGeometry(dirVertical, dirHorizontal, numV, numH, ini);
//
//		updateBoundingBox();
//	}
//};

#endif /* GEOMETRICMESHIMPL_H_ */
