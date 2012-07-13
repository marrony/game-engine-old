cmake_minimum_required(VERSION 2.6)
project(Compiler CXX C)

include_directories(../../engine/inc)
include_directories(../../engine/inc/gui)
include_directories(../../engine/inc/win32)
include_directories(../../tinyxml/inc)
include_directories(inc)

SET(PLUGIN_LIBS replacement-new memory-manager engine tinyxml glew ${COMMONS_LIB})

add_library(tga-loader SHARED TGALoader.cpp MemoryTexture.cpp)
target_link_libraries(tga-loader ${PLUGIN_LIBS})
set_target_properties(tga-loader PROPERTIES DEFINE_SYMBOL PLUGIN_EXPORTS)

add_library(dotmesh-loader SHARED DotMeshLoader.cpp)
target_link_libraries(dotmesh-loader ${PLUGIN_LIBS})
set_target_properties(dotmesh-loader PROPERTIES DEFINE_SYMBOL PLUGIN_EXPORTS)

add_library(effect-loader SHARED EffectLoader.cpp)
target_link_libraries(effect-loader ${PLUGIN_LIBS})
set_target_properties(effect-loader PROPERTIES DEFINE_SYMBOL PLUGIN_EXPORTS)

add_library(material-loader SHARED MaterialLoader.cpp MemoryTexture.cpp)
target_link_libraries(material-loader ${PLUGIN_LIBS})
set_target_properties(material-loader PROPERTIES DEFINE_SYMBOL PLUGIN_EXPORTS)

add_library(ms3d-loader SHARED MS3DModelLoader.cpp MS3DModel.cpp mathlib.cpp)
target_link_libraries(ms3d-loader ${PLUGIN_LIBS})
set_target_properties(ms3d-loader PROPERTIES DEFINE_SYMBOL PLUGIN_EXPORTS)

SET(SRC_COLLADA
	collada/src/ColladaArray.cpp
	collada/src/ColladaAsset.cpp
	collada/src/ColladaBind.cpp
	collada/src/ColladaBindMaterial.cpp
	collada/src/ColladaBindVertexInput.cpp
	collada/src/ColladaCamera.cpp
	collada/src/ColladaDocument.cpp
	collada/src/ColladaEffect.cpp
	collada/src/ColladaEffectConstant.cpp
	collada/src/ColladaEffectNewParam.cpp
	collada/src/ColladaEffectProfileCommon.cpp
	collada/src/ColladaEffectSampler2D.cpp
	collada/src/ColladaEffectSemantic.cpp
	collada/src/ColladaEffectSurface.cpp
	collada/src/ColladaEffectTechnique.cpp
	collada/src/ColladaElement.cpp
	collada/src/ColladaExtra.cpp
	collada/src/ColladaGeometricPrimitive.cpp
	collada/src/ColladaGeometry.cpp
	collada/src/ColladaImage.cpp
	collada/src/ColladaInput.cpp
	collada/src/ColladaInstanceController.cpp
	collada/src/ColladaInstanceEffect.cpp
	collada/src/ColladaInstanceGeometry.cpp
	collada/src/ColladaInstanceMaterial.cpp
	collada/src/ColladaInstanceNode.cpp
	collada/src/ColladaLibrary.cpp
	collada/src/ColladaLight.cpp
	collada/src/ColladaMaterial.cpp
	collada/src/ColladaMatrix.cpp
	collada/src/ColladaMesh.cpp
	collada/src/ColladaNode.cpp
	collada/src/ColladaPolyList.cpp
	collada/src/ColladaPrimitive.cpp
	collada/src/ColladaScene.cpp
	collada/src/ColladaSceneLoader.cpp
	collada/src/ColladaSkeleton.cpp
	collada/src/ColladaSource.cpp
	collada/src/ColladaTriangles.cpp
	collada/src/ColladaVertices.cpp
	collada/src/ColladaVisualScene.cpp
	
	collada/src/CreateGeometry.cpp
	collada/src/CreateScene.cpp
)

include_directories(collada/inc)
add_library(collada-loader SHARED ${SRC_COLLADA})
target_link_libraries(collada-loader ${PLUGIN_LIBS})
set_target_properties(collada-loader PROPERTIES DEFINE_SYMBOL PLUGIN_EXPORTS)

add_library(dotscene-loader SHARED DotSceneLoader.cpp)
target_link_libraries(dotscene-loader ${PLUGIN_LIBS})
set_target_properties(dotscene-loader PROPERTIES DEFINE_SYMBOL PLUGIN_EXPORTS)
