/*
 * FBXLoader.cpp
 *
 *  Created on: 27/09/2011
 *      Author: marrony
 */

#include "FBXLoader.h"

namespace engine {

	enum TokenType {
		INVALID,
		ID,
		NUMBER,
		STRING,
		COMMENT,
		SLASH,
		COMMA,       // ,
		COLON,       // :
		SEMICOLON,   // ;
		LBRACE,      // {
		RBRACE,      // }
		END_OF_FILE,
	};

	class FBXLexer {
	};

	Model* FBXLoader::loadModel(std::istream& file) {
		return 0;
	}

} /* namespace engine */
