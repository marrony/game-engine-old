/*
 * CreateScene.cpp
 *
 *  Created on: 22/05/2012
 *      Author: marrony.neris
 */

#include "CreateScene.h"
#include "ColladaInstanceNode.h"
#include "ColladaInstanceGeometry.h"
#include "ColladaInstanceMaterial.h"
#include "ColladaInstanceEffect.h"
#include "ColladaBindMaterial.h"
#include "ColladaArray.h"
#include "CreateGeometry.h"
#include "Material.h"

ResourceId CreateScene::addResource(Resource* resource) {
	resources.push_back(resource);
	return compiler->addResource(loader, resource);
}

void CreateScene::pushNode(Node* node) {
	nodes.push(node);
	matrix.push(node->getTransformation());
}

void CreateScene::popNode() {
	math::Vector3 translation, scale;
	math::Quaternion orientation;

	matrix.top().decompose(orientation, translation, scale);
	nodes.top()->setOrientation(orientation);
	nodes.top()->setPosition(translation);
	nodes.top()->setScale(scale);

	nodes.top();
	matrix.top();
}

void CreateScene::concatNode(Node* node) {
	nodes.top()->addChild(node);
}

void CreateScene::concatMatrix(math::Matrix4 m) {
	matrix.top() = matrix.top() * m;
}

CreateScene::CreateScene(ResourceCompiler* compiler, ResourceLoader* loader, ColladaDocument* document) :
		compiler(compiler), loader(loader), document(document) {
}

void CreateScene::visit(ColladaScene* scene) {
	ColladaInstanceVisualScene* instanceVisualScene = scene->getInstanceVisualScene();

	ColladaVisualScene* visualScene = document->findVisualScene(instanceVisualScene->getUrl());

	if(!visualScene)
		return;

	product = new Scene(visualScene->getId());
	addResource(product);

	pushNode(product->getRoot());

	for(ColladaNode* node : visualScene->getNodes()) {
		if(!node->getInstanceControllers().empty()) //TODO animation
			continue;

		node->accept(this);
	}

	popNode();
}

void CreateScene::visit(ColladaNode* colladaNode) {
	Node* newNode = product->createNode();

	concatNode(newNode);

	pushNode(newNode);

	for(ColladaTransformation* transformation : colladaNode->getTransformations())
		transformation->accept(this);

	for(ColladaNode* child : colladaNode->getNodes())
		child->accept(this);

	for(ColladaInstanceNode* instanceNode : colladaNode->getInstanceNodes()) {
		ColladaNode* node = document->findNode(instanceNode->getUrl());

		if(node)
			node->accept(this);
	}

	for(ColladaInstanceGeometry* instanceGeometry : colladaNode->getInstanceGeometries())
		instanceGeometry->accept(this);

	popNode();
}

void CreateScene::visit(ColladaInstanceGeometry* instanceGeometry) {
	materials.clear();

	for(ColladaInstanceMaterial* instanceMaterial : instanceGeometry->getBindMaterial()->getTechniqueCommon()->getInstanceMaterial()) {
		const std::string& target = instanceMaterial->getTarget();
		ColladaMaterial* material = document->findMaterial(instanceMaterial->getTarget());

		material->accept(this);
	}

	ColladaGeometry* geometry = document->findGeometry(instanceGeometry->getUrl());

	CreateGeometry createGeometry("");
	geometry->accept(&createGeometry);

	Model* model = createGeometry.getModel();

	ResourceId modelId = compiler->findResource(model->getName().c_str(), Model::TYPE);
	if(modelId == ResourceId(-1)) {
		modelId = addResource(model);
		product->createGeometry(modelId, model->getBoundingBox(), nodes.top());
	}
}

void CreateScene::visit(ColladaGeometry* geometry) {
	std::string name;

	if(!geometry->getId().empty())
		name = geometry->getId();
	else
		name = geometry->getName();

	model = new Model(name);

	ColladaMesh* mesh = geometry->getMesh();

	mesh->accept(this);

	model->geometry->calculateTangent();
	model->geometry->calculateBoundingBox();
}

void CreateScene::visit(ColladaMesh* mesh) {
	for(ColladaPolyList* polylist : mesh->getPolylists())
		polylist->accept(this);

	for(ColladaTriangles* triangle : mesh->getTriangles())
		triangle->accept(this);
}

struct VertexSoup {
	size_t icount;
	const std::vector<int>& primitive;
	std::vector<MeshVertex> vertices;
	std::vector<unsigned short> indices;
	ColladaInput* vertexInput;
	ColladaInput* normalInput;
	ColladaInput* texCoordInput;
	ColladaElement* vertex;
	ColladaSource* position;
	ColladaSource* normal;
	ColladaSource* texCoord;
	int inputs;
	int flags;

	VertexSoup(ColladaGeometricPrimitive* g, const std::vector<int>& primitive) : primitive(primitive) {
		icount = 0;

		vertexInput = g->findInputWithSemantic("VERTEX");
		normalInput = g->findInputWithSemantic("NORMAL");
		texCoordInput = g->findInputWithSemantic("TEXCOORD");

		inputs = -1;
		flags = MeshVertex::POSITION;

		position = vertexInput->getSourceElement();
		inputs = std::max(inputs, vertexInput->getOffset());

		if(normalInput) {
			normal = normalInput->getSourceElement();
			inputs = std::max(inputs, normalInput->getOffset());
			flags |= MeshVertex::NORMAL;
		}

		if(texCoordInput) {
			texCoord = texCoordInput->getSourceElement();
			inputs = std::max(inputs, texCoordInput->getOffset());
			flags |= MeshVertex::TEXTURE;
		}

		inputs++;
	}

