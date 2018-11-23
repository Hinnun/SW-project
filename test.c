#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <mmsystem.h>

//------------------------------------midi
#pragma comment(lib, "winmm.lib")
#pragma pack(push,1)

typedef struct
{
	BYTE byteState;
	BYTE byteNote;
	BYTE byteVelocity;
	BYTE byteNULL;
}MJGRShortMidiMSG_ST;

typedef union
{
	DWORD                dwMidi_Data;
	MJGRShortMidiMSG_ST  stMidi_Data;

}MJGRShortMidiMSG;

#pragma pack(pop)

HMIDIOUT     m_DevHandle;
MMRESULT     m_MMErrCode;
MIDIOUTCAPS  m_NowMidiOutCaps;

void CALLBACK MidiOutProc(HMIDIOUT MidiOut, UINT Msg, DWORD Instance, DWORD Param1, DWORD Param2)
{
}



void Midi_OutputError(MMRESULT mmrErrCode)
{
	char m_szErrMsg[128];
	midiInGetErrorText(mmrErrCode, (char *)m_szErrMsg, sizeof(m_szErrMsg));
	MessageBox(0, m_szErrMsg, "Midi Error!!!", MB_OK);

}


long Midi_GetDevID(HMIDIOUT m_DevOutHandle)
{

	UINT     uDeviceID;
	MMRESULT Result = midiOutGetID(m_DevOutHandle, &uDeviceID);

	if (Result != MMSYSERR_NOERROR)
	{
		Midi_OutputError(Result);
		return -1;
	}

	return (long)uDeviceID;

}

void Midi_GetDevCaps(HMIDIOUT m_DevOutHandle, MIDIOUTCAPS *Caps)

{

	long     lDeviceID;
	MMRESULT Result;
	lDeviceID = Midi_GetDevID(m_DevOutHandle);

	if (lDeviceID < 0) return;

	Result = midiOutGetDevCaps((UINT)lDeviceID, Caps, sizeof(MIDIOUTCAPS));

	if (Result != MMSYSERR_NOERROR)
	{

		Midi_OutputError(Result);

	}

}

HMIDIOUT Midi_Open(WORD wDevsNum)
{

	WORD      wMaxDevNum = 0;
	MMRESULT  uiMidiOpen = 0;
	HMIDIOUT  m_DevHandle = NULL;
	wMaxDevNum = midiInGetNumDevs();



	if (wDevsNum >= wMaxDevNum)

	{
		wDevsNum = 0;
	}



	uiMidiOpen = midiOutOpen(&m_DevHandle, wDevsNum,
		(DWORD)(MidiOutProc),
		(DWORD)NULL, CALLBACK_FUNCTION);


	if (uiMidiOpen != MMSYSERR_NOERROR)
	{
		Midi_OutputError(uiMidiOpen);
		return NULL;
	}

	return m_DevHandle;

}

LRESULT Midi_Close(HMIDIOUT m_DevOutHandle)

{

	MMRESULT Result;
	Result = midiOutClose(m_DevOutHandle);

	if (Result != MMSYSERR_NOERROR)
	{

		Midi_OutputError(Result);
		return FALSE;

	}
	else
	{
		m_DevOutHandle = NULL;
	}

	return TRUE;

}

void Midi_SendShortMsg(HMIDIOUT m_DevOutHandle, BYTE byteState, BYTE byteNote, BYTE byteValo)
{
	MJGRShortMidiMSG  sMsg;
	sMsg.stMidi_Data.byteVelocity = byteValo;
	sMsg.stMidi_Data.byteNote = byteNote;
	sMsg.stMidi_Data.byteState = byteState;
	sMsg.stMidi_Data.byteNULL = 0;
	midiOutShortMsg(m_DevOutHandle, sMsg.dwMidi_Data);
}

