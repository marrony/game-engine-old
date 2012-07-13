///////////////////////////////////////////////////////////////////////
// title:          MilkShape 3D Model Viewer Sample
//
// copyright:      The programs and associated files contained in this
//                 distribution were developed by Mete Ciragan.  The
//                 programs are not in the public domain, but they are
//                 freely distributable without licensing fees.  These
//                 programs are provided without guarantee or warrantee
//                 expressed or implied.  Use at your own risk!
//
// email:          mciragan@gmx.net
// web:            http://www.milkshape3d.com
///////////////////////////////////////////////////////////////////////
#include "MS3DModel.h"
#include "mathlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MS3DModel::MS3DModel() {
	Clear();
}

MS3DModel::~MS3DModel() {
	Clear();
}

bool MS3DModel::Load(std::istream& file) {
	Clear();

	char id[10];
	file.read((char*)id, sizeof(id));
	if(strncmp(id, "MS3D000000", 10) != 0) {
		return false;
	}

	int version;
	file.read((char*)&version, sizeof(int));
	if(version != 4) {
		return false;
	}

	int i, j;

	// vertices
	unsigned short numVertices;
	file.read((char*)&numVertices, sizeof(unsigned short));
	m_vertices.resize(numVertices);
	for(i = 0; i < numVertices; i++) {
		file.read((char*)&m_vertices[i].flags, sizeof(unsigned char));
		file.read((char*)&m_vertices[i].vertex, sizeof(float) * 3);
		file.read((char*)&m_vertices[i].boneId, sizeof(char));
		file.read((char*)&m_vertices[i].referenceCount, sizeof(unsigned char));
	}

	// triangles
	unsigned short numTriangles;
	file.read((char*)&numTriangles, sizeof(unsigned short));
	m_triangles.resize(numTriangles);
	for(i = 0; i < numTriangles; i++) {
		file.read((char*)&m_triangles[i].flags, sizeof(unsigned short));
		file.read((char*)m_triangles[i].vertexIndices, sizeof(unsigned short) * 3);
		file.read((char*)m_triangles[i].vertexNormals, sizeof(float) * 3 * 3);
		file.read((char*)m_triangles[i].s, sizeof(float) * 3);
		file.read((char*)m_triangles[i].t, sizeof(float) * 3);
		file.read((char*)&m_triangles[i].smoothingGroup, sizeof(unsigned char));
		file.read((char*)&m_triangles[i].groupIndex, sizeof(unsigned char));

		// TODO: calculate triangle normal
	}

	// groups
	unsigned short numGroups;
	file.read((char*)&numGroups, sizeof(unsigned short));
	m_groups.resize(numGroups);
	for(i = 0; i < numGroups; i++) {
		file.read((char*)&m_groups[i].flags, sizeof(unsigned char));
		file.read((char*)m_groups[i].name, sizeof(char) * 32);

		unsigned short numGroupTriangles;
		file.read((char*)&numGroupTriangles, sizeof(unsigned short));
		m_groups[i].triangleIndices.resize(numGroupTriangles);
		if(numGroupTriangles > 0)
			file.read((char*)&m_groups[i].triangleIndices[0], sizeof(unsigned short) * numGroupTriangles);

		file.read((char*)&m_groups[i].materialIndex, sizeof(char));
	}

	// materials
	unsigned short numMaterials;
	file.read((char*)&numMaterials, sizeof(unsigned short));
	m_materials.resize(numMaterials);
	for(i = 0; i < numMaterials; i++) {
		file.read((char*)m_materials[i].name, sizeof(char) * 32);
		file.read((char*)&m_materials[i].ambient, sizeof(float) * 4);
		file.read((char*)&m_materials[i].diffuse, sizeof(float) * 4);
		file.read((char*)&m_materials[i].specular, sizeof(float) * 4);
		file.read((char*)&m_materials[i].emissive, sizeof(float) * 4);
		file.read((char*)&m_materials[i].shininess, sizeof(float));
		file.read((char*)&m_materials[i].transparency, sizeof(float));
		file.read((char*)&m_materials[i].mode, sizeof(unsigned char));
		file.read((char*)m_materials[i].texture, sizeof(char) * MAX_TEXTURE_FILENAME_SIZE);
		file.read((char*)m_materials[i].alphamap, sizeof(char) * MAX_TEXTURE_FILENAME_SIZE);

		// set alpha
		m_materials[i].ambient[3] = m_materials[i].transparency;
		m_materials[i].diffuse[3] = m_materials[i].transparency;
		m_materials[i].specular[3] = m_materials[i].transparency;
		m_materials[i].emissive[3] = m_materials[i].transparency;
	}

	// animation
	file.read((char*)&m_animationFps, sizeof(float));
	if(m_animationFps < 1.0f)
		m_animationFps = 1.0f;
	file.read((char*)&m_currentTime, sizeof(float));
	file.read((char*)&m_totalFrames, sizeof(int));

	// joints
	unsigned short numJoints;
	file.read((char*)&numJoints, sizeof(unsigned short));
	m_joints.resize(numJoints);
	for(i = 0; i < numJoints; i++) {
		file.read((char*)&m_joints[i].flags, sizeof(unsigned char));
		file.read((char*)m_joints[i].name, sizeof(char) * 32);
		file.read((char*)m_joints[i].parentName, sizeof(char) * 32);
		file.read((char*)m_joints[i].rot, sizeof(float) * 3);
		file.read((char*)m_joints[i].pos, sizeof(float) * 3);

		unsigned short numKeyFramesRot;
		file.read((char*)&numKeyFramesRot, sizeof(unsigned short));
		m_joints[i].rotationKeys.resize(numKeyFramesRot);

		unsigned short numKeyFramesPos;
		file.read((char*)&numKeyFramesPos, sizeof(unsigned short));
		m_joints[i].positionKeys.resize(numKeyFramesPos);

		// the frame time is in seconds, so multiply it by the animation fps, to get the frames
		// rotation channel
		for(j = 0; j < numKeyFramesRot; j++) {
			float key[3];
			file.read((char*)&m_joints[i].rotationKeys[j].time, sizeof(float));
			file.read((char*)key, sizeof(float) * 3);

			AngleQuaternion(key, m_joints[i].rotationKeys[j].key);

			m_joints[i].rotationKeys[j].time *= m_animationFps;
		}

		// translation channel
		for(j = 0; j < numKeyFramesPos; j++) {
			file.read((char*)&m_joints[i].positionKeys[j].time, sizeof(float));
			file.read((char*)&m_joints[i].positionKeys[j].key, sizeof(float) * 3);
			m_joints[i].positionKeys[j].time *= m_animationFps;
		}
	}

	// comments
	if(!file.eof()) {
		int subVersion = 0;
		file.read((char*)&subVersion, sizeof(int));
		if(subVersion == 1) {
			int numComments = 0;
			size_t commentSize = 0;

			// group comments
			file.read((char*)&numComments, sizeof(int));
			for(i = 0; i < numComments; i++) {
				int index;
				file.read((char*)&index, sizeof(int));
				std::vector<char> comment;
				file.read((char*)&commentSize, sizeof(size_t));
				comment.resize(commentSize);
				if(commentSize > 0)
					file.read((char*)&comment[0], sizeof(char) * commentSize);
				if(index >= 0 && index < (int) m_groups.size())
					m_groups[index].comment.append(comment.begin(), comment.end());
			}

			// material comments
			file.read((char*)&numComments, sizeof(int));
			for(i = 0; i < numComments; i++) {
				int index;
				file.read((char*)&index, sizeof(int));
				std::vector<char> comment;
				file.read((char*)&commentSize, sizeof(size_t));
				comment.resize(commentSize);
				if(commentSize > 0)
					file.read((char*)&comment[0], sizeof(char) * commentSize);
				if(index >= 0 && index < (int) m_materials.size())
					m_materials[index].comment.append(comment.begin(), comment.end());
			}

			// joint comments
			file.read((char*)&numComments, sizeof(int));
			for(i = 0; i < numComments; i++) {
				int index;
				file.read((char*)&index, sizeof(int));

				std::vector<char> comment;
				file.read((char*)&commentSize, sizeof(size_t));

				comment.resize(commentSize);

				if(commentSize > 0)
					file.read((char*)&comment[0], sizeof(char) * commentSize);

				if(index >= 0 && index < (int) m_joints.size())
					m_joints[index].comment.append(comment.begin(), comment.end());
			}

			// model comments
			file.read((char*)&numComments, sizeof(int));
			if(numComments == 1) {
				std::vector<char> comment;
				file.read((char*)&commentSize, sizeof(size_t));

				comment.resize(commentSize);

				if(commentSize > 0)
					file.read((char*)&comment[0], sizeof(char) * commentSize);

				m_comment.append(comment.begin(), comment.end());
			}
		} else {
			// "Unknown subversion for comments %d\n", subVersion);
		}
	}

	// vertex extra
	if(!file.eof()) {
		int subVersion = 0;
		file.read((char*)&subVersion, sizeof(int));
		if(subVersion == 2) {
			for(int i = 0; i < numVertices; i++) {
				file.read((char*)&m_vertices[i].boneIds[0], sizeof(char) * 3);
				file.read((char*)&m_vertices[i].weights[0], sizeof(unsigned char) * 3);
				file.read((char*)&m_vertices[i].extra, sizeof(unsigned int));
			}
		} else if(subVersion == 1) {
			for(int i = 0; i < numVertices; i++) {
				file.read((char*)&m_vertices[i].boneIds[0], sizeof(char) * 3);
				file.read((char*)&m_vertices[i].weights[0], sizeof(unsigned char) * 3);
			}
		} else {
			// "Unknown subversion for vertex extra %d\n", subVersion);
		}
	}

	// joint extra
	if(!file.eof()) {
		int subVersion = 0;
		file.read((char*)&subVersion, sizeof(int));
		if(subVersion == 1) {
			for(int i = 0; i < numJoints; i++) {
				file.read((char*)&m_joints[i].color, sizeof(float) * 3);
			}
		} else {
			// "Unknown subversion for joint extra %d\n", subVersion);
		}
	}

	// model extra
	if(!file.eof()) {
		int subVersion = 0;
		file.read((char*)&subVersion, sizeof(int));
		if(subVersion == 1) {
			file.read((char*)&m_jointSize, sizeof(float));
			file.read((char*)&m_transparencyMode, sizeof(int));
			file.read((char*)&m_alphaRef, sizeof(float));
		} else {
			//"Unknown subversion for model extra %d\n", subVersion);
		}
	}

	return true;
}

