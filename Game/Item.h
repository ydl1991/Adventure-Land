#pragma once
#include "ObjectBase.h"
class ParticleEmitter;

class Item : public ObjectBase
{
public:
	enum class ItemType
	{
		kWeaponPowerUp = 0,
		kClearOutScreen,
	};
protected:
	ParticleEmitter* m_pParticleEmitter;
	ItemType m_type;

public:
	Item(ItemType type);
	~Item() {};

	// virtual override
	void Init() override;
	void Tick(float deltaTime) override;
	void HandleBeginOverlap(ObjectBase* pOtherCollider) override;
	void Render(SDL_Renderer* pRenderer) override;

	// Type Getter
	ItemType GetItemType() const { return m_type; }

protected:
	void UpdatePosition(float deltaTime);
};

