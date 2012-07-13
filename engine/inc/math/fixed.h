#ifndef FIXED_H_
#define FIXED_H_

#include <stdint.h>

template<int nbits>
class fixedNbits {
	inline static int32_t fixdiv(int32_t a, int32_t b) {
#if 1
		int64_t x = int64_t(a) << nbits;
		return (int32_t)(x / b);
#else
		union {
			int64_t a;
			struct {
				int32_t l;
				int32_t h;
			};
		} x;

		x.l = a << nbits;
		x.h = a >> (sizeof(int32_t) * 8 - nbits);
		return (int32_t)(x.a / b);
#endif
	}

	inline static int32_t fixmul(int32_t a, int32_t b) {
		return (int32_t)(((int64_t)a * b) >> nbits);
	}

	inline static void floor_divmod(int32_t numerator, int32_t denominator, int32_t &floor, int32_t &mod) {
		if(numerator >= 0) {
			// positive case, C is okay
			floor = numerator / denominator;
			mod = numerator % denominator;
		} else {
			// Numerator is negative, do the right thing
			floor = -((-numerator) / denominator);
			mod = (-numerator) % denominator;
			if(mod) {
				// there is a remainder
				floor--;
				mod = denominator - mod;
			}
		}
	}

	int32_t value;

#ifndef NDEBUG
	float floatValue;
#endif

	uint32_t multiplier() const {
		return (uint32_t)(1 << nbits);
	}
public:

	static const fixedNbits<nbits> ONE;
	static const fixedNbits<nbits> EPSILON;

	fixedNbits() {
	}

	fixedNbits(const fixedNbits<nbits>& other) :
		value(other.value) {

#ifndef NDEBUG
	floatValue = getFloat();
#endif
	}

	explicit fixedNbits(int32_t _value) :
		value(_value << nbits) {

#ifndef NDEBUG
	floatValue = getFloat();
#endif
	}

	explicit fixedNbits(float _value) :
		value(_value * float(multiplier())) {

#ifndef NDEBUG
	floatValue = getFloat();
#endif
	}

	explicit fixedNbits(double _value) :
		value(_value * double(multiplier())) {

#ifndef NDEBUG
	floatValue = getFloat();
#endif
	}

	fixedNbits(double _value, int32_t subPixelBits) :
		value(_value * double(multiplier())) {

		const int32_t fixedOne = multiplier();
		const int32_t snapMask = ~((fixedOne / (1 << subPixelBits)) - 1);

		value &= snapMask;

#ifndef NDEBUG
	floatValue = getFloat();
#endif
	}

	int32_t getValue() const {
		return value;
	}

	int32_t getInt() const {
		return value >> nbits;
	}

	float getFloat() const {
		return value / float(multiplier());
	}

	double getDouble() const {
		return value / double(multiplier());
	}

	operator int() const {
		return getInt();
	}

	operator float() const {
		return getFloat();
	}

	operator double() const {
		return getDouble();
	}

	fixedNbits<nbits> operator-() const {
		return fixedNbits<nbits>::fromValue(-value);
	}

	bool operator==(const fixedNbits<nbits> other) const {
		return value == other.value;
	}

	bool operator!=(const fixedNbits<nbits> other) const {
		return value != other.value;
	}

	bool operator>(const fixedNbits<nbits> other) const {
		return value > other.value;
	}

	bool operator>=(const fixedNbits<nbits> other) const {
		return value >= other.value;
	}

	bool operator<(const fixedNbits<nbits> other) const {
		return value < other.value;
	}

	bool operator<=(const fixedNbits<nbits> other) const {
		return value <= other.value;
	}

	fixedNbits operator*(const fixedNbits<nbits> other) const {
		return fixedNbits::fromValue(fixedNbits<nbits>::fixmul(value, other.value));
	}

	fixedNbits operator/(const fixedNbits<nbits> other) const {
		return fixedNbits::fromValue(fixedNbits<nbits>::fixdiv(value, other.value));
	}

	fixedNbits operator+(const fixedNbits<nbits> other) const {
		return fixedNbits::fromValue(value + other.value);
	}

	fixedNbits operator-(const fixedNbits<nbits> other) const {
		return fixedNbits::fromValue(value - other.value);
	}

	fixedNbits operator%(const fixedNbits<nbits> other) const {
		int32_t div, mod;

		fixedNbits<nbits>::floor_divmod(value, other.value, div, mod);

		return fixedNbits::fromValue(mod);
	}

	fixedNbits& operator+=(const fixedNbits<nbits> other) {
		value += other.value;

#ifndef NDEBUG
	floatValue = getFloat();
#endif
		return *this;
	}

	fixedNbits& operator-=(const fixedNbits<nbits> other) {
		value -= other.value;

#ifndef NDEBUG
	floatValue = getFloat();
#endif
		return *this;
	}

	fixedNbits& operator*=(const fixedNbits<nbits> other) {
		value = fixedNbits<nbits>::fixmul(value, other.value);

#ifndef NDEBUG
	floatValue = getFloat();
#endif
		return *this;
	}

	fixedNbits& operator/=(const fixedNbits<nbits> other) {
		value = fixedNbits<nbits>::fixdiv(value, other.value);

#ifndef NDEBUG
	floatValue = getFloat();
#endif
		return *this;
	}

	inline static fixedNbits<nbits> fromValue(int32_t value) {
		fixedNbits<nbits> f;
		f.value = value;

#ifndef NDEBUG
		f.floatValue = f.getFloat();
#endif

		return f;
	}
};

template<int nbits>
const fixedNbits<nbits> fixedNbits<nbits>::ONE = fixedNbits<nbits>(1);

template<int nbits>
const fixedNbits<nbits> fixedNbits<nbits>::EPSILON = fixedNbits<nbits>::fromValue(1);

template<int nbits>
int32_t ceil(fixedNbits<nbits> value) {
	int32_t v = value.getValue();

	const int32_t mask = (1 << nbits) - 1;
	const int32_t b = v & mask;

	return (v >> nbits) + (b > 0);
}

template<int nbits>
int32_t floor(fixedNbits<nbits> value) {
	return value.getInt();
}

//inline int GlFixedToIntRound(S32 f) {
//	return (f + 0x0800) >> 16;
//}
//
//inline int GlFixedToIntRoundUp(S32 f) {
//	return (f + 0xffff) >> 16;
//}

typedef fixedNbits<4> float284;
typedef fixedNbits<16> float1616;

#endif /* FLOATN_H_ */