void MS3DModel::Clear() {
	m_vertices.clear();
	m_triangles.clear();
	m_groups.clear();
	m_materials.clear();
	m_animationFps = 24.0f;
	m_currentTime = 1.0f;
	m_totalFrames = 30;
	m_joints.clear();
	m_comment.clear();
	m_jointSize = 1.0f;
	m_transparencyMode = TRANSPARENCY_MODE_SIMPLE;
	m_alphaRef = 0.5f;
}

int MS3DModel::GetNumGroups() const {
	return (int) m_groups.size();
}

ms3d_group_t *MS3DModel::GetGroup(int index) {
	return &m_groups[index];
}

int MS3DModel::GetNumTriangles() const {
	return (int) m_triangles.size();
}

ms3d_triangle_t *MS3DModel::GetTriangle(int index) {
	return &m_triangles[index];
}

int MS3DModel::GetNumVertices() const {
	return (int) m_vertices.size();
}

ms3d_vertex_t *MS3DModel::GetVertex(int index) {
	return &m_vertices[index];
}

int MS3DModel::GetNumMaterials() const {
	return (int) m_materials.size();
}

ms3d_material_t *MS3DModel::GetMaterial(int index) {
	return &m_materials[index];
}

int MS3DModel::GetNumJoints() const {
	return (int) m_joints.size();
}

