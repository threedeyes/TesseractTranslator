/*
 * Copyright 2013, Gerasim Troeglazov, 3dEyes@gmail.com. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef TESS_TRANSLATOR_H
#define TESS_TRANSLATOR_H


#include "BaseTranslator.h"

#include <Translator.h>
#include <TranslatorFormats.h>
#include <TranslationDefs.h>
#include <GraphicsDefs.h>
#include <InterfaceDefs.h>
#include <DataIO.h>
#include <File.h>
#include <ByteOrder.h>
#include <OS.h>
#include <Notification.h>
#include <Resources.h>
#include <Roster.h>
#include <File.h>
#include <Bitmap.h>
#include <IconUtils.h>
#include <TranslationUtils.h>

#include <tesseract/baseapi.h>
#include <tesseract/ocrclass.h>
#include <leptonica/allheaders.h>

#include "TesseractLanguages.h"

#define TESS_TRANSLATOR_VERSION B_TRANSLATION_MAKE_VERSION(1, 0, 1)
#define TESS_TEXT_FORMAT	'TESS'

#define BITS_IN_QUALITY		1
#define BITS_IN_CAPABILITY	1

#define TESS_OUT_QUALITY	0.8
#define TESS_OUT_CAPABILITY	0.8

class TesseractTranslator : public BaseTranslator {
	public:
		TesseractTranslator();

		virtual status_t DerivedIdentify(BPositionIO *inSource,
			const translation_format *inFormat, BMessage *ioExtension,
			translator_info *outInfo, uint32 outType);

		virtual status_t DerivedTranslate(BPositionIO *inSource,
			const translator_info *inInfo, BMessage *ioExtension,
			uint32 outType, BPositionIO *outDestination, int32 baseType);

		virtual status_t DerivedCanHandleImageSize(float width,
			float height) const;

		virtual BView *NewConfigView(TranslatorSettings *settings);	

	protected:
		virtual ~TesseractTranslator();
};

#endif	/* TESS_TRANSLATOR_H */
