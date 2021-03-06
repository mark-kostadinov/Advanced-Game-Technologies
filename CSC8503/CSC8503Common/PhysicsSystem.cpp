#include "PhysicsSystem.h"
#include "PhysicsObject.h"
#include "GameObject.h"
#include "CollisionDetection.h"
#include "../../Common/Quaternion.h"

#include "Constraint.h"

#include "Debug.h"

#include <functional>
using namespace NCL;
using namespace CSC8503;

/*
These two variables help define the relationship between positions
and the forces that are added to objects to change those positions
*/

const float PhysicsSystem::UNIT_MULTIPLIER = 1.0f;
const float PhysicsSystem::UNIT_RECIPROCAL = 1.0f / UNIT_MULTIPLIER;

PhysicsSystem::PhysicsSystem(GameWorld& g) : gameWorld(g)
{
	applyGravity = false;
	useBroadPhase = true;
	dTOffset = 0.0f;
	globalDamping = 0.95f;
	SetGravity(Vector3(0.0f, -9.8f * 100.0f, 0.0f));
}

PhysicsSystem::~PhysicsSystem()
{
}

void PhysicsSystem::SetGravity(const Vector3& g)
{
	gravity = g * UNIT_MULTIPLIER;
}

/*
If the 'game' is ever reset, the PhysicsSystem must be
'cleared' to remove any old collisions that might still
be hanging around in the collision list. If your engine
is expanded to allow objects to be removed from the world,
you'll need to iterate through this collisions list to remove
any collisions they are in.
*/
void PhysicsSystem::Clear()
{
	allCollisions.clear();
}

/*
This is the core of the physics engine update
*/
void PhysicsSystem::Update(float dt)
{
	const float iterationDt = 1.0f / 120.0f; // Ideally we'll have 120 physics updates a second 
	dTOffset += dt; // We accumulate time delta here - there might be remainders from previous frame!

	int iterationCount = (int)(dTOffset / iterationDt); // And split it up here

	float subDt = dt / (float)iterationCount; // How many seconds per iteration do we get?

	IntegrateAccel(dt); // Update accelerations from external forces

	for (int i = 0; i < iterationCount; ++i)
	{
		if (useBroadPhase)
		{
			//UpdateObjectAABBs();
			BroadPhase();
			NarrowPhase();
		}
		else
		{
			BasicCollisionDetection();
		}

		//This is our simple iterative solver - 
		//we just run things multiple times, slowly moving things forward
		//and then rechecking that the constraints have been met

		int constraintIterationCount = 10;
		float constraintDt = subDt / (float)constraintIterationCount;

		for (int i = 0; i < constraintIterationCount; ++i)
		{
			UpdateConstraints(constraintDt);
			IntegrateVelocity(constraintDt); //update positions from new velocity changes
		}
		dTOffset -= iterationDt;
	}
	ClearForces();	//Once we've finished with the forces, reset them to zero

	UpdateCollisionList(); //Remove any old collisions
}

/*
Later on we're going to need to keep track of collisions
across multiple frames, so we store them in a set.

The first time they are added, we tell the objects they are colliding.
The frame they are to be removed, we tell them they're no longer colliding.

From this simple mechanism, we we build up gameplay interactions inside the
OnCollisionBegin / OnCollisionEnd functions (removing health when hit by a
rocket launcher, gaining a point when the player hits the gold coin, and so on).
*/
void PhysicsSystem::UpdateCollisionList()
{
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = allCollisions.begin(); i != allCollisions.end();)
	{
		if ((*i).framesLeft == numCollisionFrames)
		{
			i->a->OnCollisionBegin(i->b);
			i->b->OnCollisionBegin(i->a);
		}
		(*i).framesLeft = (*i).framesLeft - 1;

		if ((*i).framesLeft < 0)
		{
			i->a->OnCollisionEnd(i->b);
			i->b->OnCollisionEnd(i->a);
			i = allCollisions.erase(i);
		}
		else
			++i;
	}
}

void PhysicsSystem::UpdateObjectAABBs()
{
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i)
		(*i)->UpdateBroadphaseAABB();
}

/*
This is how we'll be doing collision detection in tutorial 4.
We step thorugh every pair of objects once (the inner for loop offset
ensures this), and determine whether they collide, and if so, add them
to the collision set for later processing. The set will guarantee that
a particular pair will only be added once, so objects colliding for
multiple frames won't flood the set with duplicates.
*/
void PhysicsSystem::BasicCollisionDetection()
{
	vector<GameObject*>::const_iterator first;
	vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);

	for (auto it = first; it != last; ++it)
	{
		if ((*it)->GetPhysicsObject() == nullptr)
			continue;

		for (auto jt = it + 1; jt != last; ++jt)
		{
			if ((*jt)->GetPhysicsObject() == nullptr)
				continue;

			// No need to check whether immovable objects collide with each other
			if ((*it)->GetPhysicsObject()->GetInverseMass() == 0 && (*jt)->GetPhysicsObject()->GetInverseMass() == 0)
				continue;

			CollisionDetection::CollisionInfo info;
			if (CollisionDetection::ObjectIntersection(*it, *jt, info))
			{
#ifdef DEBUG_MODE
				std::cout << "Collision between " << (*it)->GetName() << " and " << (*jt)->GetName() << std::endl;
#endif // DEBUG_MODE
				ImpulseResolveCollision(*info.a, *info.b, info.point);
				info.framesLeft = numCollisionFrames;
				allCollisions.insert(info);
			}
		}
	}
}

