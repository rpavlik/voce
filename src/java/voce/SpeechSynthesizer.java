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

import java.io.File;
import java.util.Locale;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import javax.speech.Central;
import javax.speech.Engine;
import javax.speech.EngineList;
import javax.speech.EngineCreate;
import javax.speech.EngineException;
import javax.speech.EngineModeDesc;
import javax.speech.synthesis.Synthesizer;
import javax.speech.synthesis.SynthesizerModeDesc;
import javax.speech.synthesis.SynthesizerProperties;
import javax.speech.synthesis.Voice;
import javax.speech.synthesis.Speakable;
import javax.speech.synthesis.SpeakableAdapter;
import javax.speech.synthesis.SpeakableEvent;
import com.sun.speech.freetts.jsapi.FreeTTSEngineCentral; 

/// Handles all speech synthesis (i.e. text-to-speech) 
/// functions.
public class SpeechSynthesizer
{
	/// The speech Synthesizer instance.
	private Synthesizer mSynthesizer = null;

	/// Constructs and initializes the speech synthesizer.
	public SpeechSynthesizer(String name)
	{
		// Create a default voice.
		Voice theVoice = new Voice(name, 
			Voice.GENDER_DONT_CARE, Voice.AGE_DONT_CARE, null);

		try
		{		
			// Create the synthesizer with the general domain voice.
			SynthesizerModeDesc generalDesc = new SynthesizerModeDesc(
				null,			// engine name
				"general",		// mode name
				Locale.US,		// locale
				null,			// running
				null);			// voice

			// Avoid using the JSAPI Central class (and the use of the speech.properties 
			// file) by using FreeTTSEngineCentral directly.
			FreeTTSEngineCentral central = new FreeTTSEngineCentral();
			EngineList list = central.createEngineList(generalDesc); 

			if (list.size() > 0)
			{ 
				EngineCreate creator = (EngineCreate)list.get(0);
				mSynthesizer = (Synthesizer)creator.createEngine();
			}

			if (null  == mSynthesizer)
			{
				Utils.log("ERROR", "Cannot create speech synthesizer");
				System.exit(1);
			}

			mSynthesizer.allocate();

			// Setup the general domain synthesizer.
			mSynthesizer.getSynthesizerProperties().setVoice(theVoice);

			mSynthesizer.resume();

			// Force the synthesizer to create its thread now by making 
			// it synthesize something.  Otherwise, the first synthesize 
			// request in a user's app could be delayed.
			synthesize(" ");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	
	/// Deallocates voice synthesizer.
	public void destroy()
	{
		mSynthesizer.cancelAll();
		
		try
		{		
			mSynthesizer.deallocate();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}

	/// Adds a message to the synthesizer's queue and synthesize it as 
	/// soon as it reaches the front of the queue.
	public void synthesize(String message)
	{
		//Utils.log("debug", "SpeechSynthesizer.speak: Adding message to speech queue: " + message);
	
		// Note that the Synthesize class maintains its own internal queue.
		mSynthesizer.speakPlainText(message, null);
	}

	/// Stops synthesizing the current message and removes all pending 
	/// messages from the queue.
	public void stopSynthesizing()
	{
		mSynthesizer.cancelAll();
	}
}
