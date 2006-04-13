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

#ifndef VOCE_H
#define VOCE_H

// This file contains a C++ interface for Voce's Java functions.  All of 
// the Java methods in Voce's API are instance methods, so we don't need 
// to handle class methods here.  For documentation on the specific 
// functions, refer to the API documentation for the Java source.

#include <jni.h>
#include <iostream>
#include <string>

/// The namespace containing everything in the Voce C++ API.
namespace voce
{
#ifdef WIN32
const std::string pathSeparator = ";";
#else
const std::string pathSeparator = ":";
#endif

	/// Contains things that should only be accessed within Voce.
	namespace internal
	{
		/// Global instance of the JNI environment.
		JNIEnv* gEnv = NULL;

		/// Global instance of the Java virtual machine.
		JavaVM *gJVM = NULL;

		/// Global reference to the main Voce Java class.
		jclass gClass = NULL;

		/// Global references to the various Java method IDs.  Storing 
		/// these saves lookup time later.
		jmethodID gInitID = NULL;
		jmethodID gDestroyID = NULL;
		jmethodID gSynthesizeID = NULL;
		jmethodID gStopSynthesizingID = NULL;
		jmethodID gGetRecognizerQueueSizeID = NULL;
		jmethodID gPopRecognizedStringID = NULL;
		jmethodID gSetRecognizerEnabledID = NULL;
		jmethodID gIsRecognizerEnabledID = NULL;

		/// The name of the main Voce Java class.
		const std::string gClassName = "voce/SpeechInterface";

		/// A simple message logging function.  The message type gets printed 
		/// before the actual message.
		void log(const std::string& msgType, const std::string& msg)
		{
			std::cout << "[Voce";

			if (std::string("") != msgType)
			{
				std::cout << " " << msgType;
			}

			std::cout << "] " << msg << std::endl;
		}

		/// Finds and returns a method ID for the given function name and 
		/// Java method signature.
		jmethodID loadJavaMethodID(const std::string& functionName, 
			const std::string& functionSignature)
		{
			if (!gEnv)
			{
				log("ERROR", "Method ID cannot be found for function: " 
					+ functionName + " before initialization.");
				return 0;
			}

			// Call the JNI function GetStaticMethodID, which performs a 
			// lookup for the given Java method.  The lookup is based on 
			// the name of the method as well as the method signature.  If 
			// the method does not exist, GetStaticMethodID 
			// returns zero (0). An immediate return from the native method 
			// at that point causes a NoSuchMethodError to be thrown in the 
			// Java application code.  Note that the JNI uses the method 
			// signature to denote the return type of a Java method. The 
			// signature (I)V, for example, denotes a Java method that takes 
			// one argument of type int and has a return type void. The 
			// general form of a method signature argument is: 
			// "(argument-types)return-type".
			jmethodID methodID = gEnv->GetStaticMethodID(gClass, 
				functionName.c_str(), functionSignature.c_str());

			if (0 == methodID)
			{
				log("ERROR", "Invalid Java method ID requested for: " 
					+ functionName + ".  Function call ignored.");
				return 0;
			}

			return methodID;
		}
	}

