#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>

// basic window-based SDL stuff
#define windowWidth 1000
#define windowHeight 1000
#define halfWidth 500
#define halfHeight 500

int FPS = 120;

int appRunning = 0;
SDL_Window* window = NULL; SDL_Renderer* renderer = NULL; SDL_Texture* texture = NULL;

int numThreads = 10;

// rendering data storage
char* p; int pitch;

int threadIndices[100];
int* pthreadIndices[100];


// time and frame-based stuff
uint64_t timeStorage;
uint64_t timeNow;
uint64_t lastFrameTime = 0;
int frameTargetTime;
double deltaTime;
int frame = 0;

char mouseLeftDown = 0;
char mouseLeftUp = 0;
char mouseLeftOn = 0;
char mouseRightDown = 0;
char mouseRightUp = 0;
char mouseRightOn = 0;

char wDown = 0;
char aDown = 0;
char sDown = 0;
char dDown = 0;

char wUp = 0;
char aUp = 0;
char sUp = 0;
char dUp = 0;

char wOn = 0;
char aOn = 0;
char sOn = 0;
char dOn = 0;

char upDown = 0;
char leftDown = 0;
char downDown = 0;
char rightDown = 0;

char upUp = 0;
char leftUp = 0;
char downUp = 0;
char rightUp = 0;

char upOn = 0;
char leftOn = 0;
char downOn = 0;
char rightOn = 0;

char dotDown = 0;
char slashDown = 0;

char dotUp = 0;
char slashUp = 0;

char dotOn = 0;
char slashOn = 0;

int mouseX; int mouseY; int lastMouseX; int lastMouseY; double mouseSpeedX; double mouseSpeedY;

unsigned char inR = 0;
unsigned char inG = 0;
unsigned char inB = 0;
unsigned char outR = 255;
unsigned char outG = 255;
unsigned char outB = 255;

char seq[150000000];
int seqLength = 0;

#define gridSize 2500

char grid[gridSize * gridSize];

char direction = 0;

double zoom = 1.0;
double cornerx = 0.0;
double cornery = 0.0;

#define maxNumLines 40000000
int numLines = 0;
int iteration = 0;
char lineOrs[maxNumLines];
int lineX[maxNumLines];
int lineY[maxNumLines];

char infiniteZoom = 1;
char printNumLines = 0;


static uint64_t getTimeSinceEpoch(void) {
	struct timespec ts;
	if (timespec_get(&ts, TIME_UTC) != TIME_UTC) { fputs("timespec_get failed!", stderr); return 0; }
	return 1000000000 * ts.tv_sec + ts.tv_nsec;
}

static void startTimer(void) {
	timeStorage = getTimeSinceEpoch();
}

static void stopTimer(void) {
	printf("Timer result: %i\n", (int)((getTimeSinceEpoch() - timeStorage) / (uint64_t)1000));
}

static void printFPS(void) { printf("FPS: %f\n", 1.0f / deltaTime); }

int initWindow(void) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return 0;
	}
	window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);
	if (!window) { fprintf(stderr, "Error creating SDL window.\n"); return 0; }
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) { fprintf(stderr, "Error creating SDL renderer.\n"); return 0; }
	return 1;
}

