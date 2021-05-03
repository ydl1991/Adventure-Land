#include "RandomNumberGenerator.h"

//---------------------------------------------------------------------------------
// Random Number Generator Constructor
// @ Param
//		- lower range (inclusive)
//		- upper range (inclusive)
//		- flag that determins if it's a int RNG or float RNG
//			if float is chosen, only gonna generate 3 decimal places
//---------------------------------------------------------------------------------
RandomNumberGenerator::RandomNumberGenerator(float lower, float higher, IntOrFloat intOrFloat)
	: m_intOrFloat{ intOrFloat }
	, m_signedOrUnsigned{ SignedOrUnsigned::kUnsigned }
	, m_offset{ 0 }
	, m_lowerBound{ lower }
	, m_upperBound{ higher }
{
	// if asking for float, multiply by 1000 first to get the 3 decimal places
	if (intOrFloat == IntOrFloat::kFloat)
	{
		m_lowerBound = lower * 1000;
		m_upperBound = higher * 1000;
	}

	// if signed, set offset value and reset lower and upper bound
	if (m_lowerBound < 0)
	{
		m_signedOrUnsigned = SignedOrUnsigned::kSigned;
		m_offset = 0 - m_lowerBound;
		m_lowerBound = 0;
		m_upperBound += m_offset;
	}

	std::random_device randomDevice;
	// seed random
	unsigned int seed = randomDevice();
	m_engine.seed(seed);
}

RandomNumberGenerator::~RandomNumberGenerator()
{
	//
}

//--------------------------------------
// Generate Random Int Number
//	if m_intOrFloat is kFloat, return 0
//--------------------------------------
int RandomNumberGenerator::GenerateRandomInt()
{
	if (m_intOrFloat == IntOrFloat::kFloat)
		return 0;
	else
	{
		std::uniform_int_distribution<unsigned int> distribution((int)m_lowerBound, (int)m_upperBound);
		if (m_signedOrUnsigned == SignedOrUnsigned::kSigned)
		{
			int randomNumber = distribution(m_engine);
			return randomNumber - (int)m_offset;
		}
		else
		{
			int randomNumber = distribution(m_engine);
			return randomNumber;
		}
	}
}

//--------------------------------------
// Generate Random Float Number
//	if m_intOrFloat is kInt, return 0.f
//--------------------------------------
float RandomNumberGenerator::GenerateRandomFloat()
{
	if (m_intOrFloat == IntOrFloat::kInt)
		return 0.0f;
	else
	{
		std::uniform_int_distribution<unsigned int> distribution((int)m_lowerBound, (int)m_upperBound);
		if (m_signedOrUnsigned == SignedOrUnsigned::kSigned)
		{
			int randomNumber = distribution(m_engine);
			return (randomNumber - m_offset) / 1000.f;
		}
		else
		{
			int randomNumber = distribution(m_engine);
			return randomNumber / 1000.f;
		}
	}
}
