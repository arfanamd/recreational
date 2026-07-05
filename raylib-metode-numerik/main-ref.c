#include <stdio.h>
#include <string.h>
#include "raylib.h"

#define COLOR1  CLITERAL(Color){ 238, 238, 238, 255 }
#define COLOR7  CLITERAL(Color){   0,   0,   0, 255 }
#define BLACK   CLITERAL(Color){ 027,  38, 044, 255 }
#define NAVY    CLITERAL(Color){ 015, 076, 117, 255 }
#define BLUE    CLITERAL(Color){ 050, 130, 184, 255 }
#define WINTER  CLITERAL(Color){ 187, 225, 250, 255 }

int gaussJordan(double *m) {
	const int NEQ = 3;
	const int COL = 4;
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

void RectwStroke(Rectangle rec, Color color) {
	DrawRectangleRec(rec, color);
	DrawRectangleLinesEx(rec, 1.0f, NAVY);
}

void RectwStrokeGrad(Rectangle rec, Color color1, Color color2) {
	DrawRectangleGradientEx(rec, color1, color2, color2, color1);
	DrawRectangleLinesEx(rec, 2.0f, NAVY);
}

void RoundedRectwStroke(Rectangle rec, Color color) {
	DrawRectangleRounded(rec, 0.1f, 0, color);
	DrawRectangleRoundedLinesEx(rec, 0.1f, 0, 5.0f, color);
}

typedef enum {
	GAUSS,
	SECANT,
	LAGRANGE,
	EULER,
	NOOP,
} Select;

const size_t screenWidth  = 1080;
const size_t screenHeight = 720;

const Rectangle winAboutRec = {
	.x = 1,
	.y = 1,
	.width = screenWidth * 30 / 100,
	.height = screenHeight * 30 / 100,
};
const Rectangle winMenuRec = {
	.x = 1,
	.y = winAboutRec.height,
	.width = screenWidth * 30 / 100,
	.height = screenHeight - winAboutRec.height,
};
const Rectangle winTitleRec = {
	.x = winAboutRec.width,
	.y = 1,
	.width = screenWidth - (screenWidth * 30 / 100),
	.height = screenHeight * 10 /100,
};
const Rectangle winEquationRec = {
	.x = winAboutRec.width,
	.y = winTitleRec.height,
	.width = screenWidth - (screenWidth * 30 / 100),
	.height = screenHeight - winTitleRec.height,
};

const Vector2 iconBannerPos = {
	.x = 100,
	.y = winAboutRec.height * 5 / 100,
};

const Vector2 menuPos = {
	.x = 30,
	.y = winAboutRec.height + 20,
};

const Vector2 tboxMainMenu = {
	.x = winMenuRec.x + (winMenuRec.width * 10 / 100),
	.y = winMenuRec.y + 35,
};

const Vector2 tboxMenuGauss = {
	.x = winMenuRec.x + (winMenuRec.width * 10 / 100),
	.y = tboxMainMenu.y + 45,
};
const Rectangle rboxMenuGauss = {
	.x = tboxMenuGauss.x,
	.y = tboxMenuGauss.y,
	.width = winMenuRec.width - tboxMenuGauss.x - 30,
	.height = 20,
};

const Vector2 tboxMenuSecant = {
	.x = winMenuRec.x + (winMenuRec.width * 10 / 100),
	.y = tboxMenuGauss.y + 35,
};
const Rectangle rboxMenuSecant = {
	.x = tboxMenuSecant.x,
	.y = tboxMenuSecant.y,
	.width = winMenuRec.width - tboxMenuSecant.x - 130,
	.height = 20,
};

const Vector2 tboxMenuLagrange = {
	.x = winMenuRec.x + (winMenuRec.width * 10 / 100),
	.y = tboxMenuSecant.y + 35,
};
const Rectangle rboxMenuLagrange = {
	.x = tboxMenuLagrange.x,
	.y = tboxMenuLagrange.y,
	.width = winMenuRec.width - tboxMenuLagrange.x - 100,
	.height = 20,
};

const Vector2 tboxMenuEuler = {
	.x = winMenuRec.x + (winMenuRec.width * 10 / 100),
	.y = tboxMenuLagrange.y + 35,
};
const Rectangle rboxMenuEuler = {
	.x = tboxMenuEuler.x,
	.y = tboxMenuEuler.y,
	.width = winMenuRec.width - tboxMenuEuler.x - 150,
	.height = 20,
};

const Vector2 tboxMenuExit = {
	.x = winMenuRec.x + (winMenuRec.width * 10 / 100),
	.y = tboxMenuEuler.y + 35,
};
const Rectangle rboxMenuExit = {
	.x = tboxMenuExit.x,
	.y = tboxMenuExit.y,
	.width = winMenuRec.width - tboxMenuExit.x - 240,
	.height = 20,
};

const Vector2 tboxTitle = {
	.x = winTitleRec.x + 30,
	.y = winTitleRec.y + 30,
};
const Rectangle rboxTitle = {
	.x = tboxTitle.x,
	.y = tboxTitle.y,
	.width = winTitleRec.width - 10,
	.height = winTitleRec.height - 10,
};

const Rectangle rboxGaussInput1 = {
	.x = winEquationRec.x + 40,
	.y = winEquationRec.y + 40,
	.width = 400,
	.height = 50,
};

void GaussMode(Vector2 mouseP, Font font) {
	const Rectangle rboxGauss = {
		.x = winEquationRec.x + 30,
		.y = winEquationRec.y + 30,
		.width = winEquationRec.width - (winEquationRec.width * 8 / 100),
		.height = winEquationRec.height - (winEquationRec.height * 8 / 100),
	};
	const Rectangle GaussInput1 = {
		.x = rboxGauss.x + 30,
		.y = rboxGauss.y + 20,
		.width = rboxGauss.width * 80 / 100,
		.height = rboxGauss.height * 8 / 100,
	};
	const Vector2 tGaussInput1 = {
		.x = GaussInput1.x + 10,
		.y = GaussInput1.y + (GaussInput1.height * 40 / 100),
	};

	char input1[128] = "";
	const char *suggested1 = "Enter value...";
	RoundedRectwStroke(GaussInput1, BLUE);
	DrawTextEx(font, suggested1, tGaussInput1, 20.0f, 2.0f, NAVY);

	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		if (CheckCollisionPointRec(mouseP, GaussInput1)) {
			fprintf(stderr, "[DEBUG]: Gauss box1 pressed\n");
			RoundedRectwStroke(GaussInput1, BLACK);
			int ch;
			while ((ch = GetCharPressed()) > 0) {
				const size_t len = strlen(input1);
				if (len < sizeof(input1) - 1) {
					input1[len] = (char)ch;
					input1[len + 1] = '\0';
				}
			}
		}
	}
	DrawTextEx(font, input1, tGaussInput1, 20.0f, 2.0f, NAVY);

	const Rectangle GaussInput2 = {
		.x = GaussInput1.x,
		.y = GaussInput1.height + GaussInput1.y + 20,
		.width = rboxGauss.width * 80 / 100,
		.height = rboxGauss.height * 10 / 100,
	};
	RoundedRectwStroke(GaussInput2, BLUE);

	const Rectangle GaussInput3 = {
		.x = GaussInput2.x,
		.y = GaussInput2.height + GaussInput2.y + 20,
		.width = rboxGauss.width * 80 / 100,
		.height = rboxGauss.height * 10 / 100,
	};
	RoundedRectwStroke(GaussInput3, BLUE);
}