void processInput() {
	SDL_Event event;

	mouseLeftDown = 0; mouseLeftUp = 0; mouseRightDown = 0; mouseRightUp = 0;
	lastMouseX = mouseX; lastMouseY = mouseY; mouseSpeedX = 0; mouseSpeedY = 0;
	upDown = 0; upUp = 0;
	leftDown = 0; leftUp = 0;
	downDown = 0; downUp = 0;
	rightDown = 0; rightUp = 0;
	wDown = 0; wUp = 0;
	aDown = 0; aUp = 0;
	sDown = 0; sUp = 0;
	dDown = 0; dUp = 0;
	dotDown = 0; dotUp = 0;
	slashDown = 0; slashUp = 0;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {

		case SDL_QUIT:
			appRunning = 0; break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_w:
				if (!wOn) { wDown = 1; } wOn = 1; wUp = 0;
				break;
			case SDLK_a:
				if (!aOn) { aDown = 1; } aOn = 1; aUp = 0;
				break;
			case SDLK_s:
				if (!sOn) { sDown = 1; } sOn = 1; sUp = 0;
				break;
			case SDLK_d:
				if (!dOn) { dDown = 1; } dOn = 1; dUp = 0;
				break;
			case SDLK_UP:
				if (!upOn) { upDown = 1; } upOn = 1; upUp = 0;
				break;
			case SDLK_LEFT:
				if (!leftOn) { leftDown = 1; } leftOn = 1; leftUp = 0;
				break;
			case SDLK_DOWN:
				if (!downOn) { downDown = 1; } downOn = 1; downUp = 0;
				break;
			case SDLK_RIGHT:
				if (!rightOn) { rightDown = 1; } rightOn = 1; rightUp = 0;
				break;
			case SDLK_PERIOD:
				if (!dotOn) { dotDown = 1; } dotOn = 1; dotUp = 0;
				break;
			case SDLK_SLASH:
				if (!slashOn) { slashDown = 1; } slashOn = 1; slashUp = 0;
				break;
			} break;

		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
			case SDLK_w:
				if (wOn) { wUp = 1; } wOn = 0; wDown = 0;
				break;
			case SDLK_a:
				if (aOn) { aUp = 1; } aOn = 0; aDown = 0;
				break;
			case SDLK_s:
				if (sOn) { sUp = 1; } sOn = 0; sDown = 0;
				break;
			case SDLK_d:
				if (dOn) { dUp = 1; } dOn = 0; dDown = 0;
				break;
			case SDLK_UP:
				if (upOn) { upUp = 1; } upOn = 0; upDown = 0;
				break;
			case SDLK_LEFT:
				if (leftOn) { leftUp = 1; } leftOn = 0; leftDown = 0;
				break;
			case SDLK_DOWN:
				if (downOn) { downUp = 1; } downOn = 0; downDown = 0;
				break;
			case SDLK_RIGHT:
				if (rightOn) { rightUp = 1; } rightOn = 0; rightDown = 0;
				break;
			case SDLK_PERIOD:
				if (dotOn) { dotUp = 1; } dotOn = 0; dotDown = 0;
				break;
			case SDLK_SLASH:
				if (slashOn) { slashUp = 1; } slashOn = 0; slashDown = 0;
				break;
			} break;

		case SDL_MOUSEMOTION:
			mouseX = event.motion.x;
			mouseY = event.motion.y;
			mouseSpeedX = event.motion.xrel;
			mouseSpeedY = event.motion.yrel; break;

		case SDL_MOUSEBUTTONDOWN:
			if (event.button.button == SDL_BUTTON_RIGHT) { if (!mouseRightOn) { mouseRightDown = 1; } mouseRightOn = 1; mouseRightUp = 0; }
			if (event.button.button == SDL_BUTTON_LEFT) { if (!mouseLeftOn) { mouseLeftDown = 1; } mouseLeftOn = 1; mouseLeftUp = 0; }

			break;

		case SDL_MOUSEBUTTONUP:
			if (event.button.button == SDL_BUTTON_RIGHT) { if (mouseRightOn) { mouseRightUp = 1; } mouseRightOn = 0; mouseRightDown = 0; }
			if (event.button.button == SDL_BUTTON_LEFT) { if (mouseLeftOn) { mouseLeftUp = 1; } mouseLeftOn = 0; mouseLeftDown = 0; }

			break;
		}
	}
}

void clearBuffer() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}

void magnifyAllLines() {
	int cx = (double)(halfWidth - cornerx) / zoom;
	int cy = (double)(halfHeight - cornery) / zoom;
	for (int i = 0; i < numLines; i++) {
		lineX[i] = cx + 2 * (lineX[i] - cx);
		lineY[i] = cy + 2 * (lineY[i] - cy);
	}
}