void Midi_SendLongMsg(HMIDIOUT m_DevOutHandle, BYTE *lpMsg, DWORD dwMsgLength)
{
	MIDIHDR  m_MIDIHdr;
	MMRESULT Result;
	ZeroMemory(&m_MIDIHdr, sizeof(MIDIHDR));

	m_MIDIHdr.lpData = (char *)lpMsg;
	m_MIDIHdr.dwBufferLength = dwMsgLength;
	m_MIDIHdr.dwFlags = 0;

	Result = midiOutPrepareHeader(m_DevOutHandle, &m_MIDIHdr, sizeof(MIDIHDR));

	if (Result != MMSYSERR_NOERROR) {
		Midi_OutputError(Result);
		return;
	}

	Result = midiOutLongMsg(m_DevOutHandle, &m_MIDIHdr, sizeof(MIDIHDR));

	if (Result != MMSYSERR_NOERROR) {
		Midi_OutputError(Result);
		return;
	}

	midiOutUnprepareHeader(m_DevOutHandle, &m_MIDIHdr, sizeof(MIDIHDR));
}


void Midi_AllChannelSoundOff(HMIDIOUT m_DevOutHandle)
{
	BYTE channel;

	for (channel = 0; channel < 16; channel++)
		Midi_SendShortMsg(m_DevOutHandle, (BYTE)(0xB0 + channel), 0x78, 0);

}

//---------------------------------------------------------------------------------

#define MAX_X 40
#define MAX_Y 20
#define MIN_X 8
#define MIN_Y 4

#define INTERFACE_X 50
#define INTERFACE_Y 2

#define MAX_BLOCK 16
#define ACTIVE_BLOCK 1
#define INACTIVE_BLOCK 0

typedef enum { NOCURSOR, SOLIDCURSOR, NORMALCURSOR } CURSOR_TYPE;
void setcursortype(CURSOR_TYPE c) { //커서숨기는 함수 

	CONSOLE_CURSOR_INFO CurInfo;

	switch (c) {
	case NOCURSOR:
		CurInfo.dwSize = 1;
		CurInfo.bVisible = FALSE;
		break;
	case SOLIDCURSOR:
		CurInfo.dwSize = 100;
		CurInfo.bVisible = TRUE;
		break;
	case NORMALCURSOR:
		CurInfo.dwSize = 20;
		CurInfo.bVisible = TRUE;
		break;
	}
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

int arr[MAX_BLOCK][MAX_BLOCK] = { 0, };
int column = 0;
int row = 0;

int note[] = { 27, 25, 24, 22, 20, 18, 17, 15, 13, 12, 10, 8, 6, 5, 3, 1 };
int highsound = -1;
int speed = 100;

int x = MIN_X;
int y = MIN_Y;
int mouse_x = MIN_X;


void gotoxy(int x, int y) {
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void drawMap()
{
	int ly = 1;
	gotoxy(INTERFACE_X, INTERFACE_Y); printf("		   >>HOW TO PLAY<<\n");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l   -   MOVE   :   ◁ ▷, △ ▽, q");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l   -   ACTIVE  or  INACTIVE : SPACE BAR");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l   -   RANDOM :   R");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l   -   CLEAR  :   C");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l   *   SPEED UP / DOWN  : A / D");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l   *   HIGHT SOUND UP / DOWN  :  W / S");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l___________________________________________");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l   HIGHT SOUND = %4d", highsound);
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l   SPEED = %4d", speed);
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l   x = %4d      mouse_x = %4d", x, mouse_x);
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l	          mouse_y = %4d", y);
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l			    EXIT   :   E");
	gotoxy(INTERFACE_X, INTERFACE_Y + ly++); printf("	l___________________________________________");
}

void drawBorad()
{
	for (int i = 0; i < MAX_BLOCK; i++) {
		for (int j = 0; j < MAX_BLOCK; j++) {
			gotoxy((2 * i) + MIN_X, j + MIN_Y);
			if ((2 * i) + MIN_X == mouse_x && j + MIN_Y == y) {
				if (arr[i][j] == ACTIVE_BLOCK)
					printf("★");
				else
					printf("☆");
			}
			else if (arr[i][j] == ACTIVE_BLOCK)
				printf("●");
			else
				printf("○");
		}
	}
}