/*
In tutorial 5, we start determining the correct response to a collision,
so that objects separate back out.
*/
void PhysicsSystem::ImpulseResolveCollision(GameObject& a, GameObject& b, CollisionDetection::ContactPoint& p) const
{
	PhysicsObject * physA = a.GetPhysicsObject();
	PhysicsObject * physB = b.GetPhysicsObject();

	Transform & transformA = a.GetTransform();
	Transform & transformB = b.GetTransform();

	float totalMass = physA->GetInverseMass() + physB->GetInverseMass();

	if (totalMass == 0.0f) // Make sure two infinitely massful objects touching don't explode the impulse
		return;

	// Separate them out using projection
	transformA.SetWorldPosition(transformA.GetWorldPosition() - (p.normal * p.penetration * (physA->GetInverseMass() / totalMass)));
	transformB.SetWorldPosition(transformB.GetWorldPosition() + (p.normal * p.penetration * (physB->GetInverseMass() / totalMass)));

	Vector3 relativeA = p.position - transformA.GetWorldPosition();
	Vector3 relativeB = p.position - transformB.GetWorldPosition();

	Vector3 angVelocityA = Vector3::Cross(physA->GetAngularVelocity(), relativeA);
	Vector3 angVelocityB = Vector3::Cross(physB->GetAngularVelocity(), relativeB);

	Vector3 fullVelocityA = physA->GetLinearVelocity() + angVelocityA;
	Vector3 fullVelocityB = physB->GetLinearVelocity() + angVelocityB;

	Vector3 contactVelocity = fullVelocityB - fullVelocityA;

	float impulseForce = Vector3::Dot(contactVelocity, p.normal);

	if (impulseForce > 0) // Make sure impulses don't cause objects to come together instead of apart
		return;

	// Now to work out the effect of inertia...
	Vector3 inertiaA = Vector3::Cross(physA->GetInertiaTensor() * Vector3::Cross(relativeA, p.normal), relativeA);
	Vector3 inertiaB = Vector3::Cross(physB->GetInertiaTensor() * Vector3::Cross(relativeB, p.normal), relativeB);

	float angularEffect = Vector3::Dot(inertiaA + inertiaB, p.normal);
	float restitutionCoeff = a.GetPhysicsObject()->GetElasticty() * b.GetPhysicsObject()->GetElasticty(); // Disperse some kinectic energy
	float j = (-(1.0f + restitutionCoeff) * impulseForce) / (totalMass + angularEffect);

	Vector3 fullImpulse = p.normal * j;

	physA->ApplyLinearImpulse(-fullImpulse);
	physB->ApplyLinearImpulse(fullImpulse);

	if (a.GetName() == "Moving Obstacle" || b.GetName() == "Moving Obstacle") // Prevent the moving platforms from rotating
		return;

	if (a.GetName() == "Robot" || b.GetName() == "Robot") // Prevent the Robot from rotating too
		return;

	physA->ApplyAngularImpulse(Vector3::Cross(relativeA, -fullImpulse));
	physB->ApplyAngularImpulse(Vector3::Cross(relativeB, fullImpulse));
}

/*
Later, we replace the BasicCollisionDetection method with a broadphase
and a narrowphase collision detection method. In the broad phase, we
split the world up using an acceleration structure, so that we can only
compare the collisions that we absolutely need to.
*/
void PhysicsSystem::BroadPhase()
{
	broadphaseCollisions.clear();
	QuadTree<GameObject*> tree(Vector2(2048, 2048), 7, 10);
	
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);

	for (auto it = first; it != last; ++it)
	{
		Vector3 halfSizes;
		if (!(*it)->GetBroadphaseAABB(halfSizes))
			continue;

		Vector3 pos = (*it)->GetConstTransform().GetWorldPosition();
		tree.Insert(*it, pos, halfSizes);
	}

	if (renderQuadTreeMode)
		tree.DebugDraw();

	tree.OperateOnContents([&](std::list<QuadTreeEntry<GameObject*>> &data)
	{
		CollisionDetection::CollisionInfo info;
		
		for (auto it = data.begin(); it != data.end(); ++it)
		{
			for (auto jt = std::next(it); jt != data.end(); ++jt)
			{
				// No need to check whether immovable objects collide with each other
				if ((*it).object->GetPhysicsObject()->GetInverseMass() == 0 && (*jt).object->GetPhysicsObject()->GetInverseMass() == 0)
					continue;

				// Is this pair of items already in the collision set -
				// if the same pair is in another quadtree node together, etc.
				info.a = min((*it).object, (*jt).object);
				info.b = max((*it).object, (*jt).object);

				broadphaseCollisions.insert(info);
			}
		}
	});
}

