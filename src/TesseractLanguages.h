/*
 * Copyright 2013, Gerasim Troeglazov, 3dEyes@gmail.com. All rights reserved.
 * Distributed under the terms of the MIT License.
 */

#ifndef TESS_LANGUAGES_H
#define TESS_LANGUAGES_H

struct lang_info {
	char 	lang[16];
	char 	description[32];
};

extern lang_info gLngInfo[];

#endif	/* TESS_LANGUAGES_H */
