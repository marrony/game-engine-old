/*
 * Unit.h
 *
 *  Created on: 14/02/2011
 *      Author: marrony.neris
 */

#ifndef UNIT_H_
#define UNIT_H_

namespace engine {

	template<int Meter, int Second, int Mass, typename T = float>
	class Unit {
		T value;
	public:

		typedef T Type;

		template<int Meter2, int Second2, int Mass2, Type>
		friend class Unit;

		INLINE Unit(T newValue) :
			value(newValue) {
		}

		INLINE Unit<Meter, Second, Mass, T> operator+(const Unit<Meter, Second, Mass, T>& other) const {
			return Unit<Meter, Second, Mass, T>(value + other.value);
		}

		INLINE Unit<Meter, Second, Mass, T>& operator+=(const Unit<Meter, Second, Mass, T>& other) const {
			value += other.value;
			return *this;
		}

		INLINE Unit<Meter, Second, Mass, T> operator-(const Unit<Meter, Second, Mass, T>& other) const {
			return Unit<Meter, Second, Mass, T>(value - other.value);
		}

		INLINE Unit<Meter, Second, Mass, T>& operator-=(const Unit<Meter, Second, Mass, T>& other) const {
			value -= other.value;
			return *this;
		}

		template<int Meter2, int Second2, int Mass2, T>
		INLINE Unit<Meter+Meter2, Second+Second2, Mass+Mass2, T> operator*(const Unit<Meter2, Second2, Mass2, T>& other) const {
			return Unit<Meter+Meter2, Second+Second2, Mass+Mass2, T>(value * other.value);
		}

		template<int Meter2, int Second2, int Mass2, T>
		INLINE Unit<Meter-Meter2, Second-Second2, Mass-Mass2, T> operator/(const Unit<Meter2, Second2, Mass2, T>& other) const {
			return Unit<Meter-Meter2, Second-Second2, Mass-Mass2, T>(value / other.value);
		}
	};

	typedef Unit<0, 0, 0> Scalar;
	typedef Unit<1, 0, 0> Meter;
	typedef Unit<0, 1, 0> Second;
	typedef Unit<0, 0, 1> Mass;

	typedef Unit<1, -1, 0> Velocity;
	typedef Unit<1, -2, 0> Acceleration;
	typedef Unit<1, -2, 1> Force;

}  // namespace engine

#endif /* UNIT_H_ */
