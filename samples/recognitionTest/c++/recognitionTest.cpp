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

#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

/// A sample application showing how to use Voce's speech synthesis 
/// capabilities.

int main(int argc, char **argv)
{
	voce::init("../../../lib", false, true, "./grammar", "digits");

	std::cout << "This is a speech recognition test. " 
		<< "Speak digits from 0-9 into the microphone. " 
		<< "Speak 'quit' to quit." << std::endl;

	bool quit = false;
	while (!quit)
	{
		// Normally, applications would do application-specific things 
		// here.  For this sample, we'll just sleep for a little bit.
#ifdef WIN32
		::Sleep(200);
#else
		usleep(200);
#endif

		while (voce::getRecognizerQueueSize() > 0)
		{
			std::string s = voce::popRecognizedString();

			// Check if the string contains 'quit'.
			if (std::string::npos != s.rfind("quit"))
			{
				quit = true;
			}

			std::cout << "You said: " << s << std::endl;
			//voce::synthesize(s);
		}
	}

	voce::destroy();
	return 0;
}