	/// Initializes Voce.  This function performs some extra stuff needed 
	/// to setup the Java virtual machine and cache the Java method IDs.  
	/// The 'vocePath' argument is the path to the Voce class files and 
	/// config file.  This path can be absolute or relative to the 
	/// working directory of the executable using Voce.  See documentation 
	/// for the Java version for an explanation of the rest of the 
	/// parameters.
	void init(const std::string& vocePath, bool initSynthesis, 
		bool initRecognition, const std::string& grammarPath, 
		const std::string& grammarName)
	{
		// Setup the Java virtual machine.
		JavaVMInitArgs vm_args;
		memset(&vm_args, 0, sizeof(vm_args));
		vm_args.version = JNI_VERSION_1_4;

		long status = 0;

		// If recognition is being used, we need to increase the max heap 
		// size.
		if (initRecognition)
		{
			vm_args.nOptions = 2;

			// Setup the VM options.
			// TODO: check out other options to be used here, like disabling the 
			// JIT compiler.
			JavaVMOption options[2];

			// Add the required Java class paths.
			std::string classPathString = "-Djava.class.path=";
			classPathString += vocePath;
			classPathString += "/voce.jar";
			char s[1024];
			sprintf(s, classPathString.c_str());
			options[0].optionString = s;
			options[0].extraInfo = NULL;

			// Add an option to increase the max heap size.
			options[1].optionString = "-Xmx256m";
			options[1].extraInfo = NULL;
			//options[1].optionString = "-Djava.compiler=NONE"; // Disable JIT.
			//options[1].optionString = "-verbose:gc,class,jni";
			vm_args.options = options;
			//vm_args.ignoreUnrecognized = JNI_FALSE;

			// Create the VM.
			status = JNI_CreateJavaVM(&internal::gJVM, 
				(void**)&internal::gEnv, &vm_args);
		}
		else
		{
			vm_args.nOptions = 1;

			// Setup the VM options.
			// TODO: check out other options to be used here, like disabling the 
			// JIT compiler.
			JavaVMOption options[1];

			// Add the required Java class paths.
			std::string classPathString = "-Djava.class.path=";
			classPathString += vocePath;
			classPathString += "/voce.jar";
			char s[1024];
			sprintf(s, classPathString.c_str());
			options[0].optionString = s;
			options[0].extraInfo = NULL;
			//options[1].optionString = "-Djava.compiler=NONE"; // Disable JIT.
			//options[1].optionString = "-verbose:gc,class,jni";
			vm_args.options = options;
			//vm_args.ignoreUnrecognized = JNI_FALSE;

			// Create the VM.
			status = JNI_CreateJavaVM(&internal::gJVM, 
				(void**)&internal::gEnv, &vm_args);
		}

		if (status < 0)
		{
			internal::log("ERROR", "Java virtual machine cannot be created");
			return;
		}

		internal::log("", "Java virtual machine created");

		// Find the main Voce class by name.
		jclass c = internal::gEnv->FindClass(internal::gClassName.c_str());

		if (0 == c)
		{
			internal::log("ERROR", "The requested Java class: " 
				+ internal::gClassName + " could not be found.  \
Make sure the class path correctly points to the Voce classes.");
			return;
		}

		// Store a global reference so the class won't go out of 
		// scope and get destroyed.
		internal::gClass = (jclass)internal::gEnv->NewGlobalRef(c);

		// Cache the global method ID references for the various 
		// functions.  This should save lookup time.
		internal::gInitID = internal::loadJavaMethodID("init", 
			"(Ljava/lang/String;ZZLjava/lang/String;Ljava/lang/String;)V");
		internal::gDestroyID = internal::loadJavaMethodID("destroy", "()V");
		internal::gSynthesizeID = internal::loadJavaMethodID("synthesize", 
			"(Ljava/lang/String;)V");
		internal::gStopSynthesizingID = internal::loadJavaMethodID(
			"stopSynthesizing", "()V");
		internal::gGetRecognizerQueueSizeID = internal::loadJavaMethodID(
			"getRecognizerQueueSize", "()I");
		internal::gPopRecognizedStringID = internal::loadJavaMethodID(
			"popRecognizedString", "()Ljava/lang/String;");
		internal::gSetRecognizerEnabledID = internal::loadJavaMethodID(
			"setRecognizerEnabled", "(Z)V");
		internal::gIsRecognizerEnabledID = internal::loadJavaMethodID(
			"isRecognizerEnabled", "()Z");

		// Convert the C++ strings to Java strings.
		jstring jStrVocePath = internal::gEnv->NewStringUTF(
			vocePath.c_str());
		jstring jStrGrammarPath = internal::gEnv->NewStringUTF(
			grammarPath.c_str());
		jstring jStrGrammarName = internal::gEnv->NewStringUTF(
			grammarName.c_str());

		// Initialize the Java Voce stuff.
		internal::gEnv->CallStaticVoidMethod(internal::gClass, 
			internal::gInitID, jStrVocePath, initSynthesis, initRecognition, 
			jStrGrammarPath, jStrGrammarName);
	}

