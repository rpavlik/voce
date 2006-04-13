/*************************************************************************
 *                                                                       *
 * Voce                                                                  *
 * Copyright (C) 2005                                                    *
 * Tyler Streeter  tylerstreeter@gmail.com                               *
 * All rights reserved.                                                  *
 * Web: voce.sourceforge.net                                             *
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

package voce;

/// A collection of utility functions used in 
/// Voce.
public class Utils
{
	private static boolean mPrintDebug = false;

	/// A simple message logging function.  The message type gets printed 
	/// before the actual message.
	static public void log(String msgType, String msg)
	{
		// If we're ignoring debug messages and this one is a debug 
		// message, return.
		if (!mPrintDebug && msgType.equals("debug"))
		{
			return;
		}

		String finalMessage = "[Voce";

		if (!msgType.equals(""))
		{
			finalMessage = finalMessage + " " + msgType;
		}

		finalMessage = finalMessage + "] " + msg;
		System.out.println(finalMessage);
		System.out.flush();
	}
	
	/// Sets how much debug output to print ('true' prints debug and error 
	/// messages; 'false' prints only error messages).
	static public void setPrintDebug(boolean printDebug)
	{
		mPrintDebug = printDebug;
	}
}
