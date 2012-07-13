#include "DotSceneLoader.h"
#include "Exception.h"
#include "FileUtil.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "ResourceCompiler.h"
#include "GameEntity.h"
#include "Geometry.h"
#include "Node.h"
#include "Light.h"
#include "Model.h"
#include "Scene.h"

#include "tinyxml.h"

#include <fstream>

DotSceneLoader::DotSceneLoader() :
		rootNode(0) {
}

DotSceneLoader::~DotSceneLoader() {
	for(size_t i = 0; i < resources.size(); i++)
		delete resources[i];
}

void DotSceneLoader::release() {
	delete this;
}

void DotSceneLoader::initialize(ResourceCompiler* compiler, ResourceManager* manager) {
	this->compiler = compiler;
	this->manager = manager;

	compiler->registerLoader(this, "Dot Scene Loader", "scene.xml");
}

void DotSceneLoader::compileResource(const char* fileName, std::map<std::string, std::string>& options) {
	staticObjects.clear();
	dynamicObjects.clear();

	try {
		TiXmlDocument xmlDoc;
		TiXmlElement *xmlRoot;

		xmlDoc.LoadFile(fileName);

		if(xmlDoc.Error()) {
			return;
		}

		xmlRoot = xmlDoc.RootElement();
		if(std::string(xmlRoot->Value()) != "scene") {
			return;
		}

		scene = new Scene(file::getFilename(fileName));
		rootNode = scene->getRoot();

		processScene(xmlRoot);

		scene->createSceneTree();

		resources.push_back(scene);
	} catch(...) {
		if(scene)
			delete scene;
	}
}

void DotSceneLoader::destroyResource(Resource* resource) {
	std::vector<Resource*>::iterator ite = std::find(resources.begin(), resources.end(), resource);

	if(ite != resources.end()) {
		resources.erase(ite);
		delete resource;
	}
}

void DotSceneLoader::processScene(TiXmlElement* xmlRoot) {
	std::string version = getAttrib(xmlRoot, "formatVersion", "unknown");

	std::string message = "[DotSceneLoader] Parsing dotScene file with version " + version;

	if(xmlRoot->Attribute("ID"))
		message += ", id " + std::string(xmlRoot->Attribute("ID"));
	if(xmlRoot->Attribute("sceneManager"))
		message += ", scene manager " + std::string(xmlRoot->Attribute("sceneManager"));
	if(xmlRoot->Attribute("minOgreVersion"))
		message += ", min. Ogre version " + std::string(xmlRoot->Attribute("minOgreVersion"));
	if(xmlRoot->Attribute("author"))
		message += ", author " + std::string(xmlRoot->Attribute("author"));

	TiXmlElement *element;

	// Process nodes (?)
	element = xmlRoot->FirstChildElement("nodes");
	if(element)
		processNodes(element);

	// Process externals (?)
	element = xmlRoot->FirstChildElement("externals");
	if(element)
		processExternals(element);

	// Process environment (?)
	element = xmlRoot->FirstChildElement("environment");
	if(element)
		processEnvironment(element);

	// Process terrain (?)
	element = xmlRoot->FirstChildElement("terrain");
	if(element)
		processTerrain(element);

	// Process userDataReference (?)
	element = xmlRoot->FirstChildElement("userDataReference");
	if(element)
		processUserDataReference(element);

	// Process octree (?)
	element = xmlRoot->FirstChildElement("octree");
	if(element)
		processOctree(element);

	// Process light (?)
	element = xmlRoot->FirstChildElement("light");
	if(element)
		processLight(element);

	// Process camera (?)
	element = xmlRoot->FirstChildElement("camera");
	if(element)
		processCamera(element);
}

