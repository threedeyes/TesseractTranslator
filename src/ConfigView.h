/*
 * Copyright 2012, Gerasim Troeglazov, 3dEyes@gmail.com. All rights reserved.
 * Distributed under the terms of the MIT License.
 */ 

#ifndef CONFIG_VIEW_H
#define CONFIG_VIEW_H

#include "TranslatorSettings.h"

#include <View.h>
#include <TextView.h>
#include <String.h>
#include <GroupView.h>
#include <Menu.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <MenuField.h>


#define TESS_LANGUAGE "tess/language"


class ConfigView : public BGroupView {
	public:
		ConfigView(TranslatorSettings *settings);
		virtual ~ConfigView();
	private:
		void MessageReceived(BMessage* message);
		void AllAttached();
		
		BMenuField 		   *fLanguageMF;
		BTextView*			fCopyrightView;	
		TranslatorSettings *fSettings;
};

#endif	// CONFIG_VIEW_H
