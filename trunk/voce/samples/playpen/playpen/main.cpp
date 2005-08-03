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

#include <time.h>
#include "../src/BaseOpalApp.h"
#include <voce/voce.h>

namespace playpen
{
	enum ObjectType
	{
		OBJECT_TYPE_BOX,
		OBJECT_TYPE_SPHERE,
		OBJECT_TYPE_WALL,
		OBJECT_TYPE_TOWER,
		OBJECT_TYPE_RAGDOLL
	};

	class PlaypenApp : public opalSamples::BaseOpalApp
	{
	public:
		PlaypenApp();

		~PlaypenApp();

	protected:
		virtual void createScene();

		virtual bool appFrameStarted(opal::real dt);

		virtual bool processUnbufferedKeyInput(Ogre::Real dt);

		virtual bool processUnbufferedMouseInput(Ogre::Real dt);

		/// Creates the initial objects in the world.
		void setupInitialPhysicalEntities();

		/// Create an object using the given material.
		void createObject(const std::string& material, ObjectType type);

		/// Builds a wall of boxes centered at the origin.
		void createWall(unsigned int length, unsigned height, 
			const opal::Vec3r& boxDim, 
			const opal::Matrix44r& baseTransform=opal::Matrix44r(), 
			const std::string& material="");

		/// Builds a tower of boxes centered at the origin.  For best 
		/// results, make box length = 2 * box width to helps the 
		/// corners line up correctly.
		void createTower(unsigned int width, unsigned int depth, 
			unsigned height, const opal::Vec3r& boxDim, 
			const opal::Matrix44r& baseTransform=opal::Matrix44r(), 
			const std::string& material="");

	private:
		/// Used to toggle shadows on and off.
		bool mUseShadows;

		/// Point where new objects are created.
		opal::Point3r mCreationPoint;
	};

	PlaypenApp::PlaypenApp()
	: BaseOpalApp()
	{
		mUseShadows = true;
		mCreationPoint.set(-55, 60, -55);
		voce::init("../../../../lib", true, true, "../../data/grammar", 
			"objects");
	}

	PlaypenApp::~PlaypenApp()
	{
		voce::destroy();
	}

