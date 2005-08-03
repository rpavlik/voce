/*************************************************************************
 *                                                                       *
 * Open Physics Abstraction Layer                                        *
 * Copyright (C) 2004-2005                                               *
 * Alan Fischer  alan.fischer@gmail.com                                  *
 * Andres Reinot  andres@reinot.com                                      *
 * Tyler Streeter  tylerstreeter@gmail.com                               *
 * All rights reserved.                                                  *
 * Web: opal.sourceforge.net                                             *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file license-LGPL.txt.                                          *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file license-BSD.txt.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * license-LGPL.txt and license-BSD.txt for more details.                *
 *                                                                       *
 *************************************************************************/

#ifndef OPAL_SAMPLES_PHYSICAL_CAMERA_H
#define OPAL_SAMPLES_PHYSICAL_CAMERA_H

#include <opal/opal.h>

// TODO: Make an option to switch from physical to non-physical and vice 
// versa on the fly.

namespace opalSamples
{
	//const opal::real graspDistanceScalar = (opal::real)5;
	//const opal::real maxReachScalar = (opal::real)8;

	/// Define how object grasping is handled.
	enum GraspMode
	{
		/// Grasped objects stay the same distance from the camera as 
		/// when they were initially grasped.  
		LONG_RANGE_GRASP_MODE,

		/// Grasped objects are pulled close to the camera.  Better for 
		/// first-person perspective.
		SHORT_RANGE_GRASP_MODE
	};

	/// A camera class that enables an Ogre Camera to be physical by 
	/// using an OPAL Solid.  The physical aspect can also be disabled, 
	/// allowing the camera to move freely through objects and not be 
	/// affected by gravity.  When the physics option is used, the 
	/// physical shape of the camera is a stack of two sphere, like a 
	/// snowman.
	class PhysicalCamera
	{
	public:
		/// Note: eye height and Solid radius are only used when the 
		/// physics option is enabled.
		PhysicalCamera(opal::Simulator* sim, Ogre::Camera* cam, 
			bool isPhysical, opal::real eyeHeight=2)
		{
			mIsPhysical = isPhysical;
			mGraspMode = LONG_RANGE_GRASP_MODE;
			mSimulator = sim;
			mSolid = NULL;
			mOgreCamera = cam;
			mGraspingMotor = sim->createSpringMotor();
			mGraspingSensor = sim->createRaycastSensor();
			mGraspedObject = NULL;
			mGraspOffset.set(0, 0, -5);
			mMaxReach = 500;
			mEyeHeight = eyeHeight;

			// Setup OPAL Solid, if necessary.
			if (isPhysical)
			{
				mSolid = sim->createSolid();
				mSolid->setPosition(0, eyeHeight, 0);

				// Keep the camera Solid from falling asleep.
				mSolid->setSleepiness(0);

				// Make the spheres' radii 1/4 * eye height.
				opal::real radius = eyeHeight * (opal::real)0.25;

				// Add a sphere at the eye/Solid position.
				opal::SphereShapeData sphereData;
				sphereData.radius = radius;
				mSolid->addShape(sphereData);

				// Add another sphere down below.
				sphereData.offset.translate(0, -3 * radius, 0);
				mSolid->addShape(sphereData);

				// Attach the ray Sensor to the Solid.
				opal::RaycastSensorData raySensorData;
				raySensorData.solid = mSolid;
				mGraspingSensor->init(raySensorData);
			}
		}

		~PhysicalCamera()
		{
			if (mIsPhysical)
			{
				assert(mSimulator && mGraspingMotor);
				mSimulator->destroySolid(mSolid);
			}

			mSimulator->destroyMotor(mGraspingMotor);
			mSimulator->destroySensor(mGraspingSensor);
		}