void makeSound() //
{
	int mu[MAX_BLOCK] = { 0, };

	for (column = 0; column < MAX_BLOCK; column++)
	{
		if (row == column)
			mu[column] = 1;
		else
			mu[column] = 0;
		gotoxy(MIN_X + column * 2, MIN_Y - 1);
		printf("%d ", mu[column]);

		if (arr[row][column] == ACTIVE_BLOCK) {
			Midi_SendShortMsg(m_DevHandle, 0x90, (BYTE)(0x30 + note[column] + highsound), 50);
		}
	}
	if (++row == MAX_BLOCK)	row = 0;
}

void clear()
{
	for (int i = 0; i < MAX_BLOCK; i++)
		for (int j = 0; j < MAX_BLOCK; j++)
			arr[i][j] = INACTIVE_BLOCK;
}

void random()
{
	srand(time(NULL));
	int a[MAX_BLOCK] = { 0, };
	int n = rand()%((MAX_BLOCK * MAX_BLOCK - 126) - 69) + 70;



	for (int i = 0; i < MAX_BLOCK; i++) {
		for (int j = 0; j < MAX_BLOCK; j++) {
			if (n-- > 0) {
				if (arr[i][j] = rand() % 2) {
					if (arr[i][j] = rand() % 2) {
						a[i]++;
						if (j < MAX_BLOCK)
							j++;
					}
				}
			}
			else
				break;
			if (a[i] > rand() % 6)
				break;
		}
		if (n == 0)
			break;
	}
}

checkKey()
{
	int chr;


	if (_kbhit()) {
		chr = _getch();
		if (chr == 0 || chr == 0xe0) {
			do { chr = _getch(); } while (chr == 224);
			if (chr == 72) {
				y--;
				if (y < MIN_Y) {
					y = MIN_Y;
				}
			}
			else if (chr == 80) {
				y++;
				if (y > MAX_Y - 1) {
					y = MAX_Y - 1;
				}
			}
			else if (chr == 75) {
				x--;
				mouse_x -= 2;
				if (mouse_x < MIN_X) {
					mouse_x = MIN_X;
					x = MIN_X;
				}
			}
			else if (chr == 77) {
				x++;
				mouse_x += 2;
				if (mouse_x > MAX_X - 2) {
					mouse_x = MAX_X - 2;
					x = MAX_X - MAX_BLOCK - 1;
				}
			}
		}
		else if (chr == 32)							// space bar
		{
			if (arr[x - MIN_X][y - MIN_Y] == ACTIVE_BLOCK)
				arr[x - MIN_X][y - MIN_Y] = INACTIVE_BLOCK;
			else if (arr[x - MIN_X][y - MIN_Y] == INACTIVE_BLOCK)
				arr[x - MIN_X][y - MIN_Y] = ACTIVE_BLOCK;
		}
		else if (chr == 113)						// q
		{
			x = 16;
			mouse_x = 24;
			y = 12;
		}
		else if (chr == 114)						// r
		{
			clear();
			random();
		}
		else if (chr == 97 || chr == 100) {			// a d
			speed = (chr == 100) ? speed + 5 : speed - 5;
			if (speed > 200)
				speed = 200;
			else if (speed < 40)
				speed = 40;
		}
		else if (chr == 115 || chr == 119) {		// s w
			(chr == 119) ? highsound ++ : highsound --;
			if (highsound > 8)
				highsound = 8;
			else if (highsound < -4)
				highsound = -4;
		}
		else if(chr == 99)							// c
			clear();
		else if (chr == 101)						// e
			exit(1);
	}

	while (_kbhit()) _getch();
}


int main() {

	m_DevHandle = Midi_Open(0);

	if (m_DevHandle == NULL)
		return;

	Midi_AllChannelSoundOff(m_DevHandle);
	Midi_SendShortMsg(m_DevHandle, 0xB0, 7, 127);

	setcursortype(NOCURSOR);


	while (1) {

		makeSound();
		checkKey();

		drawBorad();
		drawMap();

		Sleep(speed);
	}

	Midi_AllChannelSoundOff(m_DevHandle);
	Midi_Close(m_DevHandle);

}
