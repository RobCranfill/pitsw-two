/**
	font.c
	Functions for fonts, using the font data found in font_data.h
**/

#include <string.h>
#include <stdio.h>
#include <malloc.h>

#include "font.h"
#include "font_data.h"


void font_test() {
	font_test_4();
	}

void font_test_4() {

	char *test = "XO!";
	uint32_t *led_data = getRastersForStr(test);
	for (int i=0; i<CHAR_WIDTH_BITS*strlen(test); i++) {
		printf("led_data[%02d] = %08b \n", i, led_data[i]);
		}
	}


/**
 * Return the (vertical) LED rasters for the given string.
*/
uint32_t *getRastersForStr(char *str) {

	uint32_t *result = malloc(strlen(str) * CHAR_WIDTH_BITS);
	for (int i=0; i<strlen(str); i++) {
		char c = str[i];
		uint32_t *ri = getVRasterForChar(c);
		for (int j=0; j<CHAR_WIDTH_BITS; j++) {
			result[i * CHAR_WIDTH_BITS + j] = ri[j];
			}
		}
	return result;
	}


uint32_t *getVRaster(int n) {
	return vRasters[n];
	}

uint32_t *getVRasterForChar(char c) {
	return vRasters[c - ' '];
	}


void font_test_1() {
	
	printf("font_test_1\n\n");
	uint32_t *foo;
	foo = getVRaster(1);
	for (int i=0; i<CHAR_WIDTH_BITS; i++) {
		printf(" r[%d] = 0x%02x ", i, foo[i]);
		}
	printf("\n\n");

	foo = getVRaster('?');
	for (int i=0; i<CHAR_WIDTH_BITS; i++) {
		printf(" r['?'] = 0x%02x ", foo[i]);
		}
	printf("\n\n");
	return;
	}

void font_test_2() {

	printf("font_test_2\n\n");
	char *testString = "Test";
	for (int i=0; i<strlen(testString); i++) {
		int ascii = testString[i];
		printf(" char %d is '%d'\n", i, ascii);
		int thisRasterSet[CHAR_WIDTH_BITS];
		for (int c=0; c<CHAR_WIDTH_BITS; c++) {
			thisRasterSet[c] = vRasters[ascii - 0x20][c];
			printf(" r[%d] is %02x\n", c, thisRasterSet[c]);
			}
		}
	}


void font_test_3() {
	
	printf("font_test_3\n\n");
	uint32_t *foo;
	
	foo = vRasters[0x10]; // '0'
	// 	{0x3e, 0x7f, 0x41, 0x49, 0x41, 0x7f, 0x3e, 0x00}, 

	for (int i=0; i<=CHAR_WIDTH_BITS; i++) {
		uint32_t t = foo[i];
		printf(" 0[%d] = 0o%03o \n", i, t);
		printf("  shifted: %012o\n", (t << 6));
		}
	printf("\n");
	}

