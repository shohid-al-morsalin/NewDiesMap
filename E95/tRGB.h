#ifndef __TRGB_H__
#define __TRGB_H__

struct tRGB {
	BYTE B, G, R;

	BYTE I() {
		return (R + 2 * G + B) / 4;
	}
	void Grey(BYTE v) {
		//v = (v<256)?v:255;
		//v = (v<0)? 0:v;
		R = G = B = v;
	}
	void Set(BYTE r, BYTE g, BYTE b) {
		R = r;
		G = g;
		B = b;
	}
	void Dup(tRGB& rgb) {
		R = rgb.R;
		G = rgb.G;
		B = rgb.B;
	}
};

#endif