int main(void) {
	InitWindow(
		screenWidth,
		screenHeight,
		"Metode Numerik"
	);
	SetTargetFPS(30);

	const Image iconBanner = LoadImage("assets/iconBanner.png");
	const Texture2D iconBannerTex = LoadTextureFromImage(iconBanner);

	const Vector2 tboxAboutPos = {
		.x = 50,
		.y = iconBannerPos.y + iconBanner.height + 10,
	};

	// const Font font = LoadFont("assets/fonts/LoveYaLikeASister-Regular.ttf");
	const Font font = LoadFont("assets/fonts/Azeret_Mono/static/AzeretMono-Bold.ttf");
	// const Font font = LoadFont("assets/fonts/Workbench.ttf");

	const char *tboxMessage = "Nama: Ahmad Arfan Maulana\nNPM: 202443500619\nMetode Numerik";


	Vector2 mouseP = {0};
	char *menuSelected = "";
	Select select = NOOP;

	while (!WindowShouldClose()) {
		// BeginDrawing();
		mouseP = GetMousePosition();
		{
			ClearBackground(COLOR7);
			RectwStroke(winAboutRec, BLUE);
			RectwStroke(winMenuRec, WINTER);
			RectwStroke(winTitleRec, BLUE);
			RectwStroke(winEquationRec, WINTER);

			DrawTextureV(iconBannerTex, iconBannerPos, COLOR1);
			DrawTextEx(font, tboxMessage, tboxAboutPos, 14.0f, 2.0f, WINTER);

			DrawTextEx(font, "Main Menu", tboxMainMenu, 20.0f, 2.0f, NAVY);

			DrawTextEx(font, "Metode Eliminasi Gauss", tboxMenuGauss, 20.0f, 2.0f, NAVY);
			if (CheckCollisionPointRec(mouseP, rboxMenuGauss)) {
				DrawTextEx(font, "Metode Eliminasi Gauss", tboxMenuGauss, 20.0f, 2.0f, BLACK);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					menuSelected = "Metode Eliminasi Gauss";
					select = GAUSS;
					fprintf(stderr, "[DEBUG]: Metode Eliminasi Gauss\n");
				}
			}

			DrawTextEx(font, "Metode Secant", tboxMenuSecant, 20.0f, 2.0f, NAVY);
			if (CheckCollisionPointRec(mouseP, rboxMenuSecant)) {
				DrawTextEx(font, "Metode Secant", tboxMenuSecant, 20.0f, 2.0f, BLACK);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					menuSelected = "Metode Secant";
					fprintf(stderr, "[DEBUG]: Metode Secant\n");
				}
			}

			DrawTextEx(font, "Metode Lagrange", tboxMenuLagrange, 20.0f, 2.0f, NAVY);
			if (CheckCollisionPointRec(mouseP, rboxMenuLagrange)) {
				DrawTextEx(font, "Metode Lagrange", tboxMenuLagrange, 20.0f, 2.0f, BLACK);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					menuSelected = "Metode Lagrange";
					fprintf(stderr, "[DEBUG]: Metode Lagrange\n");
				}
			}

			DrawTextEx(font, "Metode Euler", tboxMenuEuler, 20.0f, 2.0f, NAVY);
			if (CheckCollisionPointRec(mouseP, rboxMenuEuler)) {
				DrawTextEx(font, "Metode Euler", tboxMenuEuler, 20.0f, 2.0f, BLACK);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					menuSelected = "Metode Euler";
					fprintf(stderr, "[DEBUG]: Metode Euler\n");
				}
			}

			DrawTextEx(font, "Exit", tboxMenuExit, 20.0f, 2.0f, NAVY);
			if (CheckCollisionPointRec(mouseP, rboxMenuExit)) {
				DrawTextEx(font, "Exit", tboxMenuExit, 20.0f, 2.0f, BLACK);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					goto quit;
				}
			}

			DrawTextEx(font, menuSelected, tboxTitle, 32.0f, 2.0f, WINTER);
		}

		switch (select) {
			case GAUSS:
				fprintf(stderr, "[DEBUG]: gauss is selected\n");
				GaussMode(mouseP, font);
				break;
			default:
				break;
		}
		BeginDrawing();

		EndDrawing();
		if (IsKeyPressed(KEY_Q)) goto quit;
	}

quit:
	UnloadImage(iconBanner);
	UnloadTexture(iconBannerTex);
	UnloadFont(font);
	CloseWindow();
	return 0;
}

// vim:ft=c:ts=4:sw=4:nosi:noai:noet:cin