void replaceLine(int l) {
	char o = lineOrs[l];
	int n = numLines;

	char ors[8][4] = { {0,3,0,1},{2,1,0,1},{2,1,2,3},{0,3,2,3}, {1,0,3,0},{1,0,1,2},{3,2,1,2},{3,2,3,0} };
	lineOrs[l] = ors[(iteration % 2) * 4 + o][0];
	lineOrs[n] = ors[(iteration % 2) * 4 + o][1];
	lineOrs[n + 1] = ors[(iteration % 2) * 4 + o][2];
	lineOrs[n + 2] = ors[(iteration % 2) * 4 + o][3];

	if (iteration % 2) {
		switch (o) {
		case 0:
			lineX[n] = lineX[l];
			lineY[n] = lineY[l] - 1;
			lineX[n + 1] = lineX[n] + 1;
			lineY[n + 1] = lineY[n];
			lineX[n + 2] = lineX[n + 1];
			lineY[n + 2] = lineY[n + 1] + 1;
			break;
		case 1:
			lineX[n] = lineX[l];
			lineY[n] = lineY[l] - 1;
			lineX[n + 1] = lineX[n] + 1;
			lineY[n + 1] = lineY[n];
			lineX[n + 2] = lineX[n + 1];
			lineY[n + 2] = lineY[n + 1] - 1;
			break;
		case 2:
			lineX[n] = lineX[l];
			lineY[n] = lineY[l] + 1;
			lineX[n + 1] = lineX[n] - 1;
			lineY[n + 1] = lineY[n];
			lineX[n + 2] = lineX[n + 1];
			lineY[n + 2] = lineY[n + 1] - 1;
			break;
		case 3:
			lineX[n] = lineX[l];
			lineY[n] = lineY[l] + 1;
			lineX[n + 1] = lineX[n] - 1;
			lineY[n + 1] = lineY[n];
			lineX[n + 2] = lineX[n + 1];
			lineY[n + 2] = lineY[n + 1] + 1;
			break;
		}
	}else{
		switch (o) {
		case 0:
			lineX[n] = lineX[l] + 1;
			lineY[n] = lineY[l];
			lineX[n + 1] = lineX[n];
			lineY[n + 1] = lineY[n] + 1;
			lineX[n + 2] = lineX[n + 1] + 1;
			lineY[n + 2] = lineY[n + 1];
			break;
		case 1:
			lineX[n] = lineX[l] - 1;
			lineY[n] = lineY[l];
			lineX[n + 1] = lineX[n];
			lineY[n + 1] = lineY[n] - 1;
			lineX[n + 2] = lineX[n + 1] + 1;
			lineY[n + 2] = lineY[n + 1];
			break;
		case 2:
			lineX[n] = lineX[l] - 1;
			lineY[n] = lineY[l];
			lineX[n + 1] = lineX[n];
			lineY[n + 1] = lineY[n] - 1;
			lineX[n + 2] = lineX[n + 1] - 1;
			lineY[n + 2] = lineY[n + 1];
			break;
		case 3:
			lineX[n] = lineX[l] + 1;
			lineY[n] = lineY[l];
			lineX[n + 1] = lineX[n];
			lineY[n + 1] = lineY[n] + 1;
			lineX[n + 2] = lineX[n + 1] - 1;
			lineY[n + 2] = lineY[n + 1];
			break;
		}
	}

	numLines += 3;
}

