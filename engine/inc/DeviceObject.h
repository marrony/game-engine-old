/*
 * DeviceObject.h
 *
 *  Created on: 06/08/2012
 *      Author: marrony.neris
 */

#ifndef DEVICEOBJECT_H_
#define DEVICEOBJECT_H_

namespace engine {

	struct DeviceObject {
		virtual ~DeviceObject() {}

		virtual int getHandle() const = 0;
		virtual void setHandle(int handle) = 0;
	};

}  // namespace engine


#endif /* DEVICEOBJECT_H_ */
