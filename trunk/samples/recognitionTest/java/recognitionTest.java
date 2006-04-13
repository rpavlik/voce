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

/// A sample application showing how to use Voce's speech recognition 
/// capabilities.

public class recognitionTest
{
	public static void main(String[] argv)
	{
		voce.SpeechInterface.init("../../../lib", false, true, 
			"./grammar", "digits");

		System.out.println("This is a speech recognition test. " 
			+ "Speak digits from 0-9 into the microphone. " 
			+ "Speak 'quit' to quit.");

		boolean quit = false;
		while (!quit)
		{
			// Normally, applications would do application-specific things 
			// here.  For this sample, we'll just sleep for a little bit.
			try
			{
				Thread.sleep(200);
			}
			catch (InterruptedException e)
			{
			}

			while (voce.SpeechInterface.getRecognizerQueueSize() > 0)
			{
				String s = voce.SpeechInterface.popRecognizedString();

				// Check if the string contains 'quit'.
				if (-1 != s.indexOf("quit"))
				{
					quit = true;
				}

				System.out.println("You said: " + s);
				//voce.SpeechInterface.synthesize(s);
			}
		}

		voce.SpeechInterface.destroy();
		System.exit(0);
	}
}

