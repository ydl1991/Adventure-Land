#pragma once
class ObjectBase;

/***********************************************************/
// class that contains basic property and methods for all
// component type class
/***********************************************************/
class ComponentBase
{
public:
	// default constructor and destructor
	ComponentBase() {};
	~ComponentBase() {};

	// default virtual update function
	virtual void Update(float deltaTime) = 0;
};

