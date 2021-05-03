#pragma once

/***********************************************************/
// This class represents the coordinates system for any
// object exist inside the game world. It allows doing calculation
// for properties in both Object Space and World Space
/***********************************************************/
class Vector2D
{
public:
	// coordinates variable
	float m_x;
	float m_y;
	float m_rotation;

public:
	Vector2D() = default;
	Vector2D(float x, float y);
	~Vector2D();

	// property calculation
	Vector2D NormalizeVector(int space, float otherX = 0, float otherY = 0);
	float GetSqrMagnitude(int space, float otherX = 0, float otherY = 0);
	float GetDotProduct(Vector2D v1, Vector2D v2) const;
	void Rotate(float otherX, float otherY);

	// modifying vector
	void ScalarMultiply(float scalar);
	void Add(Vector2D otherVector);
	void Lerp();
};

