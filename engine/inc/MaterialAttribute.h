/*
 * MaterialAttribute.h
 *
 *  Created on: 13/03/2012
 *      Author: marrony.neris
 */

#ifndef MATERIALATTRIBUTE_H_
#define MATERIALATTRIBUTE_H_

namespace engine {

	class Shader;

	class MaterialAttribute {
	public:
		MaterialAttribute(const std::string& name) : name(name) { }

		virtual ~MaterialAttribute() { }

		virtual void apply(Shader* effect) = 0;

	protected:
		std::string name;
	};

}  // namespace engine

#endif /* MATERIALATTRIBUTE_H_ */