void DotSceneLoader::processNodes(TiXmlElement* xmlNode) {
	TiXmlElement *element;

	// Process node (*)
	element = xmlNode->FirstChildElement("node");
	while(element) {
		processNode(element);
		element = element->NextSiblingElement("node");
	}

	// Process position (?)
	element = xmlNode->FirstChildElement("position");
	if(element) {
		rootNode->setPosition(parseVector3(element));
	}

	// Process rotation (?)
	element = xmlNode->FirstChildElement("rotation");
	if(element) {
		rootNode->setOrientation(parseQuaternion(element));
	}

	// Process scale (?)
	element = xmlNode->FirstChildElement("scale");
	if(element) {
		rootNode->setScale(parseVector3(element));
	}
}

void DotSceneLoader::processExternals(TiXmlElement *xmlNode) {
	//TODO
}

void DotSceneLoader::processEnvironment(TiXmlElement *xmlNode) {
	TiXmlElement *element;

	// Process fog (?)
	element = xmlNode->FirstChildElement("fog");
	if(element)
		processFog(element);

	// Process skyBox (?)
	element = xmlNode->FirstChildElement("skyBox");
	if(element)
		processSkyBox(element);

	// Process skyDome (?)
	element = xmlNode->FirstChildElement("skyDome");
	if(element)
		processSkyDome(element);

	// Process skyPlane (?)
	element = xmlNode->FirstChildElement("skyPlane");
	if(element)
		processSkyPlane(element);

	// Process clipping (?)
	element = xmlNode->FirstChildElement("clipping");
	if(element)
		processClipping(element);

	// Process colourAmbient (?)
	element = xmlNode->FirstChildElement("colourAmbient");
	//	if(pElement) //TODO
	//		mSceneMgr->setAmbientLight(parseColour(pElement));

	// Process colourBackground (?)
	element = xmlNode->FirstChildElement("colourBackground");
	if(element)
		; //mSceneMgr->set(parseColour(pElement));

	// Process userDataReference (?)
	element = xmlNode->FirstChildElement("userDataReference");
	if(element)
		processUserDataReference(element);
}

void DotSceneLoader::processTerrain(TiXmlElement *xmlNode) {
	//! @todo Implement this
}

void DotSceneLoader::processUserDataReference(TiXmlElement *xmlNode, Node *pParent) {
	//! @todo Implement this
}

void DotSceneLoader::processOctree(TiXmlElement *xmlNode) {
	//! @todo Implement this
}

void DotSceneLoader::processLight(TiXmlElement *xmlNode, Node *pParent) {
	// Process attributes
	std::string name = getAttrib(xmlNode, "name");
	std::string id = getAttrib(xmlNode, "id");
	std::string sValue = getAttrib(xmlNode, "type");

	Light* light = 0;

	if(sValue == "point") {
		light = scene->createPointLight();
	}
	//	else if(sValue == "directional")
	//		pLight->setType(Light::LT_DIRECTIONAL);
	//	else if(sValue == "spot")
	//		pLight->setType(Light::LT_SPOTLIGHT);
	//	else if(sValue == "radPoint")
	//		pLight->setType(Light::LT_POINT);

	// Create the light
	if(pParent) {
		//lightEntity->addComponent(new NodeAdapter(pParent));
	}

	//light->setVisible(getAttribBool(xmlNode, "visible", true));
	light->castShadow(getAttribBool(xmlNode, "castShadows", true));

	TiXmlElement *pElement;

	// Process position (?)
	pElement = xmlNode->FirstChildElement("position");
	if(pElement)
		light->setPosition(parseVector3(pElement));

	// Process normal (?)
	pElement = xmlNode->FirstChildElement("normal");
	if(pElement)
		light->setDirection(parseVector3(pElement));

	// Process colourDiffuse (?)
	pElement = xmlNode->FirstChildElement("colourDiffuse");
	if(pElement)
		light->setDiffuse(parseColour(pElement));

	// Process colourSpecular (?)
	pElement = xmlNode->FirstChildElement("colourSpecular");
	if(pElement)
		light->setSpecular(parseColour(pElement));

	// Process lightRange (?)
	pElement = xmlNode->FirstChildElement("lightRange");
	if(pElement)
		processLightRange(pElement, light);

	// Process lightAttenuation (?)
	pElement = xmlNode->FirstChildElement("lightAttenuation");
	if(pElement)
		processLightAttenuation(pElement, light);

	// Process userDataReference (?)
	pElement = xmlNode->FirstChildElement("userDataReference");
	if(pElement)
		; //processUserDataReference(pElement, pLight);

	//light->setAmbient(math::Vector3(0.5, 0.5, 0.5));
	//light->setDiffuse(math::Vector3(1.0, 1.0, 1.0));
	//light->setSpecular(math::Vector3(0.5, 0.5, 0.5));
	//light->setPosition(math::Vector3(0.0, 10.0, 0.0));
	//light->setAttenuations(math::Vector3(100.0, 1.0, 1.0));
	light->setDirection(math::Vector3(0.5, -1.0, 0.0).normalize());
	light->castShadow(true);
}

