#include "RedBat.h"

ARedBat::ARedBat()
{
	MoveSpeed = 110.f;
	RotationSpeed = 90.0f;
	SwitchInterval = 1.5f;
	TimeElapsed = 0.0f;
	bIsMovingForward = true;

	MaxHealth = 300.0f;
	Health = MaxHealth;
	Damage = 10.0f;
	
	PointValue = 10;
	ColorType = "Red";
}