		/// Gives the camera a chance to update things regularly.
		void update(opal::real dt)
		{
			if (mIsPhysical)
			{
				// Keep solid upright by resetting its orientation.  Thus the 
				// camera Solid's orientation is always the same.
				opal::Point3r p = mSolid->getPosition();
				opal::Matrix44r m;
				m.translate(p[0], p[1], p[2]);
				mSolid->setTransform(m);

				// Set the Ogre camera's position.  It must be at the same 
				// position as the OPAL Solid.
				mOgreCamera->setPosition((Ogre::Real)p[0], (Ogre::Real)p[1], 
					(Ogre::Real)p[2]);
			}

			// Handle the grasped object if one exists.
			if (mGraspedObject)
			{
				// Keep the object awake.
				mGraspedObject->setSleeping(false);

				//// Set the Motor's global desired orientaiton for the Solid.
				//Ogre::Vector3 camForward = mOgreCamera->getDirection();
				//Ogre::Vector3 camUp = mOgreCamera->getUp();
				//Ogre::Vector3 camRight = mOgreCamera->getRight();
				//opal::Vec3r f(camForward[0], camForward[1], camForward[2]);
				//opal::Vec3r u(camUp[0], camUp[1], camUp[2]);
				//opal::Vec3r r(camRight[0], camRight[1], camRight[2]);
				//mGraspingMotor->setDesiredOrientation(f, u, r);

				// Set the Motor's global desired position for the Solid 
				// (at its attach position).
				mGraspingMotor->setDesiredPosition(getGraspGlobalPos());

				// Drop object if it gets too far away.
				if (opal::distance(getPosition(), mGraspedObject->getPosition())
					> mMaxReach)
				{
					release();
				}
			}
		}

		/// Returns a pointer to the Ogre Camera.
		Ogre::Camera* getOgreCamera()const
		{
			return mOgreCamera;
		}

		/// Returns a pointer to the Ogre Camera's parent SceneNode.
		Ogre::SceneNode* getOgreSceneNode()const
		{
			return mOgreCamera->getParentSceneNode();
		}

		/// Sets the position of the camera in global coordinates.  This 
		/// refers to the camera's eye position.
		void setPosition(const opal::Point3r& pos)
		{
			if (mIsPhysical)
			{
				assert(mSolid);
				mSolid->setPosition(pos);
			}
			else
			{
				mOgreCamera->setPosition(pos[0], pos[1], pos[2]);
			}
		}

		/// Returns the position of the camera in global coordinates.  This 
		/// refers to the camera's eye position.
		opal::Point3r getPosition()const
		{
			Ogre::Vector3 ogreCamPos = mOgreCamera->getPosition();
			return opal::Point3r(ogreCamPos[0], ogreCamPos[1], ogreCamPos[2]);
		}

		/// Orients the camera look at the given point.
		void lookAt(opal::Point3r point)
		{
			mOgreCamera->lookAt(point[0], point[1], point[2]);
		}

		/// Moves the camera according to the given direction vector 
		/// relative to its current transform.  This works differently 
		/// depending on whether the camera is physical.
		void moveRelative(const opal::Vec3r& dir)
		{
			if (mIsPhysical)
			{
				assert(mSolid);

				// TODO: finish this.

				//// don't let the camera fly
				////if (mSolid->getPosition()[1] > mEyeHeight + 0.0001)
				////{
				////	dir = DONT_MOVE;
				////}

				//Ogre::Vector3 vec;

				//switch(dir)
				//{
				//	case DONT_MOVE:
				//		vec = Ogre::Vector3::ZERO;
				//		break;
				//	case MOVE_LEFT:
				//		vec = -(mOgreCamera->getRight());
				//		break;
				//	case MOVE_RIGHT:
				//		vec = mOgreCamera->getRight();
				//		break;
				//	case MOVE_FORWARD:
				//		vec = mOgreCamera->getDirection();
				//		break;
				//	case MOVE_BACK:
				//		vec = -(mOgreCamera->getDirection());
				//		break;
				//}

				//// Don't let the camera fly.
				//if (vec[1] > 0)
				//{
				//	vec[1] = 0;
				//}

				//vec.normalise();
				//opal::Vec3r v(vec[0], vec[1], vec[2]);
				//v *= 3;
				//mSolid->setGlobalLinearVel(v);

				Ogre::Matrix4 ogreCamMat = 
					mOgreCamera->getViewMatrix().inverse();
				Ogre::Vector3 localDir(dir[0], dir[1], dir[2]);

				// Convert the local direction vector to a global direction 
				// vector.  Subtract out the camera's position.
				Ogre::Vector3 globalDir = ogreCamMat * localDir;
				globalDir -= mOgreCamera->getPosition();
				opal::Vec3r opalVec(globalDir[0], globalDir[1], globalDir[2]);
				mSolid->setGlobalLinearVel(opalVec);
			}
			else
			{
				Ogre::Vector3 ogreVec(dir[0], dir[1], dir[2]);
				mOgreCamera->moveRelative(ogreVec);
			}
		}

