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

namespace myAppNamespace
{
	class MyApp : public opalSamples::BaseOpalApp
	{
	public:
		MyApp();

		~MyApp();

	protected:
		virtual void createScene();

		virtual bool appFrameStarted(opal::real dt);

		virtual bool processUnbufferedKeyInput(Ogre::Real dt);

		virtual bool processUnbufferedMouseInput(Ogre::Real dt);

	private:
		/// Used to toggle shadows on and off.
		bool mUseShadows;
	};

	MyApp::MyApp()
	: BaseOpalApp()
	{
		mUseShadows = true;
	}

	MyApp::~MyApp()
	{
	}

	void MyApp::createScene()
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

		// Set the ambient light level.
		mSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));

		// Create a light source.
		Ogre::Light* light = mSceneMgr->createLight("light0");
		light->setType(Ogre::Light::LT_POINT);
		light->setDiffuseColour(1.0, 1.0, 1.0);
		light->setSpecularColour(1.0, 1.0, 1.0);
		light->setPosition(100.0, 300.0, 100.0);

		// Setup the initial camera position.
		mPhysicalCamera->setPosition(opal::Point3r(0, 30, 50));
		mPhysicalCamera->lookAt(opal::Point3r(0, 0, 0));

		// Load models, create physical objects, etc. here.
	}

	bool MyApp::appFrameStarted(opal::real dt)
	{
		// Do per-frame application-specific things here.

		// Return true to continue looping.
		return true;
	}

	bool MyApp::processUnbufferedKeyInput(Ogre::Real dt)
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

		// The following code updates the camera's position.
		opal::Vec3r cameraDir;
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

	bool MyApp::processUnbufferedMouseInput(Ogre::Real dt)
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
		}
		else
		{
			// The left mouse button is up.
		}

		// Return true to continue looping.
		return true;
	}
}

int main(int argc, char **argv)
{
	srand(time(NULL));
	SET_TERM_HANDLER;
	myAppNamespace::MyApp app;

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