ms3d_joint_t *MS3DModel::GetJoint(int index) {
	return &m_joints[index];
}

float MS3DModel::GetJointSize() const {
	return m_jointSize;
}

int MS3DModel::GetTransparencyMode() const {
	return m_transparencyMode;
}

float MS3DModel::GetAlphaRef() const {
	return m_alphaRef;
}

int MS3DModel::FindJointByName(const char *name) {
	for(size_t i = 0; i < m_joints.size(); i++) {
		if(strcmp(m_joints[i].name, name) == 0)
			return i;
	}

	return -1;
}

void MS3DModel::SetupJoints() {
	for(size_t i = 0; i < m_joints.size(); i++) {
		ms3d_joint_t *joint = &m_joints[i];
		joint->parentIndex = FindJointByName(joint->parentName);
	}

	for(size_t i = 0; i < m_joints.size(); i++) {
		ms3d_joint_t *joint = &m_joints[i];
		AngleMatrix(joint->rot, joint->matLocalSkeleton);
		joint->matLocalSkeleton[0][3] = joint->pos[0];
		joint->matLocalSkeleton[1][3] = joint->pos[1];
		joint->matLocalSkeleton[2][3] = joint->pos[2];

		if(joint->parentIndex == -1) {
			memcpy(joint->matGlobalSkeleton, joint->matLocalSkeleton, sizeof(joint->matGlobalSkeleton));
		} else {
			ms3d_joint_t *parentJoint = &m_joints[joint->parentIndex];
			R_ConcatTransforms(parentJoint->matGlobalSkeleton, joint->matLocalSkeleton, joint->matGlobalSkeleton);
		}
	}

	SetupTangents();
}

