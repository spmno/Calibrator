#pragma once
#include <windows.h>
#include "InvSqrt.h"

#define Z0 150

typedef struct CalibratePoint_
{
	unsigned short x;
	unsigned short y;
}CalibratePoint;

template<int PIXEL>
class NormalCalibrator
{
public:
	NormalCalibrator(void)
	{
		convertBuffer_ = new DWORD[PIXEL*PIXEL];
		findTable_ = new POINT[PIXEL*PIXEL];
		setZValue(Z0);
	}
	~NormalCalibrator(void)
	{
		delete[] convertBuffer_;
		delete[] findTable_;
	}

	void* calibrate(BYTE* originImage)
	{
		for(int j=0; j<PIXEL; ++j)
			for( int i=0; i<PIXEL;++i)
			{
				//convertBuffer[j*480+i][0] = (BYTE)(*((originImage+(findTable_[j][i].y*480*4+findTable_[j][i].x*4))));
				//convertBuffer[j*480+i][1] = (BYTE)(*((originImage+(findTable_[j][i].y*480*4+findTable_[j][i].x*4+1))));
				//convertBuffer[j*480+i][2] = (BYTE)(*((originImage+(findTable_[j][i].y*480*4+findTable_[j][i].x*4+2))));
				//convertBuffer[j*480+i][3] = (BYTE)(*((originImage+(findTable_[j][i].y*480*4+findTable_[j][i].x*4+3))));
				//memcpy(convertBuffer[j*480+i], (originImage+(findTable_[j][i].y*480*4+findTable_[j][i].x*4)), 4);
				//RETAILMSG(1, (TEXT("0=%x, 1=%x, 2=%x, 3=%x, all=%x\r\n"),convertBuffer[j*480+i][0],convertBuffer[j*480+i][1],convertBuffer[j*480+i][2],convertBuffer[j*480+i][3],(DWORD)(*((originImage+(findTable_[j][i].y*480*4+findTable_[j][i].x*4)))) ));
				convertBuffer_[j*PIXEL+i] = *((DWORD*)((originImage+(findTable_[j*PIXEL+i].y*PIXEL*4+findTable_[j*PIXEL+i].x*4))));

			}
		return convertBuffer_;
	}

	void setZValue(DWORD zValue)
	{
		float r = PIXEL/2;

		POINT UV0 = {0, 0};
		POINT XY0;
		InvSqrt invSqrt;
		XY0.x = 1.0*zValue*UV0.x*invSqrt(r*r-UV0.x*UV0.x-UV0.y*UV0.y);
		XY0.y = 1.0*zValue*UV0.y*invSqrt(r*r-UV0.x*UV0.x-UV0.y*UV0.y);

		for(int j=0; j<PIXEL; j++)
			for( int i=0; i<PIXEL;i++)
			{
				POINT xy;
				POINT uv;
				xy.x = i - 1.0*PIXEL/2 + XY0.x;
				xy.y = 1.0*PIXEL/2 - j - XY0.y;
				//uv = XY_to_UV(xy,r);

				uv.x = r*xy.x*invSqrt(xy.x*xy.x+xy.y*xy.y+zValue*zValue);
				uv.y = r*xy.y*invSqrt(xy.x*xy.x+xy.y*xy.y+zValue*zValue);

				uv.x=uv.x+PIXEL/2;
				uv.y=PIXEL/2-uv.y;

				findTable_[j*PIXEL+i].x = uv.x;
				findTable_[j*PIXEL+i].y = uv.y;
			}
	}
private:
	DWORD* convertBuffer_;
	POINT* findTable_;
};

template <int WIDTH, int HEIGHT>
class RectCalibrator
{
	public:
	RectCalibrator(void)
	{
		convertBuffer_ = new DWORD[WIDTH*HEIGHT];
		findTable_ = new CalibratePoint[WIDTH*HEIGHT];
		setZValue(Z0);
	}
	~RectCalibrator(void)
	{
		delete[] convertBuffer_;
		delete[] findTable_;
	}

	void* calibrate(BYTE* originImage)
	{
		for(int j=0; j<HEIGHT; ++j)
			for( int i=0; i<WIDTH;++i)
			{
				//convertBuffer[j*480+i][0] = (BYTE)(*((originImage+(findTable_[j][i].y*480*4+findTable_[j][i].x*4))));
				//convertBuffer[j*480+i][1] = (BYTE)(*((originImage+(findTable_[j][i].y*480*4+findTable_[j][i].x*4+1))));
				//convertBuffer[j*480+i][2] = (BYTE)(*((originImage+(findTable_[j][i].y*480*4+findTable_[j][i].x*4+2))));
				//convertBuffer[j*480+i][3] = (BYTE)(*((originImage+(findTable_[j][i].y*480*4+findTable_[j][i].x*4+3))));
				//memcpy(convertBuffer[j*480+i], (originImage+(findTable_[j][i].y*480*4+findTable_[j][i].x*4)), 4);
				//RETAILMSG(1, (TEXT("0=%x, 1=%x, 2=%x, 3=%x, all=%x\r\n"),convertBuffer[j*480+i][0],convertBuffer[j*480+i][1],convertBuffer[j*480+i][2],convertBuffer[j*480+i][3],(DWORD)(*((originImage+(findTable_[j][i].y*480*4+findTable_[j][i].x*4)))) ));
				convertBuffer_[j*WIDTH+i] = *((DWORD*)((originImage+(findTable_[j*WIDTH+i].y*WIDTH*4+findTable_[j*WIDTH+i].x*4))));

			}
		return convertBuffer_;
	}

