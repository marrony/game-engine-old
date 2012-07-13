/*
 * Game.h
 *
 *  Created on: 29/04/2012
 *      Author: Marrony
 */

#ifndef GAME_H_
#define GAME_H_

namespace engine {

	class Application;
	class GameTime;

	struct Game {
		virtual ~Game() {}

		virtual void create(Application& app) = 0;

		virtual void destroy() = 0;

		virtual void update(const GameTime& gameTime) = 0;

		virtual void render() = 0;
	};

}  // namespace engine


#endif /* GAME_H_ */
