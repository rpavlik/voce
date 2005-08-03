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

import edu.cmu.sphinx.frontend.util.Microphone;
import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.util.props.ConfigurationManager;
import edu.cmu.sphinx.util.props.PropertyException;

import java.util.LinkedList;
import java.io.File;
import java.io.IOException;
import java.net.URL;

/// Handles all speech recognition (i.e. speech-to-text) functions.  Uses 
/// a separate thread for recognition.  Maintains an internal queue of 
/// recognized strings.
public class SpeechRecognizer implements Runnable
{
	/// The speech Recognizer instance.
	private Recognizer mRecognizer = null;

	/// The Microphone instance.
	private Microphone mMicrophone = null;

	/// The thread used for speech recognition.  This is necessary to 
	/// avoid making applications wait for recognition to finish.
	private volatile Thread mRecognitionThread = null;

	/// A boolean that tracks whether the recognition thread is running.
	private boolean mRecognitionThreadRunning = false;

	/// A queue of the recognized strings.
	private LinkedList<String> mRecognizedStringQueue;

	/// Constructs and initializes the speech recognizer.  'grammarPath' can 
	/// be a relative or absolute path.  'grammarName' is the name of a 
	/// grammar within a .gram file in the 'grammarPath' (all .gram files 
	/// in 'grammarPath' will automatically be searched).  If the 
	/// 'grammarName' is empty, no grammar will be used.
	public SpeechRecognizer(String configFilename, String grammarPath, 
		String grammarName)
	{
		try
		{
			URL configURL = new File(configFilename).toURI().toURL();
			ConfigurationManager cm = new ConfigurationManager(configURL);

			mRecognizer = (Recognizer) cm.lookup("recognizer");
			mMicrophone = (Microphone) cm.lookup("microphone");

			if (!grammarName.equals(""))
			{
				// This will create this componenent if it has not already 
				// been created.
				cm.lookup("jsgfGrammar");

				// Setup in the user-defined grammar.
				cm.setProperty("jsgfGrammar", "grammarLocation", grammarPath);
				cm.setProperty("jsgfGrammar", "grammarName", grammarName);
			}
			else
			{
				Utils.log("", "No grammar file specified.  Defaulting to " 
					+ "'digits.gram'");
			}

			mRecognizer.allocate();
			mRecognizedStringQueue = new LinkedList<String>();
		}
		catch (IOException e)
		{
			Utils.log("ERROR", "Cannot load speech recognizer: ");
			e.printStackTrace();
		}
		catch (PropertyException e)
		{
			Utils.log("ERROR", "Cannot configure speech recognizer: ");
			e.printStackTrace();
		}
		catch (InstantiationException e)
		{
			Utils.log("ERROR", "Cannot create speech recognizer: ");
			e.printStackTrace();
		}
	}

	/// Contains the main processing to be done by the recognition 
	/// thread.  Called indirectly after 'start' is called.
	public void run()
	{
		Utils.log("debug", "Recognition thread starting");
		mRecognitionThreadRunning = true;
		//Thread currThread = Thread.currentThread();

		//while (mRecognitionThread == currThread)
		while (null != mRecognitionThread)
		{
			if (!mMicrophone.isRecording())
			{
				Utils.log("warning", "Recognition thread is running, but " 
					+ "the microphone is disabled.");
			}
			else
			{
				Result result = mRecognizer.recognize();

				if (result != null)
				{
					String s = result.getBestFinalResultNoFiller();

					// Only save non-empty strings.
					if (!s.equals(""))
					{
						Utils.log("debug", "Finished recognizing");
						mRecognizedStringQueue.addLast(s);
					}
				}
			}
		}

		Utils.log("debug", "Recognition thread finished");
		mRecognitionThreadRunning = false;
	}

	/// Returns the number of recognized strings currently in the 
	/// recognized string queue.
	public int getQueueSize()
	{
		return mRecognizedStringQueue.size();
	}

	/// Returns and removes the oldest recognized string from the 
	/// recognized string queue.  Returns an empty string if the 
	/// queue is empty.
	public String popString()
	{
		if (getQueueSize() > 0)
		{
			return mRecognizedStringQueue.removeFirst();
		}
		else
		{
			return "";
		}
	}

	/// Enables and disables the speech recognizer.  Starts and stops the 
	/// speech recognition thread.
	public void setEnabled(boolean e)
	{
		if (e)
		{
			boolean success = mMicrophone.startRecording();

			if (!success)
			{
				Utils.log("warning", "Cannot initialize microphone. " + 
					"Speech recognition disabled.");
				return;
			}
			else
			{
				if (null == mRecognitionThread)
				{
					mRecognitionThread = new Thread(this, "Recognition thread");
					mRecognitionThread.start();
				}
			}
		}
		else
		{
			//Utils.log("debug", "Clearing queues, stopping microphone...");
			mRecognizedStringQueue.clear();
			mMicrophone.stopRecording();
			mMicrophone.clear();

			// The following line indirectly stops the recognition thread 
			// from running.
			mRecognitionThread = null;

			// Wait for the thread to die before proceeding.
			while (mRecognitionThreadRunning)
			{
				Utils.log("debug", "Waiting for recognition thread to die...");

				try
				{
					// Have the main thread sleep for a bit...
					Thread.sleep(100);
				}
				catch (InterruptedException exception)
				{
				}
			}

			//Utils.log("debug", "...done clearing queues, stopping microphone");
		}
	}

	/// Returns true if the recognizer is currently enabled.
	public boolean isEnabled()
	{
		return mMicrophone.isRecording();
	}

	/// Deallocates speech recognizer.
	public void destroy()
	{
		// This function call will shut down everything, including the 
		// recognition thread.
		setEnabled(false);

		// It should now be safe to deallocate the recognizer.
		mRecognizer.deallocate();
	}
}
