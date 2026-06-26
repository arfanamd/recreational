#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEQ 3
#define COL 4

// Theme Colors
#define COLOR_BG      (Color){ 30, 32, 40, 255 }
#define COLOR_CARD    (Color){ 40, 44, 52, 255 }
#define COLOR_ACCENT  (Color){ 97, 175, 239, 255 }
#define COLOR_MINUS   (Color){ 224, 108, 117, 255 } // Soft Red
#define COLOR_PLUS    (Color){ 152, 195, 121, 255 } // Soft Green
#define COLOR_HINT    (Color){ 75, 82, 99, 255 }

int gaussJordan(double *m);

typedef struct {
	char text[16];
	Rectangle bounds;
} InputField;

InputField fields[NEQ * COL];
// Signs for: Eq1(y, z), Eq2(y, z), Eq3(y, z). 1 = Plus, -1 = Minus.
int signs[NEQ][2];
Rectangle signBounds[NEQ][2];

int focusIdx = 0;
char statusMsg[64] = "Click signs to toggle +/-";
Color statusColor = (Color){ 171, 178, 191, 255 };

int main(void) {
	const int screenWidth = 950;
	const int screenHeight = 600;

	InitWindow(screenWidth, screenHeight, "Gauss-Jordan Interactive Solver");
	SetTargetFPS(60);

	// Initialize Fields and Sign Buttons
	for (int i = 0; i < NEQ; i++) {
		for (int j = 0; j < COL; j++) {
			int idx = i * COL + j;
			fields[idx].text[0] = '\0';
			float xPos = 120 + j * 160 + (j == 3 ? 60 : 0);
			fields[idx].bounds = (Rectangle){ xPos, 180 + i * 80, 100, 45 };

			// Create sign toggle bounds between columns
			if (j > 0 && j < 3) {
				signBounds[i][j-1] = (Rectangle){ xPos - 45, 180 + i * 80 + 7, 30, 30 };
				signs[i][j-1] = 1; // Default to Plus
			}
		}
	}

	while (!WindowShouldClose()) {
		Vector2 mouse = GetMousePosition();

		// --- INTERACTION LOGIC ---

		// Handle Sign Toggles
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			for (int i = 0; i < NEQ; i++) {
				for (int j = 0; j < 2; j++) {
					if (CheckCollisionPointRec(mouse, signBounds[i][j])) {
						signs[i][j] *= -1; // Toggle between 1 and -1
					}
				}
			}
			// Handle Box Selection
			for (int i = 0; i < 12; i++) {
				if (CheckCollisionPointRec(mouse, fields[i].bounds)) focusIdx = i;
			}
		}

		// Tab Navigation
		if (IsKeyPressed(KEY_TAB)) {
			focusIdx = (IsKeyDown(KEY_LEFT_SHIFT)) ? (focusIdx + 11) % 12 : (focusIdx + 1) % 12;
		}

		// Typing logic
		int key = GetCharPressed();
		while (key > 0) {
			if ((key >= '0' && key <= '9') || key == '.' || key == '-') {
				int len = strlen(fields[focusIdx].text);
				if (len < 10) { fields[focusIdx].text[len] = (char)key; fields[focusIdx].text[len+1] = '\0'; }
			}
			key = GetCharPressed();
		}
		if (IsKeyPressed(KEY_BACKSPACE)) {
			int len = strlen(fields[focusIdx].text);
			if (len > 0) fields[focusIdx].text[len - 1] = '\0';
		}

		// --- SOLVE LOGIC ---
		if (IsKeyPressed(KEY_ENTER)) {
			double matrix[NEQ * COL];
			for (int i = 0; i < NEQ; i++) {
				for (int j = 0; j < COL; j++) {
					double val = atof(fields[i * COL + j].text);
					// Apply sign logic: y is col 1, z is col 2
					if (j == 1) val *= signs[i][0];
					if (j == 2) val *= signs[i][1];
					matrix[i * COL + j] = val;
				}
			}

			if (gaussJordan(matrix) == 0) {
				// Success: Update UI and reset signs to + to avoid double negatives
				for (int i = 0; i < NEQ; i++) {
					for (int j = 0; j < COL; j++) {
						sprintf(fields[i * COL + j].text, "%.2f", matrix[i * COL + j]);
					}
					signs[i][0] = 1; signs[i][1] = 1;
				}
				strcpy(statusMsg, "Success! Signs reset for readability.");
				statusColor = COLOR_PLUS;
			} else {
				strcpy(statusMsg, "Error: No unique solution.");
				statusColor = COLOR_MINUS;
			}
		}

		// --- DRAWING ---
		BeginDrawing();
		ClearBackground(COLOR_BG);

		// Header
		DrawRectangle(0, 0, screenWidth, 80, COLOR_CARD);
		DrawText("Equation Solver", 40, 25, 30, COLOR_ACCENT);
		DrawText("Click [+] or [-] to change operations", 400, 35, 18, GRAY);

		// Main Workspace
		DrawRectangleRounded((Rectangle){ 40, 110, 870, 400 }, 0.05, 10, COLOR_CARD);
		DrawText(statusMsg, 60, 470, 18, statusColor);

		// Labels
		const char* headers[] = {"x", "y", "z", "Result"};
		for(int i=0; i<4; i++) DrawText(headers[i], 165 + i * 160 + (i == 3 ? 55 : 0), 140, 20, COLOR_ACCENT);

		for (int i = 0; i < NEQ; i++) {
			for (int j = 0; j < COL; j++) {
				int idx = i * COL + j;
				bool isFocused = (focusIdx == idx);

				// Draw Input Box
				DrawRectangleRounded(fields[idx].bounds, 0.2, 10, (Color){ 25, 28, 35, 255 });
				DrawRectangleRoundedLines(fields[idx].bounds, 0.2, 10, isFocused ? COLOR_ACCENT : DARKGRAY);

				if (fields[idx].text[0] == '\0')
					DrawText("0", fields[idx].bounds.x + 10, fields[idx].bounds.y + 12, 22, COLOR_HINT);
				else
					DrawText(fields[idx].text, fields[idx].bounds.x + 10, fields[idx].bounds.y + 12, 22, WHITE);

				// Draw Cursor
				if (isFocused && (int)(GetTime() * 2) % 2 == 0) {
					float tw = (fields[idx].text[0] == '\0') ? 0 : MeasureText(fields[idx].text, 22);
					DrawRectangle(fields[idx].bounds.x + 12 + tw, fields[idx].bounds.y + 10, 2, 25, COLOR_ACCENT);
				}

				// Draw Sign Toggles
				if (j < 2) {
					Rectangle sb = signBounds[i][j];
					bool hov = CheckCollisionPointRec(mouse, sb);
					Color sCol = (signs[i][j] == 1) ? COLOR_PLUS : COLOR_MINUS;

					DrawRectangleRounded(sb, 0.5, 10, hov ? WHITE : sCol);
					DrawText(signs[i][j] == 1 ? "+" : "-", sb.x + 9, sb.y + 4, 24, hov ? BLACK : COLOR_BG);
				}

				if (j == 2) DrawText("=", fields[idx].bounds.x + 120, fields[idx].bounds.y + 10, 26, COLOR_ACCENT);
			}
		}

		// Solve Button UI
		Rectangle solveBtn = { 730, 525, 180, 50 };
		bool btnHov = CheckCollisionPointRec(mouse, solveBtn);
		DrawRectangleRounded(solveBtn, 0.2, 10, btnHov ? COLOR_ACCENT : (Color){ 60, 65, 75, 255 });
		DrawText("SOLVE", solveBtn.x + 55, solveBtn.y + 15, 20, btnHov ? BLACK : WHITE);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}

// Provided Gauss-Jordan function
int gaussJordan(double *m) {
	for (int i = 0; i < NEQ; i++) {
		int pivot = i;
		for (int r = i + 1; r < NEQ; r++) {
			double vR = *(m + r * COL + i); double vP = *(m + pivot * COL + i);
			if ((vR < 0 ? -vR : vR) > (vP < 0 ? -vP : vP)) pivot = r;
		}
		if (*(m + pivot * COL + i) == 0.0) return 1;
		if (pivot != i) {
			for (int k = 0; k < COL; k++) {
				double tmp = *(m + i * COL + k);
				*(m + i * COL + k) = *(m + pivot * COL + k);
				*(m + pivot * COL + k) = tmp;
			}
		}
		double pivotVal = *(m + i * COL + i);
		for (int k = 0; k < COL; k++) *(m + i * COL + k) /= pivotVal;
		for (int r = 0; r < NEQ; r++) {
			if (r == i) continue;
			double factor = *(m + r * COL + i);
			for (int k = 0; k < COL; k++) *(m + r * COL + k) -= factor * *(m + i * COL + k);
		}
	}
	return 0;
}

// vim:ft=c:ts=4:sw=4:nosi:noai:noet:cin
