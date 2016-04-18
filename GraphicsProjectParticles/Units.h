#ifndef _UNITS_H_
#define _UNITS_H_

#include <cmath>
#include "GL\glew.h"

namespace units {
	typedef float			Coordinate;
	typedef int				Pixel;
	typedef unsigned int	MS;
	typedef unsigned int	FPS;
	typedef float			Velocity;		// Coordinate / MS
	typedef float			Accelleration;	// Coordinate / MS / MS

	inline unsigned int getPowerOf2(const unsigned int exponent) {
		return 1 << exponent;
	}

	inline Pixel coordinateToPixel( Coordinate c ) { 
		return static_cast<Pixel>( c < 0.0 ? ceil(c - 0.5) : floor(c + 0.5)); 
	}

	inline Coordinate clampCoordinate( Coordinate val, Coordinate min, Coordinate max ) {
		return val < min ? min : val > max ? max : val;
	}
	inline float millisToSeconds( MS ms ) {
		return float(ms) / 1000.0f; 
	}
}

#endif // _UNITS_H_