/*
The broadphase will now only give us likely collisions, so we can now go through them,
and work out if they are truly colliding, and if so, add them into the main collision list
*/
void PhysicsSystem::NarrowPhase()
{
	for (std::set<CollisionDetection::CollisionInfo>::iterator i = broadphaseCollisions.begin(); i != broadphaseCollisions.end(); ++i)
	{
		CollisionDetection::CollisionInfo info = *i;

		//std::cout << "Testing collision between " << info.a->GetName() << " and " << info.b->GetName() << std::endl;

		if (CollisionDetection::ObjectIntersection(info.a, info.b, info))
		{
			info.framesLeft = numCollisionFrames;
			ImpulseResolveCollision(*info.a, *info.b, info.point);
			allCollisions.insert(info); // Insert into our main set
		}
	}
}

/*
Integration of acceleration and velocity is split up, so that we can
move objects multiple times during the course of a PhysicsUpdate,
without worrying about repeated forces accumulating etc.

This function will update both linear and angular acceleration,
based on any forces that have been accumulated in the objects during
the course of the previous game frame.
*/
void PhysicsSystem::IntegrateAccel(float dt)
{
	vector<GameObject*>::const_iterator first;
	vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);

	for (auto it = first; it != last; ++it)
	{
		PhysicsObject* object = (*it)->GetPhysicsObject();
		if (object == nullptr)
			continue;

		float inverseMass = object->GetInverseMass();

		Vector3 linearVel = object->GetLinearVelocity();
		Vector3 force = object->GetForce();
		Vector3 accel = force * inverseMass;

		if (applyGravity && inverseMass > 0)
			accel += gravity; // Don't move infinitely heavy things

		linearVel += accel * dt; // Integrate linear acceleration!
		object->SetLinearVelocity(linearVel);

		// Angular stuff
		Vector3 torque = object->GetTorque();
		Vector3 angVel = object->GetAngularVelocity();

		object->UpdateInertiaTensor(); // Update tensor vs orientation

		Vector3 angAccel = object->GetInertiaTensor() * torque;

		angVel += angAccel * dt; // Integrate angular acceleration!
		object->SetAngularVelocity(angVel);
	}
}

/*
This function integrates linear and angular velocity into
position and orientation. It may be called multiple times
throughout a physics update, to slowly move the objects through
the world, looking for collisions.
*/
void PhysicsSystem::IntegrateVelocity(float dt)
{
	vector<GameObject*>::const_iterator first;
	vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);
	float dampingFactor;
	float frameDamping;

	for (auto it = first; it != last; ++it)
	{
		PhysicsObject * object = (*it)->GetPhysicsObject();
		if (object == nullptr)
			continue;

		if ((*it)->GetName() == "Player")
			dampingFactor = object->GetFriction();
		else if ((*it)->GetName() == "Robot")
			dampingFactor = object->GetFriction();
		else
			dampingFactor = 0.95f;
		frameDamping = powf(dampingFactor, dt);

		Transform & transform = (*it)->GetTransform();
		// Position Stuff
		Vector3 position = transform.GetLocalPosition();
		Vector3 linearVel = object->GetLinearVelocity();
		position += linearVel * dt;
		transform.SetLocalPosition(position);
		transform.SetWorldPosition(position);
		// Linear Damping
		linearVel = linearVel * frameDamping;
		object->SetLinearVelocity(linearVel);

		// Orientation Stuff
		Quaternion orientation = transform.GetLocalOrientation();
		Vector3 angVel = object->GetAngularVelocity();

		orientation = orientation + (Quaternion(angVel * dt * 0.5f, 0.0f) * orientation);
		orientation.Normalise();

		transform.SetLocalOrientation(orientation);

		// Damp the angular velocity too
		angVel = angVel * frameDamping;
		object->SetAngularVelocity(angVel);
	}
}

/*
Once we're finished with a physics update, we have to
clear out any accumulated forces, ready to receive new
ones in the next 'game' frame.
*/
void PhysicsSystem::ClearForces()
{
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i)
	{
		//Clear our object's forces for the next frame
		(*i)->GetPhysicsObject()->ClearForces();
	}
}

/*
As part of the final physics tutorials, we add in the ability
to constrain objects based on some extra calculation, allowing
us to model springs and ropes etc.
*/
void PhysicsSystem::UpdateConstraints(float dt)
{
	std::vector<Constraint*>::const_iterator first;
	std::vector<Constraint*>::const_iterator last;
	gameWorld.GetConstraintIterators(first, last);

	for (auto i = first; i != last; ++i)
	{
		(*i)->UpdateConstraint(dt);
	}
}