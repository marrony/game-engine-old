/*
 * FBXLoader.h
 *
 *  Created on: 27/09/2011
 *      Author: marrony
 */

#ifndef FBXLOADER_H_
#define FBXLOADER_H_

namespace engine {

	class FBXLoader : public ModelLoader {
	public:
		virtual Model* loadModel(std::istream& file);
	};

} /* namespace engine */

#endif /* FBXLOADER_H_ */