		/// Attempts to grasp an object by firing a ray into the scene 
		/// directly in front of the camera and choosing the closest 
		/// intersected object, if any.
		void grasp()
		{
			if (!mGraspedObject)
			{
				// Fire a ray into the scene to find an object to grasp.  If 
				// an object is found, attach it to the grasping Motor and, 
				// if using long range grasping mode, store the intersection 
				// position relative to the camera as the grasp offset.

				// First update the ray casting Sensor's ray.
				Ogre::Vector3 camForward = mOgreCamera->getDirection();
				if (0 != camForward.squaredLength())
				{
					camForward.normalise();
				}
				opal::Vec3r rayDir(camForward[0], camForward[1], 
					camForward[2]);
				if (mIsPhysical)
				{
					// The ray's origin will be updated automatically since 
					// it is attached to the camera's Solid.  Its direction 
					// should be set manually here because we constantly 
					// reset the camera's orientation.
					opal::Point3r dummyPoint;
					opal::Rayr r(dummyPoint, rayDir);
					mGraspingSensor->setRay(r);
				}
				else
				{
					// The ray should start at the camera's position and fire 
					// straight forward into the scene.
					Ogre::Vector3 ogreCamPos = mOgreCamera->getPosition();
					opal::Point3r opalCamPos(ogreCamPos[0], ogreCamPos[1], 
						ogreCamPos[2]);
					opal::Rayr r(opalCamPos, rayDir);
					mGraspingSensor->setRay(r);
				}

				// Fire the ray.
				opal::RaycastResult result = 
					mGraspingSensor->fireRay(mMaxReach);

				if (result.solid && !result.solid->isStatic())
				{
					// Store the grasped object.
					mGraspedObject = result.solid;

					// Initialize the grasping Motor with the new data.
					opal::SpringMotorData data;
					data.solid = result.solid;
					data.mode = opal::LINEAR_MODE;
					//data.mode = opal::LINEAR_AND_ANGULAR_MODE;
					data.linearKd = 2.5;
					data.linearKs = 25;
					//data.angularKd = 0.3;
					//data.angularKs = 5;
					opal::Matrix44r solidTransform = result.solid->getTransform();
					//data.desiredUp = solidTransform.getUp();
					//data.desiredForward = solidTransform.getForward();
					//data.desiredRight = solidTransform.getRight();
					// Desired position will be updated in the "update" 
					// function.
					mGraspingMotor->init(data);
					mGraspingMotor->setGlobalAttachPoint(result.intersection);

					if (LONG_RANGE_GRASP_MODE == mGraspMode)
					{
						mGraspOffset.set(0, 0, -result.distance);
					}

					result.solid->setAngularDamping(3);
				}
			}
		}

		/// Releases any grasped objects.
		void release()
		{
			if (mGraspedObject)
			{
				// Make sure the object is awake before releasing it so it 
				// doesn't float in midair.
				mGraspedObject->setAngularDamping(
					opal::defaults::solid::angularDamping);
				mGraspedObject->setSleeping(false);
				mGraspedObject = NULL;
				mGraspingMotor->setEnabled(false);
			}
		}