void DotSceneLoader::processCamera(TiXmlElement *xmlNode, Node *pParent) {
#if 0
	// Process attributes
	std::string name = getAttrib(xmlNode, "name");
	std::string id = getAttrib(xmlNode, "id");
	float fov = getAttribReal(xmlNode, "fov", 45);
	float aspectRatio = getAttribReal(xmlNode, "aspectRatio", 1.3333);
	std::string projectionType = getAttrib(xmlNode, "projectionType", "perspective");

	// Create the camera
	Camera *pCamera = sceneManager->createCamera(name);
	if(pParent)
	pParent->attachObject(pCamera);

	// Set the field-of-view
	//! @todo Is this always in degrees?
	pCamera->setFOVy(Ogre::Degree(fov));

	// Set the aspect ratio
	pCamera->setAspectRatio(aspectRatio);

	// Set the projection type
	if(projectionType == "perspective")
	pCamera->setProjectionType(PT_PERSPECTIVE);
	else if(projectionType == "orthographic")
	pCamera->setProjectionType(PT_ORTHOGRAPHIC);

	TiXmlElement *pElement;

	// Process clipping (?)
	pElement = xmlNode->FirstChildElement("clipping");
	if(pElement) {
		float nearDist = getAttribReal(pElement, "nearPlaneDist");
		pCamera->setNearClipDistance(nearDist);

		float farDist = getAttribReal(pElement, "farPlaneDist");
		pCamera->setFarClipDistance(farDist);
	}

	// Process position (?)
	pElement = xmlNode->FirstChildElement("position");
	if(pElement)
	pCamera->setPosition(parseVector3(pElement));

	// Process rotation (?)
	pElement = xmlNode->FirstChildElement("rotation");
	if(pElement)
	pCamera->setOrientation(parseQuaternion(pElement));

	// Process normal (?)
	pElement = xmlNode->FirstChildElement("normal");
	if(pElement)
	;//!< @todo What to do with this element?

	 // Process lookTarget (?)
	pElement = xmlNode->FirstChildElement("lookTarget");
	if(pElement)
	;//!< @todo Implement the camera look target

	 // Process trackTarget (?)
	pElement = xmlNode->FirstChildElement("trackTarget");
	if(pElement)
	;//!< @todo Implement the camera track target

	 // Process userDataReference (?)
	pElement = xmlNode->FirstChildElement("userDataReference");
	if(pElement)
	;//!< @todo Implement the camera user data reference
#endif
}

