#pragma once
#include "CollisionVolume.h"

namespace NCL
{
	class CapsuleVolume : CollisionVolume
	{
	public:
		CapsuleVolume(float capsuleHeight = 10.0f, float capsuleRadius = 1.0f)
		{
			type = VolumeType::Capsule;
			height = capsuleHeight;
			radius = capsuleRadius;
		}
		~CapsuleVolume() {}

		float GetHeight() const { return height; }
		float GetRadius() const { return radius; }

	protected:
		float height;
		float radius;
	};
}
