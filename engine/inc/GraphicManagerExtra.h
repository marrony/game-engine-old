/*
 * GraphicManagerExtra.h
 *
 *  Created on: 30/07/2012
 *      Author: marrony.neris
 */

#ifndef GRAPHICMANAGEREXTRA_H_
#define GRAPHICMANAGEREXTRA_H_

namespace engine {

	class GraphicManager;

	struct Initializable {
		virtual ~Initializable() {}

		virtual void initialize(GraphicManager* graphicManager) = 0;
	};

	struct Finalizable {
		virtual ~Finalizable() {}

		virtual void finalize(GraphicManager* graphicManager) = 0;
	};

	enum class FrequencyAccess : unsigned char {
		Stream, Static, Dynamic
	};

	enum class NatureAccess : unsigned char {
		Draw, Read, Copy
	};

	enum class BufferType : unsigned char {
		VertexBuffer, IndexBuffer
	};

	enum class BlendEquation : unsigned char {
		Add,              // Result = (Source Color * Source Blend) + (Destination Color * Destination Blend)
		Subtract,         // Result = (Source Color * Source Blend) - (Destination Color * Destination Blend)
		ReverseSubtract,  // Result = (Destination Color * Destination Blend) - (Source Color * Source Blend)
		Min,              // Result = min( (Source Color * Source Blend), (Destination Color * Destination Blend) )
		Max               // Result = max( (Source Color * Source Blend), (Destination Color * Destination Blend) )
	};

}  // namespace engine

#endif /* GRAPHICMANAGEREXTRA_H_ */
