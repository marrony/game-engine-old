/*
 * Rect.h
 *
 *  Created on: 04/02/2011
 *      Author: marrony.neris
 */

#ifndef RECT_H_
#define RECT_H_

#include <ostream>

namespace engine {

	namespace gui {
		struct Position {
			int x;
			int y;

			Position(int x = 0, int y = 0) : x(x), y(y) { }

			bool operator==(const Position& other) const {
				return x == other.x && y == other.y;
			}
		};

		static inline std::ostream& operator<<(std::ostream& os, const Position& p) {
			os << "[" << p.x << ", " << p.y << "]" << std::endl;
			return os;
		}

		struct Size {
			int width;
			int height;

			Size(int width = 0, int height = 0) : width(width), height(height) { }

			bool operator==(const Size& other) const {
				return width == other.width && height == other.height;
			}
		};

		class Rect {
			Position position;
			Size size;
		public:
			Rect(int newX, int newY, int newWidth, int newHeight) :
				position(newX, newY), size(newWidth, newHeight) {
			}

			inline void resize(const Size& size) {
				this->size = size;
			}

			inline bool isInside(int ptx, int pty) const {
				return ptx >= position.x && ptx <= (position.x + size.width) &&
						pty >= position.y && pty <= (position.y + size.height);
			}

			inline Position getPosition() const {
				return position;
			}

			inline Size getSize() const {
				return size;
			}

			inline bool contains(int x, int y) {
				return true;
			}
		};

	} // namespace gui

} // namespace engine

#endif /* RECT_H_ */
