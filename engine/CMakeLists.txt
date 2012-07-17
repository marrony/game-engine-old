cmake_minimum_required(VERSION 2.6)
project(Engine CXX C)

#sources
set(ENGINE_COMMON_SRC
	src/main/DynamicLibrary.cpp
	
	src/main/math/Matrix4.cpp
	
	src/main/Window.cpp
	#src/main/OpenCL.cpp
	src/main/Application.cpp
	src/main/GameEntity.cpp
	#src/main/Framebuffer.cpp
	src/main/Frustum.cpp
	src/main/KDTree.cpp
	src/main/Light.cpp
	src/main/Material.cpp
	src/main/Image.cpp
	src/main/Node.cpp
	src/main/SceneManager.cpp
	src/main/ControllerManager.cpp
	src/main/KeyboardManager.cpp
	src/main/MouseManager.cpp
	src/main/GraphicManager.cpp
	src/main/ResourceManager.cpp
	src/main/PluginManager.cpp
	
	src/main/SoundManager.cpp
	src/main/RenderEntity.cpp
	src/main/Texture.cpp
	src/main/HardwareTexture.cpp
	src/main/HardwareBuffer.cpp
	
	src/main/Scene.cpp
	
	src/main/Shader.cpp
	
	src/main/Model.cpp
	src/main/Geometry.cpp
	src/main/SpatialLocation.cpp
	
	src/main/SceneTreeBuilder.cpp
	src/main/OctreeBuilder.cpp
	src/main/Octree.cpp
	src/main/OctreeNode.cpp
	src/main/ObjectList.cpp
	src/main/QueryList.cpp
	
	src/main/Stream.cpp
	src/main/Resource.cpp
	
	src/main/SparseVoxelOctreeBuilder.cpp
	src/main/SparseVoxelOctree.cpp
	
	src/main/Clock.cpp
	src/main/Timer.cpp
	src/main/EventManager.cpp
	
	src/main/Terrain.cpp
	src/main/File.cpp
	
	src/main/grammar/Grammar.cpp
	src/main/grammar/Tokenizer.cpp
	
	src/main/gui/Container.cpp
	src/main/gui/Control.cpp
	src/main/gui/Window.cpp
	src/main/gui/WindowManager.cpp
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(ENGINE_SRC
		${ENGINE_COMMON_SRC}
		src/main/win32/Win32Window.cpp
		src/main/win32/Win32GraphicManager.cpp
		src/main/win32/Win32Application.cpp
	)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(ENGINE_SRC
		${ENGINE_COMMON_SRC}
		src/main/linux/LinuxWindow.cpp
		src/main/linux/LinuxGraphicManager.cpp
		src/main/linux/LinuxApplication.cpp
	)
endif()

#
include_directories(inc)
include_directories(inc/gui)
include_directories(../glew/inc)
include_directories(../tinyxml/inc)

add_library(engine ${ENGINE_SRC})

add_library(replacement-new src/main/ReplacementNew.cpp)
#set_target_properties(replacement-new PROPERTIES DEFINE_SYMBOL PLUGIN_EXPORTS)
#target_link_libraries(replacement-new)

add_library(memory-manager SHARED src/main/MemoryManager.cpp)
set_target_properties(memory-manager PROPERTIES DEFINE_SYMBOL PLUGIN_EXPORTS)
#target_link_libraries(memory-manager)

#Tests
set(TEST_SRC
	src/test/TestEntity.cpp
	src/test/TestPlane.cpp
	src/test/TestFrustum.cpp
	src/test/TestSceneManager.cpp
	src/test/TestControllerManager.cpp
	src/test/TestKeyboardManager.cpp
	src/test/TestMouseManager.cpp
	src/test/TestTransformation.cpp
	src/test/TestOctreeNode.cpp
	src/test/TestOctree.cpp
	src/test/TestOctreeData.cpp
	src/test/TestBounding.cpp
	src/test/TestComponent.cpp
	src/test/TestControl.cpp
	#src/test/TestResourceManager.cpp
)

include_directories(../gtest/inc)
include_directories(../gmock/inc)

add_executable(teste-suite ${TEST_SRC})
target_link_libraries(teste-suite engine gtest gmock tinyxml glew ${COMMONS_LIB})
