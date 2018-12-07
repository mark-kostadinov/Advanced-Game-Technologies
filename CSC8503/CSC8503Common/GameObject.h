#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "definitions.h"

#include <vector>

using std::vector;

namespace NCL
{
	namespace CSC8503
	{
		class NetworkObject;

		class GameObject
		{
		public:
			GameObject(string name = "");
			~GameObject();

			void SetBoundingVolume(CollisionVolume* vol) { boundingVolume = vol; UpdateBroadphaseAABB();}
			const CollisionVolume* GetBoundingVolume() const { return boundingVolume;}
			bool IsActive() const { return isActive; }
			const Transform& GetConstTransform() const { return transform; }
			Transform& GetTransform() { return transform; }
			RenderObject* GetRenderObject() const { return renderObject;}
			PhysicsObject* GetPhysicsObject() const { return physicsObject; }
			NetworkObject* GetNetworkObject() const { return networkObject; }
			void SetRenderObject(RenderObject* newObject) { renderObject = newObject; }
			void SetPhysicsObject(PhysicsObject* newObject) { physicsObject = newObject; }
			const string& GetName() const { return name; }

			virtual void OnCollisionBegin(GameObject* otherObject)
			{
#ifdef DEBUG_MODE
				//std::cout << "OnCollisionBegin event occured!\n";
				this->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
				otherObject->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
#endif DEBUG_MODE
			}

			virtual void OnCollisionEnd(GameObject* otherObject)
			{
#ifdef DEBUG_MODE
				//std::cout << "OnCollisionEnd event occured!\n";
				if (this->GetName() == "Sphere")
					this->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
				else
					this->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));

				if (otherObject->GetName() == "Sphere")
					otherObject->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));
				else
					otherObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
#endif DEBUG_MODE
			}

			bool InsideAABB(const Vector3& pos, const Vector3& halfSize);

			bool IsAlive() const { return isAlive; }
			void Kill() { isAlive = false; }
			void Revive() { isAlive = true; }

			bool GetBroadphaseAABB(Vector3 & outsize) const;
			void UpdateBroadphaseAABB();

		protected:
			Transform			transform;

			CollisionVolume*	boundingVolume;
			PhysicsObject*		physicsObject;
			RenderObject*		renderObject;
			NetworkObject*		networkObject;

			bool	isActive;
			bool	isAlive = false;
			string	name;

			Vector3 broadphaseAABB;
		};
	}
}