		/// Returns true if the camera is grasping an object.
		bool isGrasping()
		{
			if (mGraspedObject)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		/// Adjust the camera's pitch by adding an angle to its yaw 
		/// relative to its current yaw.
		void yawRelative(opal::real degrees)
		{
			// Update the Ogre camera.
			mOgreCamera->yaw(Ogre::Radian(Ogre::Degree(degrees)));
		}

		/// Adjust the camera's pitch by adding an angle to its pitch 
		/// relative to its current pitch.
		void pitchRelative(opal::real degrees)
		{
			// Update the Ogre camera.
			mOgreCamera->pitch(Ogre::Radian(Ogre::Degree(degrees)));
		}

		/// Returns true if the camera uses physics.
		bool isPhysical()const
		{
			return mIsPhysical;
		}

		/// Sets how object grasping is handled.
		void setGraspMode(GraspMode mode)
		{
			mGraspMode = mode;
		}

		/// Returns the grasp mode being used.
		GraspMode getGraspMode()const
		{
			return mGraspMode;
		}

		/// Sets the desired position for grasped objects, represented as a 
		/// local offset from the camera's transform.  This only applies 
		/// in the short-range grasp mode.  In long-range grasp mode the 
		/// offset is calculated automatically based on a ray cast 
		/// into the scene.
		void setGraspOffset(const opal::Vec3r& offset)
		{
			if (SHORT_RANGE_GRASP_MODE == mGraspMode)
			{
				mGraspOffset = offset;
			}
		}

		/// Returns the desired position for grasped objects relative to 
		/// the camera.
		const opal::Vec3r& getGraspOffset()const
		{
			return mGraspOffset;
		}

		/// Returns the position of the grasping spring's attach point 
		/// on the grasped object in global coordinates.  This is only 
		/// meaningful when an object is being grasped.
		opal::Point3r getAttachGlobalPos()const
		{
			return mGraspingMotor->getGlobalAttachPoint();
		}

		/// Returns the desired position for grasped objects in global 
		/// coordinates.
		opal::Point3r getGraspGlobalPos()const
		{
			Ogre::Matrix4 ogreMat = mOgreCamera->getViewMatrix().inverse();
			Ogre::Vector3 graspOffset(mGraspOffset[0], mGraspOffset[1], 
				mGraspOffset[2]);

			// Transform the offset vector (actually representing a point) 
			// to global space.
			graspOffset = ogreMat * graspOffset;

			return opal::Point3r(graspOffset[0], graspOffset[1], 
				graspOffset[2]);
		}

		//opal::Point3r getGlobalPosFromLocalOffset(opal::Vec3r localOffset)
		//{
		//	Ogre::Matrix4 ogreMat = mOgreCamera->getViewMatrix().inverse();
		//	//Ogre::Vector3 camPos = mOgreCamera->getPosition();
		//	Ogre::Vector3 offset(localOffset[0], localOffset[1], 
		//		localOffset[2]);

		//	// Transform the offset vector to global space.
		//	offset = ogreMat * offset;

		//	// Combine the camera position with the local offset vector.
		//	return opal::Point3r(offset[0], offset[1], offset[2]);
		//}

		/// Sets the maximum reach distance used for grasping.
		void setMaxReach(opal::real r)
		{
			mMaxReach = r;
		}

		/// Returns the maximum reach distance used for grasping.
		opal::real getMaxReach()const
		{
			return mMaxReach;
		}

	private:
		// Old stuff...
		//opal::Point3r calcGraspPos();
		//opal::Matrix44r mGraspOffset;

		//void getOpalMatFromOgreCam(opal::Matrix44r& mat)
		//{
		//	// Make an OPAL matrix copy of the Ogre camera's transform.
		//	Ogre::Matrix4 ogreMat = mOgreCamera->getViewMatrix();
		//	mat.set(
		//		ogreMat[0][0], ogreMat[1][0], ogreMat[2][0], ogreMat[3][0], 
		//		ogreMat[0][1], ogreMat[1][1], ogreMat[2][1], ogreMat[3][1], 
		//		ogreMat[0][2], ogreMat[1][2], ogreMat[2][2], ogreMat[3][2], 
		//		ogreMat[0][3], ogreMat[1][3], ogreMat[2][3], ogreMat[3][3]);
		//}

		/// True if the camera uses physics.
		bool mIsPhysical;

		/// Determines how object grasping is handled.
		GraspMode mGraspMode;

		/// Pointer to the OPAL Simulator.
		opal::Simulator* mSimulator;

		/// Pointer to the OPAL Solid.
		opal::Solid* mSolid;

		/// Pointer to the Ogre camera.
		Ogre::Camera* mOgreCamera;

		/// The OPAL ray casting Sensor used to find objects to grasp.
		opal::RaycastSensor* mGraspingSensor;

		/// The OPAL Motor used to hold grasped objects in a desired 
		/// position and orientation.
		opal::SpringMotor* mGraspingMotor;

		/// Pointer to the currently grasped object, if one exists.
		opal::Solid* mGraspedObject;

		/// The desired position for grasped objects in global coordinates.
		opal::Vec3r mGraspOffset;

		/// When the camera attempts to grasp an object by casting a ray, 
		/// this is the length of the ray.  It determines how far away 
		/// objects can be and still be grasped.
		opal::real mMaxReach;

		/// The distance between the ground and the camera viewpoint.
		opal::real mEyeHeight;
	};
}

#endif