void MS3DModel::SetupTangents() {
	for(size_t j = 0; j < m_joints.size(); j++) {
		ms3d_joint_t *joint = &m_joints[j];
		int numPositionKeys = (int) joint->positionKeys.size();
		joint->tangents.resize(numPositionKeys);

		// clear all tangents (zero derivatives)
		for(int k = 0; k < numPositionKeys; k++) {
			joint->tangents[k].tangentIn[0] = 0.0f;
			joint->tangents[k].tangentIn[1] = 0.0f;
			joint->tangents[k].tangentIn[2] = 0.0f;
			joint->tangents[k].tangentOut[0] = 0.0f;
			joint->tangents[k].tangentOut[1] = 0.0f;
			joint->tangents[k].tangentOut[2] = 0.0f;
		}

		// if there are more than 2 keys, we can calculate tangents, otherwise we use zero derivatives
		if(numPositionKeys > 2) {
			for(int k = 0; k < numPositionKeys; k++) {
				// make the curve tangents looped
				int k0 = k - 1;
				if(k0 < 0)
					k0 = numPositionKeys - 1;
				int k1 = k;
				int k2 = k + 1;
				if(k2 >= numPositionKeys)
					k2 = 0;

				// calculate the tangent, which is the vector from key[k - 1] to key[k + 1]
				float tangent[3];
				tangent[0] = (joint->positionKeys[k2].key[0] - joint->positionKeys[k0].key[0]);
				tangent[1] = (joint->positionKeys[k2].key[1] - joint->positionKeys[k0].key[1]);
				tangent[2] = (joint->positionKeys[k2].key[2] - joint->positionKeys[k0].key[2]);

				// weight the incoming and outgoing tangent by their time to avoid changes in speed, if the keys are not within the same interval
				float dt1 = joint->positionKeys[k1].time - joint->positionKeys[k0].time;
				float dt2 = joint->positionKeys[k2].time - joint->positionKeys[k1].time;
				float dt = dt1 + dt2;
				joint->tangents[k1].tangentIn[0] = tangent[0] * dt1 / dt;
				joint->tangents[k1].tangentIn[1] = tangent[1] * dt1 / dt;
				joint->tangents[k1].tangentIn[2] = tangent[2] * dt1 / dt;

				joint->tangents[k1].tangentOut[0] = tangent[0] * dt2 / dt;
				joint->tangents[k1].tangentOut[1] = tangent[1] * dt2 / dt;
				joint->tangents[k1].tangentOut[2] = tangent[2] * dt2 / dt;
			}
		}
	}
}

void MS3DModel::SetFrame(float frame) {
	if(frame < 0.0f) {
		for(size_t i = 0; i < m_joints.size(); i++) {
			ms3d_joint_t *joint = &m_joints[i];
			memcpy(joint->matLocal, joint->matLocalSkeleton, sizeof(joint->matLocal));
			memcpy(joint->matGlobal, joint->matGlobalSkeleton, sizeof(joint->matGlobal));
		}
	} else {
		for(size_t i = 0; i < m_joints.size(); i++) {
			EvaluateJoint(i, frame);
		}
	}

	m_currentTime = frame;
}