void deleteLines() {
	double sx = 0.0;
	double sy = 0.0;

	int del = 0;
	int n = numLines;

	// delete the lines that are outside the screen by replacing their values with the next line that is inside screen
	int low = 0; int hi = 1;
	while (low < numLines) {

		// find next low that is outside
		sx = (double)lineX[low] * zoom + cornerx;
		sy = (double)lineY[low] * zoom + cornery;
		while (sx >= -1.0 && sx < (double)windowWidth + 1.0 && sy >= -1.0 && sy < (double)windowHeight + 1.0) {
			// increment low by 1 if still inside
			low++;
			// if we're out of lines to replace (and the last line is inside), end it
			if (low >= numLines) {
				numLines = n;
				return;
			}
			sx = (double)lineX[low] * zoom + cornerx;
			sy = (double)lineY[low] * zoom + cornery;
		}

		// find next hi that is inside, starting at low + 1
		if (hi <= low) {
			hi = low + 1;
		}
		// if we're out of lines to replace with, end it
		if (hi >= numLines) {
			numLines = low;
			return;
		}
		sx = (double)lineX[hi] * zoom + cornerx;
		sy = (double)lineY[hi] * zoom + cornery;
		while (sx < -1.0 || sx >= (double)windowWidth + 1.0 || sy < -1.0 || sy >= (double)windowHeight + 1.0) {
			// increment hi by 1 if still outside
			hi++;
			// if we're out of lines to replace with (and the last line is outside), end it
			if (hi >= numLines) {
				numLines = low;
				return;
			}
			sx = (double)lineX[hi] * zoom + cornerx;
			sy = (double)lineY[hi] * zoom + cornery;
		}

		// once the hi line is inside the screen, set the low line to the values of the hi line and increment low and hi by 1
		lineX[low] = lineX[hi];
		lineY[low] = lineY[hi];
		lineOrs[low++] = lineOrs[hi];
		lineX[hi] = -999999999;
		lineY[hi] = -999999999;
		lineOrs[hi++] = 0;
		del++;
		n--;
	}
	if (printNumLines) {
		printf("Deleted %i lines\n", del);
	}
}

void emptyGrid(int* ind) {

	int start = (gridSize * ind[0]) / numThreads;
	int end = (gridSize * (ind[0] + 1)) / numThreads;

	// reset the grid to all 0s
	int h = 0;
	for (int y = start; y < end; y++) {
		h = y * gridSize;
		for (int x = 0; x < gridSize; x++) {
			grid[h + x] = 0;
		}
	}
}

void rasterizeLines() {
	int x = 0; int y = 0;

	if (numThreads > 1) {

		SDL_Thread* threads[20] = { NULL }; for (int i = 0; i < numThreads; i++) { threads[i] = SDL_CreateThread(emptyGrid, "thr", pthreadIndices[i]); }
		for (int i = 0; i < numThreads; i++) { SDL_WaitThread(threads[i], NULL); }

	}
	else {

		// reset the grid to all 0s
		int h = 0;
		for (y = 0; y < gridSize; y++) {
			h = y * gridSize;
			for (x = 0; x < gridSize; x++) {
				grid[h + x] = 0;
			}
		}

	}

	deleteLines();

	// plot all lines as points on the grid
	for (int i = 0; i < numLines; i++) {
		x = lineX[i];
		y = lineY[i];
		if (x >= 0 && x < gridSize && y >= 0 && y < gridSize) {
			grid[y * gridSize + x] = 1;
		}
	}
}

void renderBuffer() {

	int pi;

	int gridx = 0;
	int gridy = 0;

	for (int y = 0; y < windowHeight; y++) {
		for (int x = 0; x < windowWidth; x++) {

			gridx = (int)(((double)x - cornerx) / zoom);
			gridy = (int)(((double)y - cornery) / zoom);

			if (gridx >= 0 && gridx < gridSize && gridy >= 0 && gridy < gridSize) {

				if (grid[gridy * gridSize + gridx]) {
					pi = (y * windowWidth + x) * 4;
					p[pi + 2] = inR; p[pi + 1] = inG; p[pi] = inB;
				}
				else {
					pi = (y * windowWidth + x) * 4;
					p[pi + 2] = outR; p[pi + 1] = outG; p[pi] = outB;
				}
			}
			else {
				pi = (y * windowWidth + x) * 4;
				p[pi + 2] = 0; p[pi + 1] = 0; p[pi] = 0;
			}
		}
	}
}

void delay() {
	if (frameTargetTime > timeNow - lastFrameTime) { SDL_Delay((int)((frameTargetTime - (timeNow - lastFrameTime)) / 1000000)); }
	deltaTime = (double)((int)(getTimeSinceEpoch() - lastFrameTime)) / 1000000000.0; // multiply this value by /second values to convert to /frame values
}

void renderScreen() {
	SDL_RenderPresent(renderer);
}

