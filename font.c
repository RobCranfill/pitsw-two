/**
	font.c
	Functions for fonts, using the font data found in font_data.h
**/

#include <string.h>
#include <stdio.h>

#include "font.h"
#include "font_data.h"

uint32_t *getVRaster(int n) {
	return vRasters[n];
	}

uint32_t *getVRasterForChar(char c) {
	return vRasters[c - ' '];
	}

void font_test_3() {
	
	uint32_t *foo;
	printf("font_test_3\n\n");
	
	foo = vRasters[0x10]; // '0'
	// 	{0x3e, 0x7f, 0x41, 0x49, 0x41, 0x7f, 0x3e, 0x00}, 

	for (int i=0; i<=8; i++) {
		uint32_t t = foo[i];
		printf(" 0[%d] = 0o%03o \n", i, t);
		printf("  shifted: %012o\n", (t << 6));
		}
	printf("\n");
	}


void font_test() {

	uint32_t *foo;
	
	foo = getVRaster(1);
	for (int i=0; i<8; i++) {
		printf(" r[%d] = 0x%02x ", i, foo[i]);
		}
	printf("\n\n");

	foo = getVRaster('?');
	for (int i=0; i<8; i++) {
		printf(" r['?'] = 0x%02x ", foo[i]);
		}
	printf("\n\n");
	return;
	}

void font_test_2() {
	char * testString = "Test";
	for (int i=0; i<strlen(testString); i++) {
		int ascii = testString[i];
		printf(" char %d is '%d'\n", i, ascii);
		int thisRasterSet[8];
		for (int c=0; c<8; c++) {
			thisRasterSet[c] = vRasters[ascii - 0x20][c];
			printf(" r[%d] is %02x\n", c, thisRasterSet[c]);
		} 
	}

}
