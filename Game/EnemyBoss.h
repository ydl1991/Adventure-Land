#pragma once
#include "Enemy.h"
#include "DtNode.h"
class AnimationComponent;
class StateMachine;
class RandomNumberGenerator;
class Thunder;
//--------------------------------------------------------------//
//  Boss Enemy Monster
//--------------------------------------------------------------//
class EnemyBoss : public Enemy
{
	bool m_isFullyPresent;						// Is it fully presented in the map
	float m_attackDuration;						// Timer for attack
	float m_mana;								// Mana for ability usage
	float m_manaRegen;							// Mana regeneration rate per tick

	DtNode* m_pAttackTreeRootNode;				// Attack decision tree start

	AnimationComponent* m_pManaRegenEffect;		// Restore mana animation
	bool m_isCharging;							// flag triggers mana regen effect

	AnimationComponent* m_pFireShieldEffect;	// Fire Shield animation

	AnimationComponent* m_pDeadAnimation;		// Dead Animation
	float m_deathAnimTimer;						// Time the death animation

	Thunder* m_pThunderAbility;					// Boss ability 1

public:
	EnemyBoss(ObjectSpawnInfo objectSpawnInfo);
	virtual ~EnemyBoss();

	virtual void Init() override;													// Init boss
	virtual void Tick(float deltaTime) override;									// Update loop
	virtual void HandleBeginOverlap(ObjectBase* pOtherCollider) override;			// Handle overlap reaction
	virtual void Render(SDL_Renderer* pRenderer) override;							// Render current state of the object

	// Getter Setter
	float GetMana() const { return m_mana; }										// Get current mana value
	Thunder* GetThunder() const { return m_pThunderAbility; }						// Get pointer of thunder
	void SetMana(float mana) { m_mana = mana; }										// Set mana value
	void SetManaRegen(float manaRe) { m_manaRegen = manaRe; }						// Setter mana regen rate
	void SetFullyPresent(bool fully) { m_isFullyPresent = fully; }					// Set presenting status

	// Attacking
	void ProcessAttack();															// Process Attack Event
	void AttackAbility();															// Ability to attack
	void DeadlyAbilityAttack();														// Attack under deadfury
	void ManaRestoreAbility();														// Ability to restore mana

	// Reset
	void ResetProperty();

	// Attack Event
	virtual void ProcessObjectAttack() override;

private:

	void CreateAndInitStateMachine();												// Create State Machine and Init it to Wander
	void CreateAndInitAllAnimation();												// Create and Init All Animations

	virtual void PositionUpdate(float deltaTime) override;							// Update position in the map
	virtual void ChangeDirection() override;										// Update new directions in the map

	void CheckStateSwitch();														// Switch object state
	void MoveOrAttack(float deltaTime);												// Either moving or attacking
	void ManaRegeneration();														// Regenerating mana

	void CountAttackingDuration(float deltaTime);									// Count Attacing Duration
	void ResetAttackDurationTimer() { m_attackDuration = 4.f; }						// Reset Attack Duration

	void BuildAbilityAttackTree();													// Build ability attack decision tree

	void CheckHealthLimitAndDeathAnimTrigger(float deltaTime);
};

