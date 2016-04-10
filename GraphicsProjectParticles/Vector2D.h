#ifndef _VECTOR2D_H
#define _VECTOR2D_H

template <typename T>

class Vector2D {
public:
	Vector2D() {}
	Vector2D(T v) : x(v), y(v) {}
	Vector2D(T x, T y) : x(x), y(y) {}

	Vector2D operator+(const Vector2D& o) const {
		return Vector2D( x + o.x, y + o.y);
	}

	Vector2D operator-(const Vector2D& o) const {
		return Vector2D( x - o.x, y - o.y);
	}

	Vector2D operator*(T t) const {
		return Vector2D( x * t, y * t);
	}

	Vector2D operator/(T t) const {
		return Vector2D( x / t, y / t);
	}
	bool operator==(const Vector2D& o) const {
		return x == o.x && y == o.y;
	}

	T cross(const Vector2D& o) const {
		return x*o.y - y*o.x;
	}
	T dot(const Vector2D& o) const {
		return x*o.x + y*o.y;
	}
	T magnitude2() const {
		return x*x + y*y;
	}
	T magnitude() const {
		return std::sqrt(magnitude2());
	}
	Vector2D normalize() const {
		const double mag = magnitude();
		return mag == 0 ? Vector2D(0,0) : (Vector2D(x,y) / mag );
	}

	template <typename U>
	Vector2D<U> convert(U (*conversionFunction)(T)) {
		return Vector2D<U> ( conversionFunction(x), conversionFunction(y));
	}

	T x, y;
};

template <typename T>
inline Vector2D<T> operator*(T t, const Vector2D<T>& v) { return v * t; }

#endif // _VECTOR2D_H