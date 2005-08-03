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

#include <voce/voce.h>

/// A sample application showing how to use Voce's speech synthesis 
/// capabilities.

int main(int argc, char **argv)
{
	voce::init("../../../lib", true, false, "", "");

	voce::synthesize("This is a speech synthesis test.");
	voce::synthesize("Type a message to hear it spoken aloud.");

	std::cout << "This is a speech synthesis test.  " 
		<< "Type a message to hear it spoken aloud." << std::endl;
	std::cout << "Type 's' + 'enter' to make the "
		<< "synthesizer stop speaking.  Type 'q' + 'enter' to quit." 
		<< std::endl;

	std::string s;

	while (s != "q")
	{
		// Read a line from keyboard.
		std::getline(std::cin, s);

		if ("s" == s)
		{
			voce::stopSynthesizing();
		}
		else
		{
			// Speak what was typed.
			voce::synthesize(s);
		}
	}
	
	voce::destroy();
	return 0;
}
