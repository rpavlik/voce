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

import java.io.BufferedReader;
import java.io.InputStreamReader;

/// A sample application showing how to use Voce's speech synthesis 
/// capabilities.

public class synthesisTest
{
	public static void main(String[] argv)
	{
		voce.SpeechInterface.init("../../../lib", true, false, "", "");

		voce.SpeechInterface.synthesize("This is a speech synthesis test.");
		voce.SpeechInterface.synthesize("Type a message to hear it spoken " 
			+ "aloud.");

		System.out.println("This is a speech synthesis test.  " 
			+ "Type a message to hear it spoken aloud.");
		System.out.println("Type 's' + 'enter' to make the "
			+ "synthesizer stop speaking.  Type 'q' + 'enter' to quit.");

		BufferedReader console = 
			new BufferedReader(new InputStreamReader(System.in));

		try
		{
			String s = "";
			while (!s.equals("q"))
			{
				// Read a line from keyboard.
				s = console.readLine();

				if (s.equals("s"))
				{
					voce.SpeechInterface.stopSynthesizing();
				}
				else
				{
					// Speak what was typed.
					voce.SpeechInterface.synthesize(s);
				}
			}
		}
		catch (java.io.IOException ioe)
		{
			System.out.println( "IO error:" + ioe );
		}

		voce.SpeechInterface.destroy();
		System.exit(0);
	}
}

