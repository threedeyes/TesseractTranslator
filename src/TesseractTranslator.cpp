/*
 * Copyright 2013-2018, Gerasim Troeglazov, 3dEyes@gmail.com. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#include "TesseractTranslator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ConfigView.h"

#define kTextMimeType "text/tesseract"
#define kTextName "Recognized text (Tesseract)"

// The input formats that this translator supports.
static const translation_format sInputFormats[] = {
	{
		B_TRANSLATOR_BITMAP,
		B_TRANSLATOR_BITMAP,
		BITS_IN_QUALITY,
		BITS_IN_CAPABILITY,
		"image/x-be-bitmap",
		"Be Bitmap Format (TesseractTranslator)"
	},
};

// The output formats that this translator supports.
static const translation_format sOutputFormats[] = {
	{
		TESS_TEXT_FORMAT,
		B_TRANSLATOR_BITMAP,
		TESS_OUT_QUALITY,
		TESS_OUT_CAPABILITY,
		kTextMimeType,
		kTextName
	},
};

// Default settings for the Translator
static const TranSetting sDefaultSettings[] = {
	{B_TRANSLATOR_EXT_HEADER_ONLY, TRAN_SETTING_BOOL, false},
	{B_TRANSLATOR_EXT_DATA_ONLY, TRAN_SETTING_BOOL, false},
	{TESS_LANGUAGE, TRAN_SETTING_INT32, 0}
};

const uint32 kNumInputFormats = sizeof(sInputFormats)
	/ sizeof(translation_format);
const uint32 kNumOutputFormats = sizeof(sOutputFormats)
	/ sizeof(translation_format);
const uint32 kNumDefaultSettings = sizeof(sDefaultSettings)
	/ sizeof(TranSetting);

struct notify_cookie {
	ETEXT_DESC *monitor;
	BBitmap *animation[24];
};

int32 monitor_thread(void *data)
{
	notify_cookie* cookie = (notify_cookie*)data;
	ETEXT_DESC *monitor = cookie->monitor;
	
	char mess_id[64];	
	sprintf(mess_id, "TESS#%p",data);
	
	int lastprogress = monitor->progress;
	int frame = 0;
	
	for(;;) {
		lastprogress = monitor->progress;
		BNotification mess(B_PROGRESS_NOTIFICATION);
		mess.SetTitle("Tesseract OCR");
		mess.SetIcon(cookie->animation[frame]);
		mess.SetProgress(((float)lastprogress-30)/70.0);
		mess.SetMessageID(mess_id);
		mess.Send(1000000);			
		
		frame++;
		frame%=24;

		if (monitor->progress == 100)
			break;
		snooze(50000);
	}
	return 0;
}


TesseractTranslator::TesseractTranslator()
	: BaseTranslator("Tesseract-OCR", 
		"Tesseract-OCR Translator",
		TESS_TRANSLATOR_VERSION,
		sInputFormats, kNumInputFormats,
		sOutputFormats, kNumOutputFormats,
		"TesseractTranslator",
		sDefaultSettings, kNumDefaultSettings,
		B_TRANSLATOR_BITMAP, TESS_TEXT_FORMAT)
{
}


TesseractTranslator::~TesseractTranslator()
{
}


status_t
TesseractTranslator::DerivedIdentify(BPositionIO *stream,
	const translation_format *format, BMessage *ioExtension,
	translator_info *info, uint32 outType)
{
	if (outType != TESS_TEXT_FORMAT)
		return B_NO_TRANSLATOR;

	char header[sizeof(TranslatorBitmap)];
	status_t err = stream->Read(header, sizeof(TranslatorBitmap));
	if (err < B_OK)
		return err;
	
	if (B_BENDIAN_TO_HOST_INT32(((TranslatorBitmap *)header)->magic) == B_TRANSLATOR_BITMAP) {
		info->type = sInputFormats[0].type;
		info->translator = 0;
		info->group = sInputFormats[0].group;
		info->quality = sInputFormats[0].quality;
		info->capability = sInputFormats[0].capability;
		strcpy(info->name, sInputFormats[0].name);
		strcpy(info->MIME, sInputFormats[0].MIME);

//		strcpy(sOutputFormats[0].MIME,TESS_TRANSLATOR_MIME_STRING);

		return B_OK;
	}
	
	return B_NO_TRANSLATOR;
}


status_t
TesseractTranslator::DerivedTranslate(BPositionIO *source,
	const translator_info *info, BMessage *ioExtension,
	uint32 outType, BPositionIO *target, int32 baseType)
{
	if (baseType == 1 && outType == TESS_TEXT_FORMAT ) {    
		TranslatorBitmap header;
		status_t err = source->Read(&header, sizeof(TranslatorBitmap));
		if (err < B_OK) return err;
		else if (err < (int)sizeof(TranslatorBitmap)) return B_ERROR;
	
		BRect bounds;
		bounds.left = B_BENDIAN_TO_HOST_FLOAT(header.bounds.left);
		bounds.top = B_BENDIAN_TO_HOST_FLOAT(header.bounds.top);
		bounds.right = B_BENDIAN_TO_HOST_FLOAT(header.bounds.right);
		bounds.bottom = B_BENDIAN_TO_HOST_FLOAT(header.bounds.bottom);
	
		int width = bounds.IntegerWidth() + 1;
		int height = bounds.IntegerHeight() + 1;
	
		color_space cs = (color_space)B_BENDIAN_TO_HOST_INT32(header.colors);
		int row_bytes = B_BENDIAN_TO_HOST_INT32(header.rowBytes);
		
		int pix_bytes = 0;
		if(cs == B_RGB32 || cs == B_RGBA32)
			pix_bytes = 4;
		if(cs == B_RGB15 || cs == B_RGBA15 || cs == B_RGB16)
			pix_bytes = 2;
		if(cs == B_CMAP8 || cs == B_GRAY8)
			pix_bytes = 1;
		
		if(pix_bytes > 0) {
			struct notify_cookie cookie;
			cookie.monitor = new ETEXT_DESC[1];						
			cookie.monitor->progress = 0;
			
			entry_ref ref;
			be_roster->FindApp("application/x-vnd.Haiku-TesseractTranslator",&ref);
			BFile file(&ref, B_READ_ONLY);
			BResources res(&file);
			
			for (int frame = 0; frame < 24 ;frame++ ) {
				char frame_name[8];
				sprintf(frame_name, "TF_%d",frame);
				size_t sz = 0;
				const void *data = res.LoadResource('DATA',frame_name, &sz);
				BMemoryIO stream(data, sz);
				stream.Seek(0, SEEK_SET);
				BBitmap *bmp = BTranslationUtils::GetBitmap(&stream);
				cookie.animation[frame] = bmp;				
			}
		
			thread_id my_thread = spawn_thread(monitor_thread, "monitor_thread", 100, (void*)&cookie);
    		resume_thread(my_thread);
    	 
		 	tesseract::TessBaseAPI tessApi;
    		tessApi.Init("data", "eng");
    		
    		BString langName("eng");
    		
			GenericVector<STRING> languages;
    		tessApi.GetAvailableLanguagesAsVector(&languages);
    
    		int32 currentLanguage = fSettings->SetGetInt32(TESS_LANGUAGE);
    
    		if(currentLanguage >= languages.size())
    			currentLanguage = 0;

			for (int index = 0; index < languages.size(); ++index) {
        		STRING& string = languages[index];		
				if(currentLanguage == index) {
					langName.SetTo(string.string());
					break;
				}
     		} 
     
    		tessApi.End();
    		tessApi.Init("data", langName.String());
    		
	    	unsigned char *pix = (unsigned char *)malloc(row_bytes*height);

		   	source->Read(pix, row_bytes*height);
    	
    		tessApi.SetImage(pix, width, height, pix_bytes, row_bytes);
    	
    		tessApi.Recognize(cookie.monitor);
    	
    		char *text = tessApi.GetUTF8Text();
    	
    		target->Write(text,strlen(text));
    		    	    	
    		cookie.monitor->progress = 100;
    	
    		status_t exit_code;    	
    		wait_for_thread(my_thread, &exit_code);
    	
    		delete [] text;
    		delete cookie.monitor;
    		
    		free(pix);
    		
    		tessApi.End();
    		
			return B_OK;
		}
	}
	return B_NO_TRANSLATOR;
}


status_t
TesseractTranslator::DerivedCanHandleImageSize(float width, float height) const
{
	return B_OK;
}


BView *
TesseractTranslator::NewConfigView(TranslatorSettings *settings)
{
	return new ConfigView(settings);
}


//	#pragma mark -


BTranslator *
make_nth_translator(int32 n, image_id you, uint32 flags, ...)
{
	if (n != 0)
		return NULL;

	return new TesseractTranslator();
}

