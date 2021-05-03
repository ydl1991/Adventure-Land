#pragma once
#include <random>

enum class IntOrFloat : uint8_t
{
	kInt,
	kFloat,
};

enum class SignedOrUnsigned : uint8_t
{
	kSigned,
	kUnsigned,
};

class RandomNumberGenerator
{
	std::mt19937 m_engine;

	float m_lowerBound;
	float m_upperBound;
	float m_offset;
	IntOrFloat m_intOrFloat;
	SignedOrUnsigned m_signedOrUnsigned;

public:
	//---------------------------------------------------------------------------------
	// Random Number Generator Constructor
	// @ Param
	//		- lower range (inclusive)
	//		- upper range (inclusive)
	//		- flag that determins if it's a int RNG or float RNG
	//---------------------------------------------------------------------------------
	RandomNumberGenerator(float lower, float higher,
		IntOrFloat intOrFloat = IntOrFloat::kInt);

	~RandomNumberGenerator();

	//--------------------------------------
	// Generate Random Int Number
	//	if m_intOrFloat is kFloat, return 0
	//--------------------------------------
	int GenerateRandomInt();

	//--------------------------------------
	// Generate Random Float Number
	//	if m_intOrFloat is kInt, return 0.f
	//--------------------------------------
	float GenerateRandomFloat();
};

