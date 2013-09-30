/*
 * Copyright 2013, Gerasim Troeglazov, 3dEyes@gmail.com. All rights reserved.
 * Distributed under the terms of the MIT License.
 */ 

#include "ConfigView.h"
#include "TesseractTranslator.h"
#include "TesseractLanguages.h"

#include <StringView.h>
#include <SpaceLayoutItem.h>
#include <ControlLook.h>

#include <stdio.h>


ConfigView::ConfigView(TranslatorSettings *settings)
	: BGroupView("ICNSTranslator Settings", B_VERTICAL, 0)
{
	fSettings = settings;
	
	BAlignment leftAlignment(B_ALIGN_LEFT, B_ALIGN_VERTICAL_UNSET);

	BStringView *stringView = new BStringView("title", "Tesseract-OCR Translator");
	stringView->SetFont(be_bold_font);
	stringView->SetExplicitAlignment(leftAlignment);
	AddChild(stringView);

	float spacing = be_control_look->DefaultItemSpacing();
	AddChild(BSpaceLayoutItem::CreateVerticalStrut(spacing));

	char version[256];
	sprintf(version, "Version %d.%d.%d, %s",
		int(B_TRANSLATION_MAJOR_VERSION(TESS_TRANSLATOR_VERSION)),
		int(B_TRANSLATION_MINOR_VERSION(TESS_TRANSLATOR_VERSION)),
		int(B_TRANSLATION_REVISION_VERSION(TESS_TRANSLATOR_VERSION)),
		__DATE__);
	stringView = new BStringView("version", version);
	stringView->SetExplicitAlignment(leftAlignment);
	AddChild(stringView);

	stringView = new BStringView("my_copyright",
		B_UTF8_COPYRIGHT "2013 Gerasim Troeglazov <3dEyes@gmail.com>.");
	stringView->SetExplicitAlignment(leftAlignment);
	AddChild(stringView);

	AddChild(BSpaceLayoutItem::CreateVerticalStrut(spacing));
	
	int32 currentLanguage = fSettings->SetGetInt32(TESS_LANGUAGE);
	
	BPopUpMenu* menu = new BPopUpMenu("eng");

	tesseract::TessBaseAPI tessApi;
	tessApi.Init("data","eng");
	
	GenericVector<STRING> languages;
    tessApi.GetAvailableLanguagesAsVector(&languages);
    
    if(currentLanguage >= languages.size())
    	currentLanguage = 0;

	for (int index = 0; index < languages.size(); ++index) {
        STRING& string = languages[index];
		
		BMessage* message = new BMessage('LNGC');
		message->AddInt32("value", index);
	
		int i=0;
		BString langName(string.string());
		while(gLngInfo[i].lang[0] != 0) {
			if (strcmp(gLngInfo[i].lang, string.string()) == 0) {
				langName.SetTo(gLngInfo[i].description);
				break;
			}
			i++;
		}
	
		BMenuItem* item = new BMenuItem(langName, message);
		item->SetMarked(currentLanguage == index);
		menu->AddItem(item);
     } 
     
    tessApi.End();

	fLanguageMF = new BMenuField("language", "Language:", menu);	
	AddChild(fLanguageMF);	
	
	AddChild(BSpaceLayoutItem::CreateGlue());
	GroupLayout()->SetInsets(B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING, 
		B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING);

	SetExplicitPreferredSize(GroupLayout()->MinSize());		
}


ConfigView::~ConfigView()
{
	fSettings->Release();
}


void
ConfigView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case 'LNGC': {
			int32 value;
			if (message->FindInt32("value", &value) >= B_OK) {
				fSettings->SetGetInt32(TESS_LANGUAGE, &value);
				fSettings->SaveSettings();
			}
//			fLanguageMF->ResizeToPreferred();
			break;
		}
		default:
			BView::MessageReceived(message);
	}
}


void
ConfigView::AllAttached()
{
	fLanguageMF->Menu()->SetTargetForItems(this);	
}