	int addVertex(const MeshVertex& vertex) {
		std::vector<MeshVertex>::iterator ite = std::find_if(vertices.begin(), vertices.end(), [&](const MeshVertex& other) {
			bool equals = (vertex.position == other.position);

			if(flags & MeshVertex::NORMAL)
				equals &= (normalInput && vertex.normal == other.normal);

			if(flags & MeshVertex::TEXTURE)
				equals &= (texCoordInput && vertex.texCoord == other.texCoord);

			return equals;
		});

		int index;

		if(ite == vertices.end()) {
			index = vertices.size();
			vertices.push_back(vertex);
		} else {
			index = std::distance(vertices.begin(), ite);
		}

		return index;
	}

	bool hasVertex() {
		return (icount+inputs-1) < primitive.size();
	}

	MeshVertex nextVertex() {
		MeshVertex vertex;

		int v = primitive[icount + vertexInput->getOffset()];
		vertex.position.x = position->getFloatArray()->getArray()[v*3 + 0];
		vertex.position.y = position->getFloatArray()->getArray()[v*3 + 1];
		vertex.position.z = position->getFloatArray()->getArray()[v*3 + 2];

		if(normalInput) {
			int n = primitive[icount + normalInput->getOffset()];
			vertex.normal.x = normal->getFloatArray()->getArray()[n*3 + 0];
			vertex.normal.y = normal->getFloatArray()->getArray()[n*3 + 1];
			vertex.normal.z = normal->getFloatArray()->getArray()[n*3 + 2];
		}

		if(texCoordInput) {
			int t = primitive[icount + texCoordInput->getOffset()];
			vertex.texCoord.x = texCoord->getFloatArray()->getArray()[t*2 + 0];
			vertex.texCoord.y = texCoord->getFloatArray()->getArray()[t*2 + 1];
		}

		icount += inputs;

		return vertex;
	}
};

void CreateScene::visit(ColladaPolyList* polylist) {
	for(const std::vector<int>& p : polylist->getPrimitive()) {
		VertexSoup vertexSoup(polylist, p);

		for(int vc : polylist->getVcount()) {
			int index[3];

			if(!vertexSoup.hasVertex()) break;
			index[0] = vertexSoup.addVertex(vertexSoup.nextVertex());

			if(!vertexSoup.hasVertex()) break;
			index[1] = vertexSoup.addVertex(vertexSoup.nextVertex());

			if(!vertexSoup.hasVertex()) break;
			index[2] = vertexSoup.addVertex(vertexSoup.nextVertex());

			vertexSoup.indices.insert(vertexSoup.indices.end(), index, index + 3);

			for(int i = 3; i < vc; i++) {
				index[1] = index[2];

				if(!vertexSoup.hasVertex()) break;
				index[2] = vertexSoup.addVertex(vertexSoup.nextVertex());

				vertexSoup.indices.insert(vertexSoup.indices.end(), index, index + 3);
			}
		}

		ResourceId materialId = materials[polylist->getMaterial()];
		model->addVertexData(vertexSoup.vertices, vertexSoup.indices, polylist->getMaterial(), vertexSoup.flags);
	}
}

void CreateScene::visit(ColladaTriangles* triangles) {
	for(const std::vector<int>& p : triangles->getPrimitive()) {
		VertexSoup vertexSoup(triangles, p);

		while(vertexSoup.hasVertex()) {
			int index = vertexSoup.addVertex(vertexSoup.nextVertex());
			vertexSoup.indices.push_back(index);
		}

		ResourceId materialId = materials[triangles->getMaterial()];
		model->addVertexData(vertexSoup.vertices, vertexSoup.indices, triangles->getMaterial(), vertexSoup.flags);
	}
}

void CreateScene::visit(ColladaMaterial* colladaMaterial) {
	ResourceId materialId = compiler->findResource(colladaMaterial->getId().c_str(), Material::TYPE);

	if(materialId == ResourceId(-1)) {
		ColladaInstanceEffect* instanceEffect = colladaMaterial->getInstanceEffect();
		ColladaEffect* effect = document->findEffect(instanceEffect->getUrl());

		ColladaEffectProfileCommon* profileCommon = effect->getProfileCommon();

		for(ColladaNewParam* newParam : profileCommon->getNewParams()) {
			ColladaEffectSurface* surface = newParam->getSurface();
			if(surface) {
				ColladaInitFrom* initFrom = surface->getInitFrom();

				//ColladaImage* image = document->findImage(initFrom->getValue());
				//std::string path = image->getPath();

				//Material* material = new Material(colladaMaterial->getId());

				break;
			}
		}
	}

	materials[colladaMaterial->getId()] = materialId;
}

void CreateScene::visit(ColladaMatrix* colladaMatrix) {
	math::Matrix4 transformation;

	colladaMatrix->getMatrix(transformation.matrix);

	concatMatrix(transformation.transpose());
}

void CreateScene::visit(ColladaRotate* rotate) {
	math::Vector3 axis;

	rotate->getAxis(axis.vector);

	math::Quaternion quat(math::AxisAngle(rotate->getAngle(), axis));

	concatMatrix(math::Matrix4::rotationMatrix(quat));
}

void CreateScene::visit(ColladaScale* scale) {
	math::Vector3 s;

	scale->getScale(s.vector);

	concatMatrix(math::Matrix4::scaleMatrix(s));
}

void CreateScene::visit(ColladaSkew* skew) {

}

void CreateScene::visit(ColladaTranslate* translate) {
	math::Vector3 t;

	translate->getTranslation(t.vector);

	concatMatrix(math::Matrix4::translateMatrix(t));
}