	void calibratePhysical(u32 originalImage, u32 convertImage)
	{
		DWORD* convertTemp = (DWORD*)MEM_PHY_TO_VIR(convertImage);
		DWORD* originalTemp = (DWORD*)MEM_PHY_TO_VIR(originalImage);
		
		for(int j=0; j<HEIGHT; ++j)
			for( int i=0; i<WIDTH;++i)
			{
				convertTemp[j*WIDTH+i] = *(originalTemp+(findTable_[j*WIDTH+i].y*WIDTH+findTable_[j*WIDTH+i].x));
			}
	}

	void calibratePhysicalArray(u32 originalImageArray[], u32 convertImageArray[])
	{
		DWORD* convertTemp0 = (DWORD*)MEM_PHY_TO_VIR(convertImageArray[0]);
		DWORD* originalTemp0 = (DWORD*)MEM_PHY_TO_VIR(originalImageArray[0]);
		DWORD* convertTemp1 = (DWORD*)MEM_PHY_TO_VIR(convertImageArray[1]);
		DWORD* originalTemp1 = (DWORD*)MEM_PHY_TO_VIR(originalImageArray[1]);
		DWORD* convertTemp2 = (DWORD*)MEM_PHY_TO_VIR(convertImageArray[2]);
		DWORD* originalTemp2 = (DWORD*)MEM_PHY_TO_VIR(originalImageArray[2]);
		DWORD* convertTemp3 = (DWORD*)MEM_PHY_TO_VIR(convertImageArray[3]);
		DWORD* originalTemp3 = (DWORD*)MEM_PHY_TO_VIR(originalImageArray[3]);

		register originalPosition = 0;
		register convertPosition = 0;
#if 1
		for(int j=0; j<HEIGHT; ++j)
			for( int i=0; i<WIDTH;++i)
			{
				convertPosition = findTable_[originalPosition].y*WIDTH+findTable_[originalPosition].x;
				*(convertTemp0+originalPosition) = *(originalTemp0+convertPosition);
				*(convertTemp1+originalPosition) = *(originalTemp1+convertPosition);
				*(convertTemp2+originalPosition) = *(originalTemp2+convertPosition);
				*(convertTemp3+originalPosition) = *(originalTemp3+convertPosition);
				++originalPosition;
			}
#else
		for (int i = 0; i < HEIGHT*WIDTH; ++i) {
			originalPosition = i;
			convertPosition = findTable_[i].y*WIDTH+findTable_[i].x;
			*(convertTemp0+originalPosition) = *(originalTemp0+convertPosition);
			*(convertTemp1+originalPosition) = *(originalTemp1+convertPosition);
			*(convertTemp2+originalPosition) = *(originalTemp2+convertPosition);
			*(convertTemp3+originalPosition) = *(originalTemp3+convertPosition);
		}
#endif
	}
	void setZValue(DWORD zValue)
	{
		int PIXEL = WIDTH;
		float r = PIXEL/2;
		POINT* tempTable = new POINT[PIXEL*PIXEL];
		POINT UV0 = {0, 0};
		POINT XY0;
		InvSqrt invSqrt;
		XY0.x = 1.0*zValue*UV0.x*invSqrt(r*r-UV0.x*UV0.x-UV0.y*UV0.y);
		XY0.y = 1.0*zValue*UV0.y*invSqrt(r*r-UV0.x*UV0.x-UV0.y*UV0.y);
		int beginHeight = (WIDTH - HEIGHT) / 2;
		int endHeight = (WIDTH - beginHeight);
		for(int j=0; j<PIXEL; j++)
			for( int i=0; i<PIXEL;i++)
			{
				POINT xy;
				POINT uv;
				xy.x = i - 1.0*PIXEL/2 + XY0.x;
				xy.y = 1.0*PIXEL/2 - j - XY0.y;
				//uv = XY_to_UV(xy,r);

				uv.x = r*xy.x*invSqrt(xy.x*xy.x+xy.y*xy.y+zValue*zValue);
				uv.y = r*xy.y*invSqrt(xy.x*xy.x+xy.y*xy.y+zValue*zValue);

				uv.x=uv.x+PIXEL/2;
				uv.y=PIXEL/2-uv.y;

				tempTable[j*PIXEL+i].x = uv.x;
				tempTable[j*PIXEL+i].y = uv.y;

				if (j >= beginHeight && j < endHeight) {
					findTable_[(j-beginHeight)*PIXEL+i].x = uv.x;
					findTable_[(j-beginHeight)*PIXEL+i].y = (uv.y-beginHeight) > 0 ? (uv.y-beginHeight) : 0;
					if (findTable_[(j-beginHeight)*PIXEL+i].y >= (HEIGHT-1)) {
						findTable_[(j-beginHeight)*PIXEL+i].y = (HEIGHT-1);
					}
				}
			}

	}
private:
	DWORD* convertBuffer_;
	CalibratePoint* findTable_;
};