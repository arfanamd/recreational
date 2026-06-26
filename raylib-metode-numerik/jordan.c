// clang example.c -o example -L./raylib-6.0_linux_amd64/lib -I./raylib-6.0_linux_amd64/include -lraylib -Wl,-rpath,raylib-6.0_linux_amd64/lib
#include "raylib.h"
#include <stdio.h>

#define NEQ 3
#define COL 4

// Theme Colors
#define COLOR_BG      CLITERAL(Color){ 30, 32, 40, 255 }
#define COLOR_CARD    CLITERAL(Color){ 40, 44, 52, 255 }
#define COLOR_ACCENT  CLITERAL(Color){ 97, 175, 239, 255 }
#define COLOR_MINUS   CLITERAL(Color){ 224, 108, 117, 255 } // Soft Red
#define COLOR_PLUS    CLITERAL(Color){ 152, 195, 121, 255 } // Soft Green
#define COLOR_HINT    CLITERAL(Color){ 75, 82, 99, 255 }
#define COLOR1        CLITERAL(Color){ 238, 238, 238, 255 }
#define COLOR2        CLITERAL(Color){ 221, 221, 221, 255 }
#define COLOR3        CLITERAL(Color){ 203,  41,  87, 255 }
#define COLOR4        CLITERAL(Color){   0,   0,   0, 255 }

int gaussJordan(double *m) {
	for (int i = 0; i < NEQ; i++) {
		int pivot = i;
		
		// pivot selection by absolute value
		for (int r = i + 1; r < NEQ; r++) {
			double vR = *(m + r * COL + i);
			double vP = *(m + pivot * COL + i);
			if ((vR < 0 ? -vR : vR) > (vP < 0 ? -vP : vP)) pivot = r;
		}
		
		// no solution. 0 pivot.
		if (*(m + pivot * COL + i) == 0.0) return 1;
		
		// If the best pivot row isn’t the current row, it swaps them so
		// the pivot is in the right place.
		if (pivot != i) {
			for (int k = 0; k < COL; k++) {
				double tmp = *(m + i * COL + k);
				*(m + i * COL + k) = *(m + pivot * COL + k);
				*(m + pivot * COL + k) = tmp;
			}
		}
		
		// Scale pivot row to make pivot = 1
		double pivotVal = *(m + i * COL + i);
		for (int k = 0; k < COL; k++) *(m + i * COL + k) /= pivotVal;
		
		// Eliminate this column in all other rows
		// This ensures you end up with 0s above and below the pivot
		for (int r = 0; r < NEQ; r++) {
			if (r == i) continue;
			double factor = *(m + r * COL + i);
			for (int k = 0; k < COL; k++) *(m + r * COL + k) -= factor * *(m + i * COL + k);
		}
	}
	
	return 0;
}

typedef struct {
	char *text;
	Rectangle bounds;
} InputField;

// Construct input fields
InputField fields[NEQ * COL];
int signs[NEQ][2];
Rectangle signBounds[NEQ][2];

int main(void) {
	const size_t screenWidth  = 1080;
	const size_t screenHeight = 720;
	
	InitWindow(screenWidth, screenHeight, "Metode Numerik");
	SetTargetFPS(30);
	
	while (!WindowShouldClose()) {
		// track mouse movements
		Vector2 mouse = GetMousePosition();
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			for (int i = 0; i < NEQ; i++) {
				for (int j = 0; j < 2; j++) {
					if (CheckCollisionPointRec(mouse, )) {
						//
					}
				}
			}
		}
	}
	
	return 0;
}

// vim:ft=c:ts=4:sw=4:nosi:noai:noet:cin
