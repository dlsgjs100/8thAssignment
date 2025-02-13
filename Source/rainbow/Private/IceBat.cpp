#include "IceBat.h"

AIceBat::AIceBat()
{
	MoveSpeed = 100.f;
	RotationSpeed = 45.0f;
	SwitchInterval = 3.0f;
	TimeElapsed = 0.0f;
	bIsMovingForward = true;

	MaxHealth = 30.0f;
	Health = MaxHealth;
	Damage = 10.0f;
	
	PointValue = 10;
	ColorType = "Ice";
}