void DotSceneLoader::processNode(TiXmlElement *xmlNode, Node *pParent) {
	// Construct the node's name
	std::string name = getAttrib(xmlNode, "name");

	// Create the scene node
	Node *node;
	if(name.empty()) {
		// Let Ogre choose the name
		if(pParent) {
			node = scene->createNode();
			pParent->addChild(node);
		} else {
			node = scene->createNode();
			rootNode->addChild(node);
		}
	} else {
		// Provide the name
		if(pParent) {
			node = scene->createNode(/*name*/);
			pParent->addChild(node);
		} else {
			node = scene->createNode(/*name*/);
			rootNode->addChild(node);
		}
	}

	// Process other attributes
	std::string id = getAttrib(xmlNode, "id");
	bool isTarget = getAttribBool(xmlNode, "isTarget");

	TiXmlElement *element;

	// Process position (?)
	element = xmlNode->FirstChildElement("position");
	if(element) {
		node->setPosition(parseVector3(element));
	}

	// Process rotation (?)
	element = xmlNode->FirstChildElement("rotation");
	if(element) {
		node->setOrientation(parseQuaternion(element));
	}

	// Process scale (?)
	element = xmlNode->FirstChildElement("scale");
	if(element) {
		node->setScale(parseVector3(element));
	}

	// Process lookTarget (?)
	element = xmlNode->FirstChildElement("lookTarget");
	if(element)
		processLookTarget(element, node);

	// Process trackTarget (?)
	element = xmlNode->FirstChildElement("trackTarget");
	if(element)
		processTrackTarget(element, node);

	// Process node (*)
	element = xmlNode->FirstChildElement("node");
	while(element) {
		processNode(element, node);
		element = element->NextSiblingElement("node");
	}

	// Process entity (*)
	element = xmlNode->FirstChildElement("entity");
	while(element) {
		processEntity(element, node);
		element = element->NextSiblingElement("entity");
	}

	// Process light (*)
	element = xmlNode->FirstChildElement("light");
	while(element) {
		processLight(element, node);
		element = element->NextSiblingElement("light");
	}

	// Process camera (*)
	element = xmlNode->FirstChildElement("camera");
	while(element) {
		processCamera(element, node);
		element = element->NextSiblingElement("camera");
	}

	// Process particleSystem (*)
	element = xmlNode->FirstChildElement("particleSystem");
	while(element) {
		processParticleSystem(element, node);
		element = element->NextSiblingElement("particleSystem");
	}

	// Process billboardSet (*)
	element = xmlNode->FirstChildElement("billboardSet");
	while(element) {
		processBillboardSet(element, node);
		element = element->NextSiblingElement("billboardSet");
	}

	// Process plane (*)
	element = xmlNode->FirstChildElement("plane");
	while(element) {
		processPlane(element, node);
		element = element->NextSiblingElement("plane");
	}

	// Process userDataReference (?)
	element = xmlNode->FirstChildElement("userDataReference");
	if(element)
		processUserDataReference(element, node);
}

void DotSceneLoader::processLookTarget(TiXmlElement *xmlNode, Node *pParent) {
#if 0
	//! @todo Is this correct? Cause I don't have a clue actually

	// Process attributes
	std::string nodeName = getAttrib(xmlNode, "nodeName");

	Node::TransformSpace relativeTo = Node::TS_PARENT;
	std::string sValue = getAttrib(xmlNode, "relativeTo");
	if(sValue == "local")
	relativeTo = Node::TS_LOCAL;
	else if(sValue == "parent")
	relativeTo = Node::TS_PARENT;
	else if(sValue == "world")
	relativeTo = Node::TS_WORLD;

	TiXmlElement *pElement;

	// Process position (?)
	math::Vector3 position;
	pElement = xmlNode->FirstChildElement("position");
	if(pElement)
	position = parseVector3(pElement);

	// Process localDirection (?)
	math::Vector3 localDirection = math::Vector3::NEGATIVE_UNIT_Z;
	pElement = xmlNode->FirstChildElement("localDirection");
	if(pElement)
	localDirection = parseVector3(pElement);

	// Setup the look target
	try {
		if(!nodeName.empty()) {
			Node *pLookNode = sceneManager->getSceneNode(nodeName);
			position = pLookNode->_getDerivedPosition();
		}

		pParent->lookAt(position, relativeTo, localDirection);
	} catch(Ogre::Exception &/*e*/) {
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a look target!");
	}
#endif
}