void MS3DModel::EvaluateJoint(int index, float frame) {
	ms3d_joint_t *joint = &m_joints[index];

	//
	// calculate joint animation matrix, this matrix will animate matLocalSkeleton
	//
	vec3_t pos = { 0.0f, 0.0f, 0.0f };
	int numPositionKeys = (int) joint->positionKeys.size();
	if(numPositionKeys > 0) {
		int i1 = -1;
		int i2 = -1;

		// find the two keys, where "frame" is in between for the position channel
		for(int i = 0; i < (numPositionKeys - 1); i++) {
			if(frame >= joint->positionKeys[i].time && frame < joint->positionKeys[i + 1].time) {
				i1 = i;
				i2 = i + 1;
				break;
			}
		}

		// if there are no such keys
		if(i1 == -1 || i2 == -1) {
			// either take the first
			if(frame < joint->positionKeys[0].time) {
				pos[0] = joint->positionKeys[0].key[0];
				pos[1] = joint->positionKeys[0].key[1];
				pos[2] = joint->positionKeys[0].key[2];
			}

			// or the last key
			else if(frame >= joint->positionKeys[numPositionKeys - 1].time) {
				pos[0] = joint->positionKeys[numPositionKeys - 1].key[0];
				pos[1] = joint->positionKeys[numPositionKeys - 1].key[1];
				pos[2] = joint->positionKeys[numPositionKeys - 1].key[2];
			}
		}

		// there are such keys, so interpolate using hermite interpolation
		else {
			ms3d_keyframe_position_t *p0 = &joint->positionKeys[i1];
			ms3d_keyframe_position_t *p1 = &joint->positionKeys[i2];
			ms3d_tangent_t *m0 = &joint->tangents[i1];
			ms3d_tangent_t *m1 = &joint->tangents[i2];

			// normalize the time between the keys into [0..1]
			float t = (frame - joint->positionKeys[i1].time)
					/ (joint->positionKeys[i2].time - joint->positionKeys[i1].time);
			float t2 = t * t;
			float t3 = t2 * t;

			// calculate hermite basis
			float h1 = 2.0f * t3 - 3.0f * t2 + 1.0f;
			float h2 = -2.0f * t3 + 3.0f * t2;
			float h3 = t3 - 2.0f * t2 + t;
			float h4 = t3 - t2;

			// do hermite interpolation
			pos[0] = h1 * p0->key[0] + h3 * m0->tangentOut[0] + h2 * p1->key[0] + h4 * m1->tangentIn[0];
			pos[1] = h1 * p0->key[1] + h3 * m0->tangentOut[1] + h2 * p1->key[1] + h4 * m1->tangentIn[1];
			pos[2] = h1 * p0->key[2] + h3 * m0->tangentOut[2] + h2 * p1->key[2] + h4 * m1->tangentIn[2];
		}
	}

	vec4_t quat = { 0.0f, 0.0f, 0.0f, 1.0f };
	int numRotationKeys = (int) joint->rotationKeys.size();
	if(numRotationKeys > 0) {
		int i1 = -1;
		int i2 = -1;

		// find the two keys, where "frame" is in between for the rotation channel
		for(int i = 0; i < (numRotationKeys - 1); i++) {
			if(frame >= joint->rotationKeys[i].time && frame < joint->rotationKeys[i + 1].time) {
				i1 = i;
				i2 = i + 1;
				break;
			}
		}

		// if there are no such keys
		if(i1 == -1 || i2 == -1) {
			// either take the first key
			if(frame < joint->rotationKeys[0].time) {
				memcpy(quat, joint->rotationKeys[0].key, sizeof(quat));
			}

			// or the last key
			else if(frame >= joint->rotationKeys[numRotationKeys - 1].time) {
				memcpy(quat, joint->rotationKeys[numRotationKeys - 1].key, sizeof(quat));
			}
		}

		// there are such keys, so do the quaternion slerp interpolation
		else {
			float t = (frame - joint->rotationKeys[i1].time)
					/ (joint->rotationKeys[i2].time - joint->rotationKeys[i1].time);
			QuaternionSlerp(joint->rotationKeys[i1].key, joint->rotationKeys[i2].key, t, quat);
		}
	}

	// make a matrix from pos/quat
	float matAnimate[3][4];
	QuaternionMatrix(quat, matAnimate);
	matAnimate[0][3] = pos[0];
	matAnimate[1][3] = pos[1];
	matAnimate[2][3] = pos[2];

	// animate the local joint matrix using: matLocal = matLocalSkeleton * matAnimate
	R_ConcatTransforms(joint->matLocalSkeleton, matAnimate, joint->matLocal);

	// build up the hierarchy if joints
	// matGlobal = matGlobal(parent) * matLocal
	if(joint->parentIndex == -1) {
		memcpy(joint->matGlobal, joint->matLocal, sizeof(joint->matGlobal));
	} else {
		ms3d_joint_t *parentJoint = &m_joints[joint->parentIndex];
		R_ConcatTransforms(parentJoint->matGlobal, joint->matLocal, joint->matGlobal);
	}
}

float MS3DModel::GetAnimationFps() const {
	return m_animationFps;
}

