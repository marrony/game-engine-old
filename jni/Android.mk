# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := engine

SRC_DIR := ../engine/src/main
TINYXML_DIR := ../tinyxml/src

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../engine/inc \
	$(LOCAL_PATH)/../game/inc \
	$(LOCAL_PATH)/../tinyxml/inc

LOCAL_SRC_FILES := ../game/src/main/MyGame.cpp \
	../game/src/main/android_main.cpp \
	$(SRC_DIR)/android/AndroidApplication.cpp \
	$(SRC_DIR)/android/AndroidGraphicManager.cpp \
	$(SRC_DIR)/MemoryManager.cpp \
	$(SRC_DIR)/DynamicLibrary.cpp \
	\
	$(SRC_DIR)/math/Matrix4.cpp \
	\
	$(SRC_DIR)/Window.cpp \
	$(SRC_DIR)/Application.cpp \
	$(SRC_DIR)/GameEntity.cpp \
	$(SRC_DIR)/Framebuffer.cpp \
	$(SRC_DIR)/Frustum.cpp \
	$(SRC_DIR)/KDTree.cpp \
	$(SRC_DIR)/Light.cpp \
	$(SRC_DIR)/Material.cpp \
	$(SRC_DIR)/Image.cpp \
	$(SRC_DIR)/Node.cpp \
	$(SRC_DIR)/SceneManager.cpp \
	$(SRC_DIR)/ControllerManager.cpp \
	$(SRC_DIR)/KeyboardManager.cpp \
	$(SRC_DIR)/MouseManager.cpp \
	$(SRC_DIR)/GraphicManager.cpp \
	$(SRC_DIR)/ResourceManager.cpp \
	$(SRC_DIR)/PluginManager.cpp \
	\
	$(SRC_DIR)/SoundManager.cpp \
	$(SRC_DIR)/RenderEntity.cpp \
	$(SRC_DIR)/Texture.cpp \
	$(SRC_DIR)/OpenglTexture.cpp \
	\
	$(SRC_DIR)/Scene.cpp \
	\
	$(SRC_DIR)/TextureAttribute.cpp \
	$(SRC_DIR)/Shader.cpp \
	\
	$(SRC_DIR)/Model.cpp \
	$(SRC_DIR)/Geometry.cpp \
	$(SRC_DIR)/SpatialLocation.cpp \
	\
	$(SRC_DIR)/SceneTreeBuilder.cpp \
	$(SRC_DIR)/OctreeBuilder.cpp \
	$(SRC_DIR)/Octree.cpp \
	$(SRC_DIR)/OctreeNode.cpp \
	$(SRC_DIR)/ObjectList.cpp \
	$(SRC_DIR)/QueryList.cpp \
	\
	$(SRC_DIR)/Stream.cpp \
	$(SRC_DIR)/Resource.cpp \
	\
	$(SRC_DIR)/SparseVoxelOctreeBuilder.cpp \
	$(SRC_DIR)/SparseVoxelOctree.cpp \
	\
	$(SRC_DIR)/Clock.cpp \
	$(SRC_DIR)/Timer.cpp \
	$(SRC_DIR)/EventManager.cpp \
	\
	$(SRC_DIR)/grammar/Grammar.cpp \
	$(SRC_DIR)/grammar/Tokenizer.cpp \
	\
	$(SRC_DIR)/gui/Container.cpp \
	$(SRC_DIR)/gui/Control.cpp \
	$(SRC_DIR)/gui/Window.cpp \
	$(SRC_DIR)/gui/WindowManager.cpp

LOCAL_SRC_FILES += $(TINYXML_DIR)/tinystr.cpp \
	$(TINYXML_DIR)/tinyxml.cpp \
	$(TINYXML_DIR)/tinyxmlerror.cpp \
	$(TINYXML_DIR)/tinyxmlparser.cpp

LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv2
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