void DotSceneLoader::processTrackTarget(TiXmlElement *xmlNode, Node *pParent) {
#if 0
	// Process attributes
	std::string nodeName = getAttrib(xmlNode, "nodeName");

	TiXmlElement *pElement;

	// Process localDirection (?)
	math::Vector3 localDirection = math::Vector3::NEGATIVE_UNIT_Z;
	pElement = xmlNode->FirstChildElement("localDirection");
	if(pElement)
	localDirection = parseVector3(pElement);

	// Process offset (?)
	math::Vector3 offset = math::Vector3::ZERO;
	pElement = xmlNode->FirstChildElement("offset");
	if(pElement)
	offset = parseVector3(pElement);

	// Setup the track target
	try {
		Node *pTrackNode = sceneManager->getSceneNode(nodeName);
		pParent->setAutoTracking(true, pTrackNode, localDirection, offset);
	} catch(Ogre::Exception &/*e*/) {
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error processing a track target!");
	}
#endif
}

void DotSceneLoader::processEntity(TiXmlElement *xmlNode, Node *parent) {
	// Process attributes
	std::string name = getAttrib(xmlNode, "name");
	std::string id = getAttrib(xmlNode, "id");
	std::string meshFile = getAttrib(xmlNode, "meshFile");
	std::string materialFile = getAttrib(xmlNode, "materialFile");
	bool isStatic = getAttribBool(xmlNode, "static", false);
	bool castShadows = getAttribBool(xmlNode, "castShadows", true);

	// TEMP: Maintain a list of static and dynamic objects
	if(isStatic)
		staticObjects.push_back(name);
	else
		dynamicObjects.push_back(name);

	TiXmlElement *element;

	// Process vertexBuffer (?)
	element = xmlNode->FirstChildElement("vertexBuffer");
	if(element)
		; //processVertexBuffer(pElement);

	// Process indexBuffer (?)
	element = xmlNode->FirstChildElement("indexBuffer");
	if(element)
		; //processIndexBuffer(pElement);

	// Create the entity
	Geometry *geometry = 0;
	try {
		std::map<std::string, std::string> options;

		options["only-geometry"] = "true";
		options["geometry-name"] = file::getFilename(meshFile);

		compiler->compile(meshFile.c_str(), options);
		Model* model = manager->loadModel(file::getFilename(meshFile));

		Geometry* entity = scene->createGeometry(model, model->getBoundingBox(), parent);
	} catch(Exception &/*e*/) {
	}

	// Process userDataReference (?)
	element = xmlNode->FirstChildElement("userDataReference");
	if(element)
		processUserDataReference(element, geometry);

}

void DotSceneLoader::processParticleSystem(TiXmlElement *xmlNode, Node *pParent) {
#if 0
	// Process attributes
	std::string name = getAttrib(xmlNode, "name");
	std::string id = getAttrib(xmlNode, "id");
	std::string file = getAttrib(xmlNode, "file");

	// Create the particle system
	try {
		ParticleSystem *pParticles = sceneManager->createParticleSystem(name, file);
		pParent->attachObject(pParticles);
	} catch(Ogre::Exception &/*e*/) {
		LogManager::getSingleton().logMessage("[DotSceneLoader] Error creating a particle system!");
	}
#endif
}

void DotSceneLoader::processBillboardSet(TiXmlElement *xmlNode, Node *pParent) {
	//! @todo Implement this
}

void DotSceneLoader::processPlane(TiXmlElement *xmlNode, Node *pParent) {
	//! @todo Implement this
}

void DotSceneLoader::processFog(TiXmlElement *xmlNode) {
#if 0
	// Process attributes
	float expDensity = getAttribReal(xmlNode, "expDensity", 0.001);
	float linearStart = getAttribReal(xmlNode, "linearStart", 0.0);
	float linearEnd = getAttribReal(xmlNode, "linearEnd", 1.0);

	FogMode mode = FOG_NONE;
	std::string sMode = getAttrib(xmlNode, "mode");
	if(sMode == "none")
	mode = FOG_NONE;
	else if(sMode == "exp")
	mode = FOG_EXP;
	else if(sMode == "exp2")
	mode = FOG_EXP2;
	else if(sMode == "linear")
	mode = FOG_LINEAR;

	TiXmlElement *pElement;

	// Process colourDiffuse (?)
	math::Vector3 colourDiffuse = math::Vector3::White;
	pElement = xmlNode->FirstChildElement("colourDiffuse");
	if(pElement)
	colourDiffuse = parseColour(pElement);

	// Setup the fog
	sceneManager->setFog(mode, colourDiffuse, expDensity, linearStart, linearEnd);
#endif
}

