#include "Vector2D.h"
#include <math.h>
#include <iostream>

Vector2D::Vector2D(float x, float y)
	: m_x{x}
	, m_y{y}
	, m_rotation{ 0.f }
{
	//
}

Vector2D::~Vector2D()
{
}

// call to calculate dot product of 2 vectors
float Vector2D::GetDotProduct(Vector2D v1, Vector2D v2) const
{
	return v1.m_x * v2.m_x + v1.m_y * v2.m_y;
}

///////////////////////////////////////////////////////////////////////
// calculate magnitude under 2 different conditions
// 1. from WORLD origin (0,0) to THIS object
//		otherX = NULL, otherY = NULL
// 2. from OTHER object to THIS object
//		otherX = otherObject.x  otherY = otherObject.y
///////////////////////////////////////////////////////////////////////
float Vector2D::GetSqrMagnitude(int space, float otherX /* Null if world space */, float otherY /* Null if world space */)
{
	// world space
	if (space == 0)
	{
		//  calculate the sum of x^2 and y^2
		float squareSum = (m_x * m_x + m_y * m_y);
		// squareroot  the sum
		return squareSum;
	}
	
	// edge case if calculate magnitude at the same point
	if (otherX == m_x && otherY == m_y)
	{
		return 0.f;
	}

	// object space
	float xLength = otherX - m_x;
	float yLength = otherY - m_y;
	float squareSum = xLength * xLength + yLength * yLength;
	return squareSum;
}

///////////////////////////////////////////////////////////////////////
// calculate unit vector under 2 different conditions
// 1. from WORLD origin (0,0) to THIS object
//		otherX = NULL, otherY = NULL
// 2. from OTHER object to THIS object
//		otherX = otherObject.x  otherY = otherObject.y
///////////////////////////////////////////////////////////////////////
Vector2D Vector2D::NormalizeVector(int space, float otherX /* Null if world space */, float otherY /* Null if world space */)
{
	// world space
	if (space == 0)
	{
		// calculate magnitude
		float sqrMagnitude = GetSqrMagnitude(space);
		// calculate unit vector
		float x = (float)(m_x / sqrt(sqrMagnitude));
		float y = (float)(m_y / sqrt(sqrMagnitude));
		return Vector2D(x, y);
	}

	// object space
	// calculate x, y differences between 2 vector2D
	float xLength = otherX - m_x;
	float yLength = otherY - m_y;
	// calculate magnitude
	float sqrMagnitude = GetSqrMagnitude(space, otherX, otherY);
	if (sqrMagnitude == 0)
	{
		return Vector2D{ 0,0 };
	}
	// normalize the vector and store the direction in m_motion
	float x = (xLength / sqrt(sqrMagnitude));
	float y = (yLength / sqrt(sqrMagnitude));
	return Vector2D(x, y);

}

void Vector2D::ScalarMultiply(float scalar)
{
	m_x *= scalar;
	m_y *= scalar;
}

void Vector2D::Add(Vector2D otherVector)
{
	m_x += otherVector.m_x;
	m_y += otherVector.m_y;
}

// call to calculate rotation angle
void Vector2D::Rotate(float otherX, float otherY)
{
	// unit vector of my sprite facing direction
	Vector2D north{ 0.f, -1.f };
	// unit vector of the other object to this object
	Vector2D dir{ otherX , otherY};
	// calculate rotation
	float cosinTheta = GetDotProduct(north, dir);
	float angle = ((acos(cosinTheta) * 180.f) / 3.141592653589f);
	if (dir.m_x < 0)
	{
		angle = 360 - angle;
	}
	m_rotation = angle;
} 

void Vector2D::Lerp()
{
}
