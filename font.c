/**
	font.c
	Horizontal and vertical raster data for a simple 8x8 font.
	Taken from 'font.h', according to my notes - but where was that from???
**/

#include <string.h>
#include <stdio.h>

#include "font_data.h"


const int *getVRaster(int n) {
	return vRasters[n];
	}

const int *getVRasterForChar(char c) {
	return vRasters[c - ' '];
	}

void font_test() {

	const int *foo = getVRaster(1);
	for (int i=0; i<8; i++) {
		printf(" r[%d] = 0x%02x ", i, foo[i]);
		}
	printf("\n\n");

	const int *fum = getVRaster('?');
	for (int i=0; i<8; i++) {
		printf(" r['?'] = 0x%02x ", fum[i]);
		}
	printf("\n\n");
	
	return;

	char * testString = "Test";

	for (int i=0; i<strlen(testString); i++) {
		int ascii = testString[i];
		printf("\nchar %d is %d\n", i, ascii);
		int thisRasterSet[8];
		for (int c=0; c<8; c++) {
			thisRasterSet[c] = hRasters[ascii - 0x20][c];
			printf(" r[%d] is %02x\n", c, thisRasterSet[c]);
		} 
	}


}