void destroyWindow() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void updateVars() {

	double prevzoom = 0.0;
	double nx = 0.0;
	double ny = 0.0;
	double cx = 0.0;
	double cy = 0.0;

	// mouse dragging
	if (mouseLeftOn) {
		cornerx += (double)mouseSpeedX;
		cornery += (double)mouseSpeedY;
	}

	// zooming
	if (aOn && deltaTime < 0.1) {

		if (infiniteZoom || zoom <= 100.0) {
			prevzoom = zoom;
			zoom *= 1.0 + 1.8 * deltaTime;
			cornerx = (double)halfWidth - ((double)(halfWidth - cornerx) / prevzoom) * zoom;
			cornery = (double)halfHeight - ((double)(halfHeight - cornery) / prevzoom) * zoom;
		}
		
		if (zoom >= 1.0 && infiniteZoom) {

			deleteLines();

			// cut the zoom in half
			prevzoom = zoom;
			zoom *= 0.5;
			cornerx = (double)halfWidth - ((double)(halfWidth - cornerx) / prevzoom) * zoom;
			cornery = (double)halfHeight - ((double)(halfHeight - cornery) / prevzoom) * zoom;
			

			iteration++;

			magnifyAllLines();

			int n = numLines;

			for (int i = 0; i < n; i++) {
				replaceLine(i);
			}

			rasterizeLines();
			if (printNumLines) {
				printf("%i lines\n", numLines);
			}
		}
	}
	if (sOn && !infiniteZoom) {
		prevzoom = zoom;
		zoom /= 1.0 + 1.8 * deltaTime;
		cornerx = (double)halfWidth - ((double)(halfWidth - cornerx) / prevzoom) * zoom;
		cornery = (double)halfHeight - ((double)(halfHeight - cornery) / prevzoom) * zoom;
	}

	
	if (upDown) {
		prevzoom = zoom;
		zoom *= 1.6;
		cornerx = (double)halfWidth - (((double)halfWidth - cornerx) / prevzoom) * zoom;
		cornery = (double)halfHeight - (((double)halfHeight - cornery) / prevzoom) * zoom;
	}
	if (downDown) {
		prevzoom = zoom;
		zoom /= 1.6;
		cornerx = (double)halfWidth - (((double)halfWidth - cornerx) / prevzoom) * zoom;
		cornery = (double)halfHeight - (((double)halfHeight - cornery) / prevzoom) * zoom;
	}
}

void getGrid() {
	for (int i = 0; i < gridSize * gridSize; i++) {
		grid[i] = 0;
	}

	numLines = 1;
	lineOrs[0] = 0;
	lineX[0] = gridSize / 2;
	lineY[0] = gridSize / 2;

	int n = numLines;

	// iterate several times to get the dragon curve
	for (iteration = 1; iteration < 11; iteration++) {
		magnifyAllLines();

		n = numLines;

		for (int j = 0; j < n; j++) {
			replaceLine(j);
		}
	}

	rasterizeLines();
	iteration++;
}


int main(int argc, char* args[]) {
	appRunning = initWindow();

	p = (char*)malloc(sizeof(char) * windowWidth * windowHeight * 4);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, windowWidth, windowHeight);

	lastFrameTime = getTimeSinceEpoch();

	for (int i = 0; i < numThreads; i++) { threadIndices[i] = i; pthreadIndices[i] = &threadIndices[i]; }

	zoom = (double)windowHeight / (double)gridSize;
	cornerx = ((double)windowWidth - (double)gridSize * zoom) / 2.0;
	cornery = 0.0;

	getGrid();

	// uncomment to print the number of lines being rendered
	// printNumLines = 1;

	// uncomment to disable the infinite zoom and allow zooming in and out
	// infiniteZoom = 0;

	while (appRunning) {
		lastFrameTime = getTimeSinceEpoch();
		frame++;
		renderScreen();
		processInput();
		updateVars();
		clearBuffer();
		SDL_LockTexture(texture, NULL, &p, &pitch);
		renderBuffer();
		SDL_UnlockTexture(texture);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		timeNow = getTimeSinceEpoch();
		delay();
		//printFPS();
	}
	destroyWindow();
	return 0;
}