	/// In addition to the usual Java Voce destroy call, this function 
	/// performs some extra stuff needed to destroy the virtual machine.
	void destroy()
	{
		if (!internal::gEnv)
		{
			internal::log("warning", "destroy called before \
initialization.  Request will be ignored.");
			return;
		}

		// Destroy the Java Voce stuff.
		internal::gEnv->CallStaticVoidMethod(internal::gClass, 
			internal::gDestroyID);

		// Destroy the global reference to the main Voce Java class and to 
		// the various global method ID references.
		internal::gEnv->DeleteGlobalRef((jobject)internal::gClass);
		internal::gEnv->DeleteGlobalRef((jobject)internal::gInitID);
		internal::gEnv->DeleteGlobalRef((jobject)internal::gDestroyID);
		internal::gEnv->DeleteGlobalRef((jobject)internal::gSynthesizeID);
		internal::gEnv->DeleteGlobalRef(
			(jobject)internal::gStopSynthesizingID);
		internal::gEnv->DeleteGlobalRef(
			(jobject)internal::gGetRecognizerQueueSizeID);
		internal::gEnv->DeleteGlobalRef(
			(jobject)internal::gPopRecognizedStringID);
		internal::gEnv->DeleteGlobalRef(
			(jobject)internal::gSetRecognizerEnabledID);
		internal::gEnv->DeleteGlobalRef(
			(jobject)internal::gIsRecognizerEnabledID);

		if (internal::gJVM)
		{
			// Destroy the virtual machine.
			internal::gJVM->DestroyJavaVM();
			internal::log("", "Java virtual machine destroyed");
		}
	}

	/// Requests that the given string be synthesized as soon as possible.
	void synthesize(const std::string& message)
	{
		if (!internal::gEnv)
		{
			internal::log("warning", "synthesize called before \
initialization.  Request will be ignored.");
		}

		// Convert the C++ string to a Java string.
		jstring jstr = internal::gEnv->NewStringUTF(message.c_str());

		// Call the Java method.
		internal::gEnv->CallStaticVoidMethod(internal::gClass, 
			internal::gSynthesizeID, jstr);
	}

	/// Tells the speech synthesizer to stop synthesizing.  This cancels all 
	/// pending messages.
	void stopSynthesizing()
	{
		if (!internal::gEnv)
		{
			internal::log("warning", "synthesize called before \
initialization.  Request will be ignored.");
		}

		// Call the Java method.
		internal::gEnv->CallStaticVoidMethod(internal::gClass, 
			internal::gStopSynthesizingID);
	}

	/// Returns the number of recognized strings currently in the 
	/// recognizer's queue.
	int getRecognizerQueueSize()
	{
		if (!internal::gEnv)
		{
			internal::log("warning", "getRecognizerQueueSize called before \
initialization.  Request will be ignored.");
		}

		// Call the Java method.
		return internal::gEnv->CallStaticIntMethod(internal::gClass, 
			internal::gGetRecognizerQueueSizeID);
	}

	/// Returns and removes the oldest recognized string from the 
	/// recognizer's queue.
	std::string popRecognizedString()
	{
		if (!internal::gEnv)
		{
			internal::log("warning", "popRecognizedString called before \
initialization.  Request will be ignored.");
		}

		// Call the Java method.
		jstring jstr = (jstring)internal::gEnv->CallStaticObjectMethod(
			internal::gClass, internal::gPopRecognizedStringID);

		// Convert string from Java to C++.  Be sure to release memory 
		// when finished.
		const char* tempStr = internal::gEnv->GetStringUTFChars(jstr, 0);
		std::string cppStr = tempStr;
		internal::gEnv->ReleaseStringUTFChars(jstr, tempStr);

		return cppStr;
	}

	/// Enables and disables the speech recognizer.
	void setRecognizerEnabled(bool e)
	{
		if (!internal::gEnv)
		{
			internal::log("warning", "setRecognizerEnabled called before \
initialization.  Request will be ignored.");
		}

		// Call the Java method.
		internal::gEnv->CallStaticVoidMethod(internal::gClass, 
			internal::gSetRecognizerEnabledID, e);
	}

	/// Returns true if the recognizer is currently enabled.
	bool isRecognizerEnabled()
	{
		if (!internal::gEnv)
		{
			internal::log("warning", "isRecognizerEnabled called before \
initialization.  Request will be ignored.");
		}

		// Call the Java method.
		jboolean b = internal::gEnv->CallStaticBooleanMethod(internal::gClass, 
			internal::gIsRecognizerEnabledID);

		if (JNI_FALSE == b)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
}

#endif
