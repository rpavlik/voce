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

#ifndef OPAL_SAMPLES_PHYSICAL_ENTITY_H
#define OPAL_SAMPLES_PHYSICAL_ENTITY_H

#include <opal/opal.h>

namespace opalSamples
{
	class PhysicalEntity
	{
	public:
		/// PhysicalEntities should not be instantiated directly.  Instead, 
		/// use the BaseOpalApp function for creating PhysicalEntities.
		PhysicalEntity(const std::string& name, Ogre::SceneNode* node, 
			opal::Solid* s)
		{
			mName = name;
			mSceneNode = node;
			mSolid = s;

			if (mSolid && mSceneNode)
			{
				updateOgreSceneNode();
			}
		}

		virtual ~PhysicalEntity()
		{
			// The OPAL Solid and Ogre SceneNode should be destroyed 
			// externally, not here.
		}

		/// Updates the PhysicalEntity.
		virtual void update(opal::real dt)
		{
			if (NULL == mSolid || NULL == mSceneNode || mSolid->isSleeping())
			{
				return;
			}

			updateOgreSceneNode();
		}

		/// Updates the Ogre SceneNode transform from the OPAL 
		/// Solid's transform.
		virtual void updateOgreSceneNode()
		{
			opal::Point3r pos = mSolid->getPosition();
			opal::Quaternion quat = mSolid->getQuaternion();
			mSceneNode->setPosition((Ogre::Real)pos[0], (Ogre::Real)pos[1], 
				(Ogre::Real)pos[2]);
			quat.normalize();
			mSceneNode->setOrientation((Ogre::Real)quat[0], 
				(Ogre::Real)quat[1], (Ogre::Real)quat[2], 
				(Ogre::Real)quat[3]);
		}

		/// Returns the PhysicalEntity's name.
		const std::string& getName()const
		{
			return mName;
		}

		/// Returns a pointer to the Ogre SceneNode.
		Ogre::SceneNode* getSceneNode()const
		{
			return mSceneNode;
		}

		/// Returns a pointer to the OPAL Solid.
		opal::Solid* getSolid()const
		{
			return mSolid;
		}

	protected:
		/// The PhysicalEntity's name.
		std::string mName;

		/// Pointer to the OPAL Solid.
		opal::Solid* mSolid;

		/// Pointer to the Ogre SceneNode.
		Ogre::SceneNode* mSceneNode;

	private:
	};
}

#endif
