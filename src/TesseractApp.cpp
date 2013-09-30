/*
 * Copyright 2013, Gerasim Troeglazov, 3dEyes@gmail.com. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include <Application.h>

#include "TesseractTranslator.h"
#include "TranslatorWindow.h"

int
main(int /*argc*/, char ** /*argv*/)
{
	BApplication app("application/x-vnd.Haiku-TesseractTranslator");

	status_t result;
	result = LaunchTranslatorWindow(new TesseractTranslator, 
		"Tesseract-OCR Settings", BRect(0, 0, 320, 200));
	if (result != B_OK)
		return 1;

	app.Run();
	return 0;
}

