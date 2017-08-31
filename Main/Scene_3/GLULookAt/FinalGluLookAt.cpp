
//Include headers
#include<Windows.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#include<math.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"Glu32.lib")

//WndProc declared globally
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

#define WIN_WIDTH  800
#define WIN_HEIGHT  600

//Global Variables
DWORD style;
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool gbActiveWindow = false;
bool gbEscapeKeyPressed = false;
bool gbFullScreen = false;

GLfloat windowWidth;
GLfloat windowHeight;

GLfloat angle = 0.0f;
GLfloat prevAngle = 0.0f;

struct CubeSize {
	GLfloat xCor = 0.3f;
	GLfloat yCor = 0.3f;
	GLfloat zCor = 0.3f;
}cubeSize;

struct RoomSize {
	GLfloat xCor = 5.0f;
	GLfloat yCor = 5.0f;
	GLfloat zCor = 5.0f;
}roomSize;

struct CameraPosition {
	GLfloat x = 0.0f;
	GLfloat y = 1.0f;
	GLfloat z = 5.0f;
}cameraPosition;

int lookAtAxis = 1.0f;
bool isRotating = false;

GLfloat zoom = 5.0f;

//Main
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLin, int iCmdShow) {
	void initialize(void);
	void uninitialize(void);
	void display(void);
	void spin(void);
	//Local Vaiables
	WNDCLASSEX wndClassEx;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("OpenGL Assignment");
	bool bDone = false;

	//Intialize WNDCLASS
	wndClassEx.cbSize = sizeof(WNDCLASSEX);
	wndClassEx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbWndExtra = 0;
	wndClassEx.hInstance = hInstance;
	wndClassEx.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClassEx.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wndClassEx.lpfnWndProc = WndProc;
	wndClassEx.lpszClassName = szClassName;
	wndClassEx.lpszMenuName = NULL;

	RegisterClassEx(&wndClassEx);

	//CreateWindow
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szClassName,
		TEXT("Opengl Assignment"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		0,
		0,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);
	ghwnd = hwnd;
	initialize();

	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	while (bDone == false) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				bDone = true;
			}
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else {

			if (gbActiveWindow == true) {
				spin();
				display();
				if (gbEscapeKeyPressed == true) {
					bDone = true;
				}
			}
		}
	}
	uninitialize();
	return(int(msg.wParam));
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	void display(void);
	void resize(int, int);
	void toggleFullScreen(void);
	void uninitialised(void);
	void spin();

	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0)
			gbActiveWindow = true;
		else
			gbActiveWindow = false;
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			if (gbEscapeKeyPressed == false){
				gbEscapeKeyPressed = true;
			}
			break;
		case VK_UP:
			isRotating == false;
			lookAtAxis = 1;
			cameraPosition.x = 0.0f;
			cameraPosition.y = cos(angle) * zoom;
			cameraPosition.z = sin(angle) * zoom;
			angle = angle + 0.1;
			break;
		case VK_DOWN:
			isRotating == false;
			lookAtAxis = 1;
			cameraPosition.x = 0.0f;
			cameraPosition.y = cos(angle) * zoom;
			cameraPosition.z = sin(angle) * zoom;
			angle = angle - 0.1;
			break;
		case VK_LEFT:
			isRotating == false;
			lookAtAxis = 2;
			cameraPosition.x = cos(angle) * zoom;
			cameraPosition.z = sin(angle)*zoom;
			angle = angle - 0.1;
			break;
		case VK_RIGHT:
			isRotating == false;
			lookAtAxis = 2;
			cameraPosition.x = cos(angle) * zoom;
			cameraPosition.z = sin(angle)*zoom;
			angle = angle + 0.1;
			break;
		case 0x46://F
			toggleFullScreen();
			break;
		case 0x41:
			cameraPosition.z = (cameraPosition.z + 0.1);

			break;
		case 0x42:
			cameraPosition.z = (cameraPosition.z - 0.1);
			break;
		case 0x58://X
			lookAtAxis = 1;
			break;
		case 0x59://Y
			lookAtAxis = 2;
			break;
		case 0x52://R
			isRotating = !isRotating;
			if (isRotating == false) {
				prevAngle = angle;
			}
			break;
		default:
			break;
		}
		break;
	case WM_ERASEBKGND:
		return(0);
		break;

	default:
		break;
	}
	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void initialize() {
	void resize(int, int);

	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	//Intialise member of pixelformatdescriptor
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cBlueBits = 8;
	pfd.cGreenBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	resize(WIN_WIDTH, WIN_HEIGHT);
}

