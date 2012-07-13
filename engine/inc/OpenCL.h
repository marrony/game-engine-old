/*
 * OpenCL.h
 *
 *  Created on: 02/09/2011
 *      Author: marrony.neris
 */

#ifndef OPENCL_H_
#define OPENCL_H_

#include "GPUComputing.h"

#include <CL/opencl.h>

#include <vector>

namespace engine {

	class GraphicManager;

	/*
	 * TODO Trocar o nome da classe para algo mais generico
	 * e trasnforma-la em uma interface, para poder ligar e
	 * desligar o opencl
	 */
	class OpenCL : public GPUComputing {
	public:
		OpenCL();
		OpenCL(GraphicManager* opengl);
		virtual ~OpenCL();

		void createProgram(const char* source);
	protected:
		void create();
		void destroy();

	private:
		GraphicManager* opengl;
		std::vector<cl_device_id> devices;
		cl_platform_id platformId;
		cl_context context;

	};

} /* namespace engine */

#endif /* OPENCL_H_ */