void DotSceneLoader::processSkyBox(TiXmlElement *xmlNode) {
#if 0
	// Process attributes
	std::string material = getAttrib(xmlNode, "material");
	float distance = getAttribReal(xmlNode, "distance", 5000);
	bool drawFirst = getAttribBool(xmlNode, "drawFirst", true);

	TiXmlElement *pElement;

	// Process rotation (?)
	math::Quaternion rotation = math::Quaternion::IDENTITY;
	pElement = xmlNode->FirstChildElement("rotation");
	if(pElement)
	rotation = parseQuaternion(pElement);

	// Setup the sky box
	sceneManager->setSkyBox(true, material, distance, drawFirst, rotation, m_sGroupName);
#endif
}

void DotSceneLoader::processSkyDome(TiXmlElement *xmlNode) {
#if 0
	// Process attributes
	std::string material = xmlNode->Attribute("material");
	float curvature = getAttribReal(xmlNode, "curvature", 10);
	float tiling = getAttribReal(xmlNode, "tiling", 8);
	float distance = getAttribReal(xmlNode, "distance", 4000);
	bool drawFirst = getAttribBool(xmlNode, "drawFirst", true);

	TiXmlElement *pElement;

	// Process rotation (?)
	math::Quaternion rotation = math::Quaternion::IDENTITY;
	pElement = xmlNode->FirstChildElement("rotation");
	if(pElement)
	rotation = parseQuaternion(pElement);

	// Setup the sky dome
	sceneManager->setSkyDome(true, material, curvature, tiling, distance, drawFirst, rotation, 16, 16, -1, m_sGroupName);
#endif
}

void DotSceneLoader::processSkyPlane(TiXmlElement *xmlNode) {
#if 0
	// Process attributes
	std::string material = getAttrib(xmlNode, "material");
	float planeX = getAttribReal(xmlNode, "planeX", 0);
	float planeY = getAttribReal(xmlNode, "planeY", -1);
	float planeZ = getAttribReal(xmlNode, "planeX", 0);
	float planeD = getAttribReal(xmlNode, "planeD", 5000);
	float scale = getAttribReal(xmlNode, "scale", 1000);
	float bow = getAttribReal(xmlNode, "bow", 0);
	float tiling = getAttribReal(xmlNode, "tiling", 10);
	bool drawFirst = getAttribBool(xmlNode, "drawFirst", true);

	// Setup the sky plane
	Plane plane;
	plane.normal = math::Vector3(planeX, planeY, planeZ);
	plane.d = planeD;
	sceneManager->setSkyPlane(true, plane, material, scale, tiling, drawFirst, bow, 1, 1, m_sGroupName);
#endif
}

void DotSceneLoader::processClipping(TiXmlElement *xmlNode) {
	//! @todo Implement this

	// Process attributes
	float fNear = getAttribReal(xmlNode, "near", 0);
	float fFar = getAttribReal(xmlNode, "far", 1);
}

void DotSceneLoader::processLightRange(TiXmlElement *xmlNode, Light *pLight) {
	// Process attributes
	float inner = getAttribReal(xmlNode, "inner");
	float outer = getAttribReal(xmlNode, "outer");
	float falloff = getAttribReal(xmlNode, "falloff", 1.0);

	// Setup the light range
	//	pLight->setSpotlightRange(Angle(inner), Angle(outer), falloff);
}

void DotSceneLoader::processLightAttenuation(TiXmlElement *xmlNode, Light *pLight) {
	// Process attributes
	float range = getAttribReal(xmlNode, "range");
	float constant = getAttribReal(xmlNode, "constant");
	float linear = getAttribReal(xmlNode, "linear");
	float quadratic = getAttribReal(xmlNode, "quadratic");

	// Setup the light attenuation
	pLight->setAttenuations(math::Vector3(range, 1, 1));
}