void display(void) {
	void drawCube(void);
	void drawRoom();
	void drawGrid();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, 1.0f);
	if (isRotating == true) {
		if (lookAtAxis == 1) { //Around X Axis
			gluLookAt(0.0f, cos(angle)*zoom, sin(angle)*zoom, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		}
		else if (lookAtAxis == 2) { // Around Y Axis
			gluLookAt(cos(angle)*zoom, 1.0f, sin(angle)*zoom, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		}
		else if (lookAtAxis == 3) { //Around Z Axis
			gluLookAt(cos(angle)*zoom, sin(angle)*zoom, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
		}
	}
	else {
		gluLookAt(cameraPosition.x,cameraPosition.y,cameraPosition.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	}
	drawGrid();
	//Actual object which needs look at
	drawCube();
	SwapBuffers(ghdc);
}

void spin(void) {
	if (isRotating == true) {
		angle = angle + 0.0005f;
		if (angle >= 360.0f)
			angle = 0.0f;
	}
}

void drawRoom() {
	glBegin(GL_QUADS);

	glColor3f(0.4f, 0.0f, 0.2f); //Left face
	glVertex3f(roomSize.xCor, roomSize.yCor, -roomSize.zCor);
	glVertex3f(roomSize.xCor, roomSize.yCor, roomSize.zCor);
	glVertex3f(roomSize.xCor, -roomSize.yCor, roomSize.zCor);
	glVertex3f(roomSize.xCor, -roomSize.yCor, -roomSize.zCor);

	glColor3f(0.0f, 0.3f, 0.7f); //back face
	glVertex3f(roomSize.xCor, roomSize.yCor, -roomSize.zCor);
	glVertex3f(-roomSize.xCor, roomSize.yCor, -roomSize.zCor);
	glVertex3f(-roomSize.xCor, -roomSize.yCor, -roomSize.zCor);
	glVertex3f(roomSize.xCor, -roomSize.yCor, -roomSize.zCor);

	glColor3f(2.0f, 0.0f, 0.3f); //Right face
	glVertex3f(-roomSize.xCor, roomSize.yCor, -roomSize.zCor);
	glVertex3f(-roomSize.xCor, roomSize.yCor, roomSize.zCor);
	glVertex3f(-roomSize.xCor, -roomSize.yCor, roomSize.zCor);
	glVertex3f(-roomSize.xCor, -roomSize.yCor, -roomSize.zCor);

	glColor3f(0.3f, 0.7f, 0.6f); //Top face
	glVertex3f(roomSize.xCor, roomSize.yCor, -roomSize.zCor);
	glVertex3f(-roomSize.xCor, roomSize.yCor, -roomSize.zCor);
	glVertex3f(-roomSize.xCor, roomSize.yCor, roomSize.zCor);
	glVertex3f(roomSize.xCor, roomSize.yCor, roomSize.zCor);

	glColor3f(89.0f/255.0f,80.0f/255.0f,67.0f/255.0f); //Bottom face
	glVertex3f(roomSize.xCor, -roomSize.yCor, -roomSize.zCor);
	glVertex3f(-roomSize.xCor, -roomSize.yCor, -roomSize.zCor);
	glVertex3f(-roomSize.xCor, -roomSize.yCor, roomSize.zCor);
	glVertex3f(roomSize.xCor, -roomSize.yCor, roomSize.zCor);

	glEnd();
}

void drawCube(void) {
	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f); //Front face
	glVertex3f(cubeSize.xCor, cubeSize.yCor, cubeSize.zCor);
	glVertex3f(-cubeSize.xCor, cubeSize.yCor, cubeSize.zCor);
	glVertex3f(-cubeSize.xCor, -cubeSize.yCor, cubeSize.zCor);
	glVertex3f(cubeSize.xCor, -cubeSize.yCor, cubeSize.zCor);

	glColor3f(0.0f, 1.0f, 0.0f); //Left face
	glVertex3f(cubeSize.xCor, cubeSize.yCor, -cubeSize.zCor);
	glVertex3f(cubeSize.xCor, cubeSize.yCor, cubeSize.zCor);
	glVertex3f(cubeSize.xCor, -cubeSize.yCor, cubeSize.zCor);
	glVertex3f(cubeSize.xCor, -cubeSize.yCor, -cubeSize.zCor);

	glColor3f(0.0f, 0.0f, 1.0f); //back face
	glVertex3f(cubeSize.xCor, cubeSize.yCor, -cubeSize.zCor);
	glVertex3f(-cubeSize.xCor, cubeSize.yCor, -cubeSize.zCor);
	glVertex3f(-cubeSize.xCor, -cubeSize.yCor, -cubeSize.zCor);
	glVertex3f(cubeSize.xCor, -cubeSize.yCor, -cubeSize.zCor);

	glColor3f(1.0f, 1.0f, 0.0f); //Right face
	glVertex3f(-cubeSize.xCor, cubeSize.yCor, -cubeSize.zCor);
	glVertex3f(-cubeSize.xCor, cubeSize.yCor, cubeSize.zCor);
	glVertex3f(-cubeSize.xCor, -cubeSize.yCor, cubeSize.zCor);
	glVertex3f(-cubeSize.xCor, -cubeSize.yCor, -cubeSize.zCor);

	glColor3f(0.0f, 1.0f, 1.0f); //Top face
	glVertex3f(cubeSize.xCor, cubeSize.yCor, -cubeSize.zCor);
	glVertex3f(-cubeSize.xCor, cubeSize.yCor, -cubeSize.zCor);
	glVertex3f(-cubeSize.xCor, cubeSize.yCor, cubeSize.zCor);
	glVertex3f(cubeSize.xCor, cubeSize.yCor, cubeSize.zCor);

	glColor3f(1.0f, 0.0f, 1.0f); //Bottom face
	glVertex3f(cubeSize.xCor, -cubeSize.yCor, -cubeSize.zCor);
	glVertex3f(-cubeSize.xCor, -cubeSize.yCor, -cubeSize.zCor);
	glVertex3f(-cubeSize.xCor, -cubeSize.yCor, cubeSize.zCor);
	glVertex3f(cubeSize.xCor, -cubeSize.yCor, cubeSize.zCor);

	glEnd();
}

void drawGrid() {
	int size = 6;
	int count = 1;
	glLineWidth(0.5f);
	glBegin(GL_LINES);
	glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
	for (float i = count; i <= size; i += count) {
		glVertex3f(-size, 0, i); 
		glVertex3f(size, 0, i);
		glVertex3f(-size, 0, -i);  
		glVertex3f(size, 0, -i);
		glVertex3f(i, 0, -size); 
		glVertex3f(i, 0, size);
		glVertex3f(-i, 0, -size);  
		glVertex3f(-i, 0, size);
	}

	glEnd();
	glBegin(GL_QUADS);
	glColor3f(0.4f, 0.4f, 0.4f);
	glVertex3f(roomSize.xCor, roomSize.yCor, -roomSize.zCor);
	glVertex3f(-roomSize.xCor, roomSize.yCor, -roomSize.zCor);
	glVertex3f(-roomSize.xCor, 0.0f, -roomSize.zCor);
	glVertex3f(roomSize.xCor, 0.0f, -roomSize.zCor);

	glColor3f(0.2f, 0.2f, 0.2f);
	glVertex3f(-roomSize.xCor, 0.0f, -roomSize.zCor);
	glVertex3f(-roomSize.xCor, roomSize.yCor, -roomSize.zCor);
	glVertex3f(-roomSize.xCor, roomSize.yCor, roomSize.zCor);
	glVertex3f(-roomSize.xCor, 0.0f, roomSize.zCor);

	glColor3f(0.2f, 0.2f, 0.2f);
	glVertex3f(roomSize.xCor, 0.0f, -roomSize.zCor);
	glVertex3f(roomSize.xCor, roomSize.yCor, -roomSize.zCor);
	glVertex3f(roomSize.xCor, roomSize.yCor, roomSize.zCor);
	glVertex3f(roomSize.xCor, 0.0f, roomSize.zCor);

	glEnd();
}

void resize(int width, int height) {
	//code
	if (height == 0)
		height = 1;
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void toggleFullScreen(void) {
	BOOL isWindowPlacement;
	BOOL isMonitorInfo;

	if (gbFullScreen == false) {
		gbFullScreen = true;
		style = GetWindowLong(ghwnd, GWL_STYLE);
		//Does it include overlappedwindow
		if (style & WS_OVERLAPPEDWINDOW)
		{
			//To get hmonitor call monitor from window
			MONITORINFO monitorInfo;
			monitorInfo.cbSize = sizeof(MONITORINFO);
			HMONITOR hmonitor = MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY);
			isMonitorInfo = GetMonitorInfo(hmonitor, &monitorInfo);
			isWindowPlacement = GetWindowPlacement(ghwnd, &wpPrev);
			if (isWindowPlacement && isMonitorInfo)
			{
				SetWindowLong(ghwnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,
					HWND_TOP,
					monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.top,
					monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
					monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(false);
	}
	else {
		gbFullScreen = false;
		SetWindowLong(ghwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0,//It is taken care by setwindowplacement
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(true);
	}
}

void uninitialize(void)
{
	//UNINITIALIZATION CODE
	if (gbFullScreen == true)
	{
		style = GetWindowLong(ghwnd, GWL_STYLE);
		SetWindowLong(ghwnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}

	wglMakeCurrent(NULL, NULL);

	wglDeleteContext(ghrc);
	ghrc = NULL;

	ReleaseDC(ghwnd, ghdc);
	ghdc = NULL;

	DestroyWindow(ghwnd);
	ghwnd = NULL;

}
