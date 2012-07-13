/*
 * StringTokenizer.h
 *
 *  Created on: 26/03/2012
 *      Author: Marrony
 */

#ifndef STRINGTOKENIZER_H_
#define STRINGTOKENIZER_H_

#include <string>
#include <string.h>

namespace engine {

	class StringTokenizer {
	public:
		StringTokenizer(const char* str, const char* delim, bool retDelims)
				: str(str), delimiters(delim), retDelims(retDelims), currentPosition(0), newPosition(-1) {
			maxPosition = strlen(str);
		}

		std::pair<int, int> nextToken() {
			currentPosition = newPosition >= 0 ? newPosition : skipDelimiters(currentPosition);

			newPosition = -1;

			int start = currentPosition;
			currentPosition = scanToken(currentPosition);
			return {start, currentPosition};
		}

		bool hasMoreTokens() {
			newPosition = skipDelimiters(currentPosition);
			return newPosition < maxPosition;
		}
	private:
		int skipDelimiters(int startPos) {
			int position = startPos;

			while(!retDelims && position < maxPosition) {
				char c = str[position];

				if(strchr(delimiters, c) != 0)
					break;

				position++;
			}

			return position;
		}

		int scanToken(int startPos) {
			int position = startPos;

			while(position < maxPosition) {
				char c = str[position];
				if(strchr(delimiters, c) != 0)
					break;
				position++;
			}

			if(retDelims && startPos == position) {
				char c = str[position];
				if(strchr(delimiters, c) != 0)
					position++;
			}

			return position;
		}

		const char* str;
		const char* delimiters;
		int currentPosition;
		int newPosition;
		int maxPosition;
		bool retDelims;
	};


}  // namespace engine


#endif /* STRINGTOKENIZER_H_ */