std::string DotSceneLoader::getAttrib(TiXmlElement *xmlNode, const std::string &attrib,
		const std::string &defaultValue) {
	if(xmlNode->Attribute(attrib.c_str()))
		return xmlNode->Attribute(attrib.c_str());
	else
		return defaultValue;
}

float DotSceneLoader::getAttribReal(TiXmlElement *xmlNode, const std::string &attrib, float defaultValue) {
	if(xmlNode->Attribute(attrib.c_str())) {
		float value;
		sscanf(xmlNode->Attribute(attrib.c_str()), "%f", &value);
		return value;
	}

	return defaultValue;
}

bool DotSceneLoader::getAttribBool(TiXmlElement *xmlNode, const std::string &attrib, bool defaultValue) {
	if(!xmlNode->Attribute(attrib.c_str()))
		return defaultValue;

	return std::string(xmlNode->Attribute(attrib.c_str())) == "true";
}

math::Vector3 DotSceneLoader::parseVector3(TiXmlElement *xmlNode) {
	math::Vector3 value;

	sscanf(xmlNode->Attribute("x"), "%f", &value.x);
	sscanf(xmlNode->Attribute("y"), "%f", &value.y);
	sscanf(xmlNode->Attribute("z"), "%f", &value.z);

	return value;
}

math::Quaternion DotSceneLoader::parseQuaternion(TiXmlElement *xmlNode) {
	//! @todo Fix this crap!

	math::Quaternion orient;

	if(xmlNode->Attribute("qx")) {
		sscanf(xmlNode->Attribute("qw"), "%f", &orient.s);
		sscanf(xmlNode->Attribute("qx"), "%f", &orient.x);
		sscanf(xmlNode->Attribute("qy"), "%f", &orient.y);
		sscanf(xmlNode->Attribute("qz"), "%f", &orient.z);
	} else if(xmlNode->Attribute("axisX")) {
		math::Vector3 axis;
		float angle;

		sscanf(xmlNode->Attribute("axisX"), "%f", &axis.x);
		sscanf(xmlNode->Attribute("axisY"), "%f", &axis.y);
		sscanf(xmlNode->Attribute("axisZ"), "%f", &axis.z);
		sscanf(xmlNode->Attribute("angle"), "%f", &angle);

		orient = math::Quaternion(math::AxisAngle(angle, axis));
//	} else if(xmlNode->Attribute("angleX")) {
//		math::Vector3 axis;
//		axis.x = StringConverter::parseReal(xmlNode->Attribute("angleX"));
//		axis.y = StringConverter::parseReal(xmlNode->Attribute("angleY"));
//		axis.z = StringConverter::parseReal(xmlNode->Attribute("angleZ"));
//		//orientation.FromAxes(&axis);
//		//orientation.F
	}

	return orient;
}

math::Vector3 DotSceneLoader::parseColour(TiXmlElement *xmlNode) {
	math::Vector3 value;

	sscanf(xmlNode->Attribute("r"), "%f", &value.x);
	sscanf(xmlNode->Attribute("g"), "%f", &value.y);
	sscanf(xmlNode->Attribute("b"), "%f", &value.z);

//	if(xmlNode->Attribute("a") != 0) {
//		sscanf(xmlNode->Attribute("a"), "%f", &value.w);
//	} else {
//		value.w = 1.0;
//	}

	return value;
}

void DotSceneLoader::processUserDataReference(TiXmlElement *xmlNode, Geometry *pEntity) {
	std::string str = xmlNode->Attribute("id");
//	pEntity->setUserAny(Any(str));
}

static Module* dynamicLib;

extern "C" Module* getModule() {
	return dynamicLib;
}

extern "C" EXPIMP Plugin* initializePlugin(Module* dynLib) {
	dynamicLib = dynLib;
	return new DotSceneLoader;
}
