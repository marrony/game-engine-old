/*
 * Component.h
 *
 *  Created on: 05/05/2011
 *      Author: marrony
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

namespace engine {

	class GameEntity;
	class GameTime;

	class Component {
	public:
		virtual ~Component() { }

		virtual void updateBindings(GameEntity* owner) { }

	protected:
		Component() { }

	private:
	};

	class ControllerComponent : public Component {
	public:
		virtual void update(const GameTime& gameTime) = 0;

	protected:
		ControllerComponent() : Component() { }
	};

	class DataComponent : public Component {
	public:

	protected:
		DataComponent() : Component() { }
	};

}

#endif /* COMPONENT_H_ */
