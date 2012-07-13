/*
 * CreateScene.h
 *
 *  Created on: 22/05/2012
 *      Author: marrony.neris
 */

#ifndef CREATESCENE_H_
#define CREATESCENE_H_

#include "ColladaElement.h"
#include "ColladaScene.h"
#include "ColladaNode.h"
#include "ColladaMaterial.h"
#include "ColladaDocument.h"
#include "ColladaMatrix.h"
#include "ColladaInstanceGeometry.h"

#include "ResourceCompiler.h"
#include "ResourceLoader.h"

#include "Scene.h"
#include "Node.h"
#include "Model.h"

#include <vector>
#include <stack>
#include <map>

using namespace engine;
using namespace compiler;

class CreateScene : public Visitor,
						public ColladaSceneVisitor,
						public ColladaNodeVisitor,
						public ColladaMatrixVisitor,
						public ColladaRotateVisitor,
						public ColladaScaleVisitor,
						public ColladaSkewVisitor,
						public ColladaTranslateVisitor,
						public ColladaInstanceGeometryVisitor,
						public ColladaMaterialVisitor {
	Scene* product;
	std::stack<Node*> nodes;
	std::stack<math::Matrix4> matrix;
	ColladaDocument* document;
	std::vector<Resource*> resources;
	ResourceCompiler* compiler;
	class ResourceManager* manager;
	ResourceLoader* loader;
	//std::map<std::string, ResourceId> materials;

	void pushNode(Node* node);
	void popNode();
	void concatNode(Node* node);
	void concatMatrix(math::Matrix4 matrix);
public:
	CreateScene(ResourceCompiler* compiler, class ResourceManager* manager, ResourceLoader* loader, ColladaDocument* document);

	Scene* getScene() const { return product; }
	const std::vector<Resource*>& getResourcesCreated() const { return resources; }

	virtual void visit(ColladaScene* scene);
	virtual void visit(ColladaNode* node);
	virtual void visit(ColladaInstanceGeometry* instanceGeometry);
	virtual void visit(ColladaMaterial* colladaMaterial);
	virtual void visit(ColladaMatrix* matrix);
	virtual void visit(ColladaRotate* rotate);
	virtual void visit(ColladaScale* scale);
	virtual void visit(ColladaSkew* skew);
	virtual void visit(ColladaTranslate* translate);
};

#endif /* CREATESCENE_H_ */