	void PlaypenApp::createScene()
	{
		// Setup shadows.
		if (mUseShadows)
		{
			mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
		}
		else
		{
			mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
		}

		//mSceneMgr->showBoundingBoxes(true);
		//mSimulator->setStepSize(0.02);
		mSimulator->setGravity(opal::Vec3r(0, -30, 0));

		// Set the ambient light level.
		mSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));

		// Create a light source.
		Ogre::Light* light = mSceneMgr->createLight("light0");
		light->setType(Ogre::Light::LT_POINT);
		light->setDiffuseColour(0.5, 0.5, 0.5);
		light->setSpecularColour(0.5, 0.5, 0.5);
		light->setPosition(100.0, 300.0, 100.0);

		// Create another light source.
		light = mSceneMgr->createLight("light1");
		light->setType(Ogre::Light::LT_POINT);
		light->setDiffuseColour(0.5, 0.5, 0.5);
		light->setSpecularColour(0.5, 0.5, 0.5);
		light->setPosition(-100.0, 300.0, 100.0);
		light->setVisible(false);

		// Setup the initial camera position.
		mPhysicalCamera->setPosition(opal::Point3r(0, 20, 30));
		mPhysicalCamera->lookAt(opal::Point3r(0, 0, 0));

		// Create a skybox.
		mSceneMgr->setSkyBox(true, "Skyboxes/Gray", 5000);

		// Make a sphere used to show the grasp position.
		Ogre::SceneNode* sphereNode = mSceneMgr->getRootSceneNode()->
			createChildSceneNode("pickingSphere0");
		sphereNode->scale(0.3, 0.3, 0.3);
		Entity* sphereEntity = mSceneMgr->createEntity("pickingSphere", 
			"sphere.mesh");
		sphereEntity->setMaterialName("Unlit/Yellow");
		sphereEntity->setCastShadows(false);
		sphereEntity->setVisible(false);
		sphereNode->attachObject(sphereEntity);

		// Make a "line" (scaled cylinder) to show the grasping spring.
		Ogre::SceneNode* lineNode = mSceneMgr->getRootSceneNode()->
			createChildSceneNode("springLine");
		Entity* lineEntity = mSceneMgr->createEntity("springLine", 
			"cylinder.mesh");
		lineEntity->setMaterialName("Unlit/White");
		lineEntity->setCastShadows(false);
		lineEntity->setVisible(false);
		lineNode->attachObject(lineEntity);

		// Make a crosshairs for picking.
		Ogre::Overlay* overlay = 
			OverlayManager::getSingleton().getByName("General/Crosshair");
		overlay->show();

		// Load models, create physical objects, etc. here.

		setupInitialPhysicalEntities();
	}

	bool PlaypenApp::appFrameStarted(opal::real dt)
	{
		// Do per-frame application-specific things here.

		// Handle speech input.
		bool keepLooping = true;
		bool makeObject = false;
		std::string material;
		ObjectType type;
		std::string outputString;
		while (voce::getRecognizerQueueSize() > 0)
		{
			std::string s = voce::popRecognizedString();

			//// Check if the string contains 'quit'.
			//if (std::string::npos != s.rfind("quit"))
			//{
			//	keepLooping = false;
			//}

			// Check if we should reset.
			if (std::string::npos != s.rfind("reset"))
			{
				// Make sure the PhysicalCamera isn't grabbing anything.
				mPhysicalCamera->release();
				destroyAllPhysicalEntities();
				setupInitialPhysicalEntities();
				voce::synthesize("reset");
				return true;
			}

			// Check for material color.
			if (std::string::npos != s.rfind("yellow"))
			{
				outputString += "yellow";
				material = "Plastic/Yellow";
			}
			else if (std::string::npos != s.rfind("red"))
			{
				outputString += "red";
				material = "Plastic/Red";
			}
			else if (std::string::npos != s.rfind("blue"))
			{
				outputString += "blue";
				material = "Plastic/Blue";
			}
			else if (std::string::npos != s.rfind("green"))
			{
				outputString += "green";
				material = "Plastic/Green";
			}
			else if (std::string::npos != s.rfind("purple"))
			{
				outputString += "purple";
				material = "Plastic/Purple";
			}
			else if (std::string::npos != s.rfind("orange"))
			{
				outputString += "orange";
				material = "Plastic/Orange";
			}
			else
			{
				// Default to dark gray.
				material = "Plastic/DarkGray";
			}

			// Check for object type.
			if (std::string::npos != s.rfind("box"))
			{
				outputString += " box";
				type = OBJECT_TYPE_BOX;
				makeObject = true;
			}
			else if (std::string::npos != s.rfind("sphere"))
			{
				outputString += " sphere";
				type = OBJECT_TYPE_SPHERE;
				makeObject = true;
			}
			else if (std::string::npos != s.rfind("wall"))
			{
				outputString += " wall";
				type = OBJECT_TYPE_WALL;
				makeObject = true;
			}
			else if (std::string::npos != s.rfind("tower"))
			{
				outputString += " tower";
				type = OBJECT_TYPE_TOWER;
				makeObject = true;
			}
			else if (std::string::npos != s.rfind("character"))
			{
				outputString += " character";
				type = OBJECT_TYPE_RAGDOLL;
				makeObject = true;
			}

			if (makeObject)
			{
				voce::synthesize(outputString);
				createObject(material, type);
			}
		}

		// Update the grasping spring line.
		if (mPhysicalCamera->isGrasping())
		{
			Ogre::Entity* pickingSphere = 
				mSceneMgr->getEntity("pickingSphere");
			if (!pickingSphere->isVisible())
			{
				pickingSphere->setVisible(true);
			}

			Ogre::Entity* springLine = 
				mSceneMgr->getEntity("springLine");
			if (!springLine->isVisible())
			{
				springLine->setVisible(true);
			}

			opal::Point3r desiredPos = 
				mPhysicalCamera->getGraspGlobalPos();
			Ogre::Vector3 point0(desiredPos[0], desiredPos[1], desiredPos[2]);

			opal::Point3r attachPos = mPhysicalCamera->getAttachGlobalPos();
			Ogre::Vector3 point1(attachPos[0], attachPos[1], attachPos[2]);

			pickingSphere->getParentSceneNode()->setPosition(point1);

			Ogre::Vector3 lineVec = point0 - point1;
			if (!lineVec.isZeroLength())
			{
				Ogre::SceneNode* springLineNode = 
					springLine->getParentSceneNode();
				springLineNode->setPosition(0.5 * (point0 + point1));
				
				springLineNode->setDirection(lineVec);
				springLineNode->setScale(0.1, 0.1, lineVec.length());
			}
			else
			{
				springLine->setVisible(false);
			}
		}
		else
		{
			Ogre::Entity* pickingSphere = 
				mSceneMgr->getEntity("pickingSphere");
			if (pickingSphere->isVisible())
			{
				pickingSphere->setVisible(false);
			}

			Ogre::Entity* springLine = 
				mSceneMgr->getEntity("springLine");
			if (springLine->isVisible())
			{
				springLine->setVisible(false);
			}
		}

		// Return true to continue looping.
		return keepLooping;
	}

	bool PlaypenApp::processUnbufferedKeyInput(Ogre::Real dt)
	{
		// Check if we should quit looping.
		if(mInputDevice->isKeyDown(KC_ESCAPE) 
			|| mInputDevice->isKeyDown(KC_Q))
		{
			return false;
		}

		// Check if we should pause physics.
		if(mInputDevice->isKeyDown(KC_P) && mTimeUntilNextToggle <= 0)
		{
			mPaused = !mPaused;

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.5;
		}

		// Reset the scene.
		if(mInputDevice->isKeyDown(KC_R))
		{
			// Make sure the PhysicalCamera isn't grabbing anything.
			mPhysicalCamera->release();
			destroyAllPhysicalEntities();
			setupInitialPhysicalEntities();
		}

		// The following code updates the camera's position.
		opal::Vec3r cameraDir(0, 0, 0);
		bool cameraMoved = false;

		if (mInputDevice->isKeyDown(KC_LEFT) 
			|| mInputDevice->isKeyDown(KC_A))
		{
			// Move camera left.
			cameraDir[0] -= (dt * mMoveSpeed);
			cameraMoved = true;
		}

		if (mInputDevice->isKeyDown(KC_RIGHT) 
			|| mInputDevice->isKeyDown(KC_D))
		{
			// Move camera right.
			cameraDir[0] += (dt * mMoveSpeed);
			cameraMoved = true;
		}

		if (mInputDevice->isKeyDown(KC_UP) 
			|| mInputDevice->isKeyDown(KC_W))
		{
			// Move camera forward.
			cameraDir[2] -= (dt * mMoveSpeed);
			cameraMoved = true;
		}

		if (mInputDevice->isKeyDown(KC_DOWN) 
			|| mInputDevice->isKeyDown(KC_S))
		{
			// Move camera backward.
			cameraDir[2] += (dt * mMoveSpeed);
			cameraMoved = true;
		}

		if (!cameraMoved)
		{
			// Slow physical camera motion if necessary.
		}

		// Use the camera dir vector to translate the camera.
		mPhysicalCamera->moveRelative(cameraDir);

		// Toggle shadows.
		if(mInputDevice->isKeyDown(KC_H) && mTimeUntilNextToggle <= 0)
		{
			mUseShadows = !mUseShadows;

			if (mUseShadows)
			{
				mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
			}
			else
			{
				mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
			}

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.5;
		}

		// Toggle second light source.
		if(mInputDevice->isKeyDown(KC_L) && mTimeUntilNextToggle <= 0)
		{
			Ogre::Light* light1 = mSceneMgr->getLight("light1");

			if (light1->isVisible())
			{
				light1->setVisible(false);
			}
			else
			{
				light1->setVisible(true);
			}

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.5;
		}

		// Toggle GUI.
		if (mInputDevice->isKeyDown(KC_G) && mTimeUntilNextToggle <= 0)
		{
			mStatsOn = !mStatsOn;
			showDebugOverlay(mStatsOn);
			mTimeUntilNextToggle = 1;
		}

		// Handy screenshot saving procedure.
		if (mInputDevice->isKeyDown(KC_SYSRQ) 
			&& mTimeUntilNextToggle <= 0)
		{
			char tmp[20];
			sprintf(tmp, "screenshot_%d.png", ++mNumScreenShots);
			ExampleApplication::mWindow->writeContentsToFile(tmp);
			ExampleApplication::mWindow->setDebugText(String("Wrote ") 
				+ tmp);

			// Reset the timer for toggle keys.
			mTimeUntilNextToggle = 0.5;
		}

		// Return true to continue looping.
		return true;
	}

	bool PlaypenApp::processUnbufferedMouseInput(Ogre::Real dt)
	{
		// The following code checks how far the mouse has move since 
		// the last poll.  This data can be used to rotate the camera 
		// around its X and Y axes (pitch and yaw, respectively).
		Ogre::Degree rotY = -mInputDevice->getMouseRelativeX() * mRotateSpeed;
		Ogre::Degree rotX = -mInputDevice->getMouseRelativeY() * mRotateSpeed;

		// Use the relative mouse motion to rotate the camera.
		mPhysicalCamera->yawRelative(rotY.valueDegrees());
		mPhysicalCamera->pitchRelative(rotX.valueDegrees());

		// Check mouse button states.
		if (true == mInputDevice->getMouseButton(0))
		{
			// The left mouse button is down.
			mPhysicalCamera->grasp();
		}
		else
		{
			// The left mouse button is up.
			mPhysicalCamera->release();
		}

		// Return true to continue looping.
		return true;
	}

	void PlaypenApp::setupInitialPhysicalEntities()
	{
		opal::Matrix44r m;

		// Create a static box for a ground plane.
		Ogre::Vector3 boxDim(70, 16, 70);
		opal::Solid* s = mSimulator->createSolid();
		s->setStatic(true);
		s->setPosition(0, -8, 0);
		opal::BoxShapeData data;
		data.dimensions.set(boxDim[0], boxDim[1], boxDim[2]);
		s->addShape(data);
		createPhysicalEntityBox("ground", "Plastic/Gray", boxDim, s);

		// Setup the "object creation tube".
		boxDim.x = 1;
		boxDim.y = 20;
		boxDim.z = 10;
		data.dimensions.set(boxDim[0], boxDim[1], boxDim[2]);

		s = mSimulator->createSolid();
		s->setStatic(true);
		m.makeTranslation(mCreationPoint[0] + 0.5 * boxDim.z, 
			mCreationPoint[1], mCreationPoint[2] + 0.5 * boxDim.x);
		s->setTransform(m);
		s->addShape(data);
		createPhysicalEntityBox("tube0", "Plastic/LightGray", boxDim, s);

		s = mSimulator->createSolid();
		s->setStatic(true);
		m.makeTranslation(mCreationPoint[0] - 0.5 * boxDim.z, 
			mCreationPoint[1], mCreationPoint[2] - 0.5 * boxDim.x);
		s->setTransform(m);
		s->addShape(data);
		createPhysicalEntityBox("tube1", "Plastic/LightGray", boxDim, s);

		s = mSimulator->createSolid();
		s->setStatic(true);
		m.makeRotation(90, 0, 1, 0);
		m.translate(-mCreationPoint[2] + 0.5 * boxDim.z, mCreationPoint[1], 
			mCreationPoint[0] + 0.5 * boxDim.x);
		s->setTransform(m);
		s->addShape(data);
		createPhysicalEntityBox("tube2", "Plastic/LightGray", boxDim, s);

		s = mSimulator->createSolid();
		s->setStatic(true);
		m.makeRotation(90, 0, 1, 0);
		m.translate(-mCreationPoint[2] - 0.5 * boxDim.z, mCreationPoint[1], 
			mCreationPoint[0] - 0.5 * boxDim.x);
		s->setTransform(m);
		s->addShape(data);
		createPhysicalEntityBox("tube3", "Plastic/LightGray", boxDim, s);

		// Make a slide under the object creation tube.
		boxDim.x = 60;
		boxDim.y = 1;
		boxDim.z = 15;
		data.dimensions.set(boxDim[0], boxDim[1], boxDim[2]);
		data.material.friction = 0.00001;
		s = mSimulator->createSolid();
		s->setStatic(true);
		m.makeTranslation(mCreationPoint[0], mCreationPoint[1] - 33, 
			mCreationPoint[2]);
		m.rotate(-45, 0, 1, 0);
		m.translate(0.3 * boxDim.x, 0, 0);
		m.rotate(-30, 0, 0, 1);
		s->setTransform(m);
		s->addShape(data);
		createPhysicalEntityBox("slide", "Plastic/DarkGray", boxDim, s);
	}

	void PlaypenApp::createObject(const std::string& material, ObjectType type)
	{
		switch(type)
		{
			case OBJECT_TYPE_BOX:
			{
				Ogre::Vector3 boxDim(5, 5, 5);
				opal::Solid* s = mSimulator->createSolid();
				s->setPosition(mCreationPoint);
				opal::BoxShapeData data;
				data.dimensions.set(boxDim[0], boxDim[1], boxDim[2]);
				s->addShape(data);
				createPhysicalEntityBox("", material, boxDim, s);
				break;
			}
			case OBJECT_TYPE_SPHERE:
			{
				Ogre::Real radius = 3; // testing
				opal::Solid* s = mSimulator->createSolid();
				s->setPosition(mCreationPoint);
				opal::SphereShapeData data;
				data.radius = radius;
				data.material.density = 1; // testing
				s->addShape(data);
				createPhysicalEntitySphere("", material, radius, s);
				break;
			}
			case OBJECT_TYPE_WALL:
			{
				opal::Matrix44r m;
				m.rotate(45, 0, 1, 0);
				m.translate(0, 0, -23);
				createWall(6, 8, opal::Vec3r(3, 1.5, 1.5), m, material);
				break;
			}
			case OBJECT_TYPE_TOWER:
			{
				createTower(2, 2, 15, opal::Vec3r(3, 1.5, 1.5), 
					opal::Matrix44r(), material);
				break;
			}
			case OBJECT_TYPE_RAGDOLL:
			{
				opal::Blueprint ragdollBP;
				opal::loadFile(ragdollBP, "../../data/blueprints/ragdoll.xml");
				opal::Matrix44r transform;
				transform.translate(mCreationPoint[0], mCreationPoint[1] - 5, 
					mCreationPoint[2]);

				// Instantiate the Blueprint.
				opal::BlueprintInstance instance;
				mSimulator->instantiateBlueprint(instance, ragdollBP, transform, 16);

				unsigned int i=0;
				for (i=0; i<instance.getNumSolids(); ++i)
				{
					opal::Solid* s = instance.getSolid(i);
					const opal::SolidData& data = s->getData();
					unsigned int j=0;
					for (j=0; j<data.getNumShapes(); ++j)
					{
						opal::ShapeData* shapeData = data.getShapeData(j);

						switch(shapeData->getType())
						{
							case opal::BOX_SHAPE:
							{
								opal::Vec3r dim = 
									((opal::BoxShapeData*)shapeData)->dimensions;
								Ogre::Vector3 boxDim(dim[0], dim[1], dim[2]);
								createPhysicalEntityBox("", material, 
									boxDim, s);
								break;
							}
							case opal::SPHERE_SHAPE:
							{
								opal::real r = 
									((opal::SphereShapeData*)shapeData)->radius;
								createPhysicalEntitySphere("", 
									material, r, s);
								break;
							}
							case opal::CAPSULE_SHAPE:
							{
								opal::real r = 
									((opal::CapsuleShapeData*)shapeData)->radius;
								opal::real l = 
									((opal::CapsuleShapeData*)shapeData)->length;
								createPhysicalEntityCapsule("", 
									material, r, l, s);
								break;
							}
							default:
								assert(false);
						}
					}
				}
				break;
			}
			default:
				assert(false);
				break;
		}
	}

	void PlaypenApp::createWall(unsigned int length, unsigned height, 
		const opal::Vec3r& boxDim, const opal::Matrix44r& baseTransform, 
		const std::string& material)
	{
		for (unsigned int l=0; l<length; ++l)
		{
			for (unsigned int h=0; h<height; ++h)
			{
				opal::real offset = 0;
				if (h % 2 == 0)
				{
					offset = (opal::real)0.5 * boxDim[0];
				}

				opal::Matrix44r blockTransform = baseTransform;
				blockTransform.translate(l * boxDim[0] + 
					0.5 * boxDim[0] - 0.5 * length * boxDim[0] + 
					offset, h * boxDim[1] + 0.5 * boxDim[1], 0);

				opal::Solid* s = mSimulator->createSolid();
				s->setTransform(blockTransform);
				//s->setSleeping(true);
				opal::BoxShapeData boxData;
				boxData.dimensions = boxDim;
				boxData.material.density = 6;
				boxData.material.hardness = 0.4;
				s->addShape(boxData);
				Ogre::Vector3 boxDimensions(boxDim[0], boxDim[1], boxDim[2]);
				createPhysicalEntityBox("", material, boxDimensions, s);
			}
		}
	}

	void PlaypenApp::createTower(unsigned int width, unsigned int depth, 
		unsigned height, const opal::Vec3r& boxDim, 
		const opal::Matrix44r& baseTransform, const std::string& material)
	{
		for (unsigned int wall=0; wall<4; ++wall)
		{
			opal::Matrix44r wallBaseTransform = baseTransform;
			unsigned int length = 0;

			switch(wall)
			{
				case 0:
					length = width;
					wallBaseTransform.translate(
						-(opal::real)0.25 * boxDim[0], 0, 
						-(opal::real)0.5 * depth * boxDim[0]);
					break;
				case 1:
					length = width;
					wallBaseTransform.translate(
						(opal::real)0.25 * boxDim[0], 0, 
						(opal::real)0.5 * depth * boxDim[0]);
					wallBaseTransform.rotate(180, 0, 1, 0);
					break;
				case 2:
					length = depth;
					wallBaseTransform.translate(
						-(opal::real)0.5 * width * boxDim[0], 0, 0);
					wallBaseTransform.rotate(90, 0, 1, 0);
					wallBaseTransform.translate(
						-(opal::real)0.25 * boxDim[0], 0, 0);
					break;
				case 3:
					length = depth;
					wallBaseTransform.translate(
						(opal::real)0.5 * width * boxDim[0], 0, 0);
					wallBaseTransform.rotate(-90, 0, 1, 0);
					wallBaseTransform.translate(
						-(opal::real)0.25 * boxDim[0], 0, 0);
					break;
				default:
					assert(false);
					break;
			}

			createWall(length, height, boxDim, wallBaseTransform, material);
		}
	}
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	SET_TERM_HANDLER;
	playpen::PlaypenApp app;

	try
	{
		app.go();
	}
	catch(Ogre::Exception& e)
	{
		std::cerr << "An exception has occured: " <<
			e.getFullDescription().c_str() << std::endl;
	}

	return 0;
}
