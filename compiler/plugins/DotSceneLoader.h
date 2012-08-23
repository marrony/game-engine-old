#ifndef DOT_SCENELOADER_H
#define DOT_SCENELOADER_H

// Includes
#include <vector>
#include <string>
#include <istream>

#include "ResourceLoader.h"
#include "math/Vector.h"
#include "math/Quaternion.h"

class TiXmlElement;

namespace engine {
	class Scene;
	class Node;
	class ModelInstance;
	class Light;
}

using namespace engine;
using namespace compiler;

class DotSceneLoader : public ResourceLoader {
	ResourceCompiler* compiler;
	ResourceManager* manager;
	Scene* scene;
	Node *rootNode;
public:
	DotSceneLoader();
	virtual ~DotSceneLoader();

	void release();
	virtual void initialize(ResourceCompiler* compiler, ResourceManager* manager);
	virtual void compileResource(const char* fileName, std::map<std::string, std::string>& options);
protected:
	std::vector<std::string> staticObjects;
	std::vector<std::string> dynamicObjects;

	void processScene(TiXmlElement *xmlRoot);

	void processNodes(TiXmlElement *xmlNode);
	void processExternals(TiXmlElement *xmlNode);
	void processEnvironment(TiXmlElement *xmlNode);
	void processTerrain(TiXmlElement *xmlNode);
	void processUserDataReference(TiXmlElement *xmlNode, Node *pParent = 0);
	void processUserDataReference(TiXmlElement *xmlNode, ModelInstance *pEntity);
	void processOctree(TiXmlElement *xmlNode);
	void processLight(TiXmlElement *xmlNode, Node *pParent = 0);
	void processCamera(TiXmlElement *xmlNode, Node *pParent = 0);

	void processNode(TiXmlElement *xmlNode, Node *pParent = 0);
	void processLookTarget(TiXmlElement *xmlNode, Node *pParent);
	void processTrackTarget(TiXmlElement *xmlNode, Node *pParent);
	void processEntity(TiXmlElement *xmlNode, Node *pParent);
	void processParticleSystem(TiXmlElement *xmlNode, Node *pParent);
	void processBillboardSet(TiXmlElement *xmlNode, Node *pParent);
	void processPlane(TiXmlElement *xmlNode, Node *pParent);

	void processFog(TiXmlElement *xmlNode);
	void processSkyBox(TiXmlElement *xmlNode);
	void processSkyDome(TiXmlElement *xmlNode);
	void processSkyPlane(TiXmlElement *xmlNode);
	void processClipping(TiXmlElement *xmlNode);

	void processLightRange(TiXmlElement *xmlNode, Light *pLight);
	void processLightAttenuation(TiXmlElement *xmlNode, Light *pLight);

	std::string getAttrib(TiXmlElement *xmlNode, const std::string &parameter, const std::string &defaultValue = "");
	float getAttribReal(TiXmlElement *xmlNode, const std::string &parameter, float defaultValue = 0);
	bool getAttribBool(TiXmlElement *xmlNode, const std::string &parameter, bool defaultValue = false);

	math::Vector3 parseVector3(TiXmlElement *xmlNode);
	math::Quaternion parseQuaternion(TiXmlElement *xmlNode);
	math::Vector3 parseColour(TiXmlElement *xmlNode);
};

#endif // DOT_SCENELOADER_H