float MS3DModel::GetCurrentFrame() const {
	return m_currentTime;
}

int MS3DModel::GetTotalFrames() const {
	return m_totalFrames;
}

void MS3DModel::TransformVertex(const ms3d_vertex_t *vertex, vec3_t out) const {
	int jointIndices[4], jointWeights[4];
	FillJointIndicesAndWeights(vertex, jointIndices, jointWeights);

	if(jointIndices[0] < 0 || jointIndices[0] >= (int) m_joints.size() || m_currentTime < 0.0f) {
		out[0] = vertex->vertex[0];
		out[1] = vertex->vertex[1];
		out[2] = vertex->vertex[2];
	} else {
		// count valid weights
		int numWeights = 0;
		for(int i = 0; i < 4; i++) {
			if(jointWeights[i] > 0 && jointIndices[i] >= 0 && jointIndices[i] < (int) m_joints.size())
				++numWeights;
			else
				break;
		}

		// init
		out[0] = 0.0f;
		out[1] = 0.0f;
		out[2] = 0.0f;

		float weights[4] = { (float) jointWeights[0] / 100.0f, (float) jointWeights[1] / 100.0f, (float) jointWeights[2]
				/ 100.0f, (float) jointWeights[3] / 100.0f };
		if(numWeights == 0) {
			numWeights = 1;
			weights[0] = 1.0f;
		}
		// add weighted vertices
		for(int i = 0; i < numWeights; i++) {
			const ms3d_joint_t *joint = &m_joints[jointIndices[i]];
			vec3_t tmp, vert;
			VectorITransform(vertex->vertex, joint->matGlobalSkeleton, tmp);
			VectorTransform(tmp, joint->matGlobal, vert);

			out[0] += vert[0] * weights[i];
			out[1] += vert[1] * weights[i];
			out[2] += vert[2] * weights[i];
		}
	}
}

void MS3DModel::TransformNormal(const ms3d_vertex_t *vertex, const vec3_t normal, vec3_t out) const {
	int jointIndices[4], jointWeights[4];
	FillJointIndicesAndWeights(vertex, jointIndices, jointWeights);

	if(jointIndices[0] < 0 || jointIndices[0] >= (int) m_joints.size() || m_currentTime < 0.0f) {
		out[0] = normal[0];
		out[1] = normal[1];
		out[2] = normal[2];
	} else {
		// count valid weights
		int numWeights = 0;
		for(int i = 0; i < 4; i++) {
			if(jointWeights[i] > 0 && jointIndices[i] >= 0 && jointIndices[i] < (int) m_joints.size())
				++numWeights;
			else
				break;
		}

		// init
		out[0] = 0.0f;
		out[1] = 0.0f;
		out[2] = 0.0f;

		float weights[4] = { (float) jointWeights[0] / 100.0f, (float) jointWeights[1] / 100.0f, (float) jointWeights[2]
				/ 100.0f, (float) jointWeights[3] / 100.0f };
		if(numWeights == 0) {
			numWeights = 1;
			weights[0] = 1.0f;
		}
		// add weighted vertices
		for(int i = 0; i < numWeights; i++) {
			const ms3d_joint_t *joint = &m_joints[jointIndices[i]];
			vec3_t tmp, norm;
			VectorIRotate(normal, joint->matGlobalSkeleton, tmp);
			VectorRotate(tmp, joint->matGlobal, norm);

			out[0] += norm[0] * weights[i];
			out[1] += norm[1] * weights[i];
			out[2] += norm[2] * weights[i];
		}
	}
}

void MS3DModel::FillJointIndicesAndWeights(const ms3d_vertex_t *vertex, int jointIndices[4], int jointWeights[4]) const {
	jointIndices[0] = vertex->boneId;
	jointIndices[1] = vertex->boneIds[0];
	jointIndices[2] = vertex->boneIds[1];
	jointIndices[3] = vertex->boneIds[2];
	jointWeights[0] = 100;
	jointWeights[1] = 0;
	jointWeights[2] = 0;
	jointWeights[3] = 0;

	if(vertex->weights[0] != 0 || vertex->weights[1] != 0 || vertex->weights[2] != 0) {
		jointWeights[0] = vertex->weights[0];
		jointWeights[1] = vertex->weights[1];
		jointWeights[2] = vertex->weights[2];
		jointWeights[3] = 100 - (vertex->weights[0] + vertex->weights[1] + vertex->weights[2]);
	}
}
