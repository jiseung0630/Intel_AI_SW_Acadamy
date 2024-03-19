#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <Windows.h> //GUI �������
#include <math.h>
#include <conio.h>
#include < stdlib.h>

/////////////////////
// ���� ������
/////////////////////
HWND hwnd;//������ ȭ��(�ܺ�,������)
HDC hdc; // ������ ȭ�� (����,����)

FILE* rfp, * wfp;
char fileName[200];
int inH, inW, outH, outW;
int maskheight, maskwidth;
// �޸� Ȯ��

unsigned char** inImage = NULL, ** outImage = NULL, ** mask1 = NULL;
int** temp = NULL;
/////////////////////
// �Լ� �����
/////////////////////
void loadImage(); int getIntValue(); void printImage(); void saveImage();
void mallocInputMemory(); void mallocOutputMemory(); void freeInputMemory();
void freeOutputMemory(); void freeMaskMemory(); void mallocTempMemory(); void freeTempMemory();
int compare(const void*, const void*); void freeDoubleMemory(double**, int );
double** mallocDoubleMemory(int , int );

void equalImage(); void addImage(); void printMenu(); void divImage();
void opImage(); void blackImage128(); void blackImageAvg();
void andImage(); void xorImage(); void orImage(); void zoomOut();
void zoomIn(); void zoomIn2(); void rotate(); void rotate2();
void moveImage(); void mirrorRL(); void mirrorUD(); void zoomOutAvg(); void histoStretch();
void endIn(); void histoEqual(); void emboss(); void blur(); void sharpning(); void sharpning2();
void zoomOutMid(); void rotate3(); void zoomIn3(); void edgever(); void edgehor(); void blur99();

void main() {

	hwnd = GetForegroundWindow();
	hdc = GetWindowDC(NULL);
	char inKey = 0;
	while (inKey != '9') {
		printMenu();
		inKey = _getch();
		system("cls");

		switch (inKey)
		{
		case '0':loadImage(); break;
		case'1': saveImage(); break;
		case '9':break;
		case 'a':case 'A': equalImage(); break;
		case 'b':case 'B':addImage(); break;
		case 'c':case 'C': divImage(); break;
		case 'd':case 'D':opImage(); break;
		case 'e':case 'E':blackImage128(); break;
		case'f':case'F':blackImageAvg(); break;
		case'g':case'G':andImage(); break;
		case'h':case'H':xorImage(); break;
		case'5':orImage(); break;
		case 'i':case'I': zoomOut(); break;
		case 'j':case'J':zoomIn(); break;
		case'k':case'K':zoomIn2(); break;
		case'l':case'L':rotate(); break;
		case'm':case'M':rotate2(); break;
		case'n':case'N':moveImage(); break;
		case'o':case'O':mirrorRL(); break;
		case'p':case'P':mirrorUD(); break;
		case'q':case'Q':zoomOutAvg(); break;
		case'r':case'R':histoStretch(); break;
		case's':case'S':endIn(); break;
		case't':case'T':histoEqual(); break;
		case'u':case'U':zoomOutMid(); break;
		case'v':case'V':emboss(); break;
		case'w':case'W':blur(); break;
		case'x':case'X':sharpning(); break;
		case'y':case'Y':rotate3(); break;
		case'z':case'Z': zoomIn3(); break;
		case'2':edgever(); break;
		case'3':edgehor(); break;
		case'4':blur99(); break;
		case'6':sharpning2(); break;



		}
	}

	freeInputMemory();
	freeOutputMemory();
	freeMaskMemory();
}

/////////////////////
// �Լ� ���Ǻ�
/////////////////////
/// ���� �Լ�
//////////////
void printMenu() {
	puts("\n ## GrayScale Image Processing (Beta4) ##\n");
	puts("0. ���� 1. ���� 9. ����");
	puts("\n A.���� B. ��� C. ��Ӱ� D. ���� E. ���128 F. ���(��հ�) ");
	puts("\n G.AND H.XOR 5.OR I.��� J.zoomIn(������) K. zoomIn(�����) L. rotate M.rotate(�߾�,�����");
	puts("\n N.�̵� O.�¿�̷��� P.���� �̷��� Q. ���(��հ�) R. ������׷� ��Ʈ��ġ");
	puts("\n S. ������ T.��Ȱȭ U.���(�߾Ӱ�) V. ������ W. ����");
	puts("\n X. ������ Y. ȸ��(Ȯ��) Z. �缱�� ���� 2. ���� ���� ���� 3. ���� ���� ���� 4.99���� 6. ������2");
}
void printImage() {
	system("cls");
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			int px = outImage[i][k];
			SetPixel(hdc, k + 1000, i + 350, RGB(px, px, px));
		}
	}
}

void loadImage() {
	char fullName[200] = "D:/RAW/Etc_Raw(squre)/";
	char tmpName[50];
	printf("���ϸ�-->"); //flower512, LENA256
	scanf("%s", tmpName);
	strcat(fullName, tmpName);
	strcat(fullName, ".raw");
	strcpy(fileName, fullName);

	//�߿�! 
	rfp = fopen(fileName, "rb");
	if (rfp == NULL) {
		printf("������ ������ �ʾҽ��ϴ�.\n");
		return 0;
	}
	fseek(rfp, 0L, SEEK_END);//������ ������ �̵�
	long long fsize = ftell(rfp);//�� ���������? 262,155-->512
	fclose(rfp);
	freeInputMemory();
	//����, �� ���ϱ�
	inH = inW = (int)sqrt(fsize);

	//�޸� �Ҵ�
	mallocInputMemory();

	//����--> �޸�
	rfp = fopen(fileName, "rb");
	for (int i = 0; i < inH; i++)
		fread(inImage[i], sizeof(unsigned char), inW, rfp);
	fclose(rfp);

	rfp = fopen("D:/RAW/Etc_Raw(squre)/circle512.raw", "rb");
	fseek(rfp, 0L, SEEK_END);
	long long maskfsize = ftell(rfp);
	fclose(rfp);
	maskheight = maskwidth = (int)sqrt(maskfsize);

	//����ũ �޸� �Ҵ�
	mask1 = (unsigned char**)malloc(sizeof(unsigned char*) * maskheight);
	for (int i = 0; i < maskheight; i++)
		mask1[i] = (unsigned char*)malloc(sizeof(unsigned char) * maskwidth);

	rfp = fopen("D:/RAW/Etc_Raw(squre)/circle512.raw", "rb");
	for (int i = 0; i < maskheight; i++)
		fread(mask1[i], sizeof(unsigned char), maskwidth, rfp);
	fclose(rfp);

	equalImage();
}
int getIntValue() {
	int retValue;
	printf("������-->"); scanf("%d", &retValue);
	return retValue;
}
void saveImage() {
	if (outImage == NULL)
		return;
	char fullName[200] = "D:/RAW/Etc_Raw(squre)/";
	char tmpName[50];
	printf("���ϸ�-->"); // out01, out02......
	scanf("%s", tmpName);
	strcat(fullName, tmpName);
	strcat(fullName, ".raw");
	strcpy(fileName, fullName);

	wfp = fopen(fileName, "wb");
	// �޸� --> ���� (���྿)
	for (int i = 0; i < outH; i++)
		fwrite(outImage[i], sizeof(unsigned char), outW, wfp);
	fclose(wfp);
	// MessageBox(hwnd, L"���� �Ϸ�", L"���� â", NULL);
	printf("���� �Ϸ�");
}

void mallocInputMemory() {
	inImage = (unsigned char**)malloc(sizeof(unsigned char*) * inH);
	for (int i = 0; i < inH; i++)
		inImage[i] = (unsigned char*)malloc(sizeof(unsigned char) * inH);
}

void mallocOutputMemory() {
	outImage = (unsigned char**)malloc(sizeof(unsigned char*) * outH);
	for (int i = 0; i < outH; i++)
		outImage[i] = (unsigned char*)malloc(sizeof(unsigned char) * outW);
}

void mallocTempMemory() {
	temp = (int**)malloc((sizeof(int*) * outH));
	for (int i = 0; i < outH; i++)
		temp[i] = (int*)malloc(sizeof(int) * outW);
}

double** mallocDoubleMemory(int h, int w) {
	double** retMemory;
	retMemory = (double**)malloc(sizeof(double*) * h);
	for (int i = 0; i < h; i++)
		retMemory[i] = (double*)malloc(sizeof(double) * w);
	return retMemory;
}

void freeInputMemory() {
	if (inImage == NULL)
		return;
	for (int i = 0; i < inH; i++)
		free(inImage[i]);
	free(inImage);
	inImage = NULL;
}

void freeOutputMemory() {
	if (outImage == NULL)
		return;
	for (int i = 0; i < outH; i++)
		free(outImage[i]);
	free(outImage);
	outImage = NULL;
}
void freeTempMemory() {
	for (int i = 0; i < outH; i++)
		free(mask1[i]);
	free(mask1);

}

void freeMaskMemory() {
	for (int i = 0; i < maskheight; i++)
		free(mask1[i]);
	free(mask1);
}

void freeDoubleMemory(double** memory, int h) {
	if (memory == NULL)
		return;
	for (int i = 0; i < h; i++)
		free(memory[i]);
	free(memory);
	memory = NULL;
}
//////////////////
//  ����ó�� �Լ�
////////////////////
void equalImage() { // ���Ͽ��� �˰���
	// �Է� �迭 --> ��� �迭
	freeOutputMemory();
	outH = inH;
	outW = inW;
	mallocOutputMemory();
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = inImage[i][k];
		}
	}
	printImage();
}
void addImage() { // ���ϱ� �˰���
	// �Է� �迭 --> ��� �迭
	freeOutputMemory();
	inH = outH;
	inW = outW;
	mallocOutputMemory();
	int val = getIntValue();
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (inImage[i][k] + val < 255)
				outImage[i][k] = inImage[i][k] + val;
			else
				outImage[i][k] = 255;
		}
	}
	printImage();
}

void divImage() {
	freeOutputMemory();
	inH = outH;
	inW = outW;
	mallocOutputMemory();
	int val = getIntValue();
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (inImage[i][k] - val > 0) {
				outImage[i][k] = inImage[i][k] - val;
			}
			else outImage[i][k] = 0;
		}
	}
	printImage();

}

void opImage() {
	freeOutputMemory();
	inH = outH;
	inW = outW;
	mallocOutputMemory();
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			outImage[i][k] = 255 - inImage[i][k];
		}
	}
	printImage();
}

void blackImage128() {
	freeOutputMemory();
	inH = outH;
	inW = outW;
	mallocOutputMemory();
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (inImage[i][k] > 128) {
				outImage[i][k] = 255;
			}
			else
				outImage[i][k] = 0;

		}
	}
	printImage();
}

void blackImageAvg() {
	int avg = 0;
	int hap = 0;
	freeOutputMemory();
	inH = outH;
	inW = outW;
	mallocOutputMemory();
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			hap += inImage[i][k];
			avg = hap / (inH * inW);
		}
	}
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (inImage[i][k] > avg)
				outImage[i][k] = 255;
			else
				outImage[i][k] = 0;
		}
	}
	printImage();
}

void andImage() {
	freeOutputMemory();
	inH = outH;
	inW = outW;
	mallocOutputMemory();
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			outImage[i][k] = inImage[i][k] & mask1[i][k];
		}
	}
	printImage();

}

void xorImage() {
	freeOutputMemory();
	inH = outH;
	inW = outW;
	printf("����");
	int compare;
	compare = getIntValue();
	mallocOutputMemory();
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			outImage[i][k] = inImage[i][k] ^ compare;
		}
	}
	printImage();

}

void orImage() {
	freeOutputMemory();
	inH = outH;
	inW = outW;
	mallocOutputMemory();
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			outImage[i][k] = inImage[i][k] | mask1[i][k];
		}
	}
	printImage();
}

void zoomOut() { //��� �˰���
	int scale = getIntValue();
	//�޸� ����
	freeOutputMemory();
	outH = (int)(inH / scale);
	outW = (int)(inW / scale);
	//�޸� �Ҵ�
	mallocOutputMemory();
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[(int)(i / scale)][(int)(k / scale)] = inImage[i][k];
		}
	}
	printImage();
}

void zoomIn() { //Ȯ�� �˰���
	int scale = getIntValue();
	//�޸� ����
	freeOutputMemory();
	//(�߿�!) ��� �̹����� ũ�⸦ ���� -->�˰��� ����
	outH = (int)(inH * scale);
	outW = (int)(inW * scale);
	//�޸� �Ҵ�
	mallocOutputMemory();
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[(int)(i * scale)][(int)(k * scale)] = inImage[i][k];
		}
	}
	printImage();

}

void zoomIn2() { //Ȯ�� �˰���
	int scale = getIntValue();
	//�޸� ����
	freeOutputMemory();
	//(�߿�!) ��� �̹����� ũ�⸦ ���� -->�˰��� ����
	outH = (int)(inH * scale);
	outW = (int)(inW * scale);
	//�޸� �Ҵ�
	mallocOutputMemory();
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			outImage[(int)(i)][(int)(k)] = inImage[i / scale][k / scale];
		}
	}
	printImage();

}

void rotate() { // ȸ�� �˰���
	// �Է� �迭 --> ��� �迭
	freeOutputMemory();
	outH = inH;
	outW = inW;
	mallocOutputMemory();
	int degree = getIntValue();
	double radian = -degree * 3.141592 / 180.0;
	//xd = cos*(xs)-sin*(ys)
	//yd = sin*(sx) + cos*(ys)


	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			int xs = i;
			int ys = k;
			int xd = (int)(cos(radian) * xs + sin(radian) * ys);
			int yd = (int)(-sin(radian) * xs + cos(radian) * ys);

			if ((0 <= xd && xd < outH) && (0 <= yd && yd < outW))
				outImage[xd][yd] = inImage[xs][ys];
		}
	}
	printImage();
}

void rotate2() { // ȸ�� �˰���
	// �Է� �迭 --> ��� �迭
	freeOutputMemory();
	outH = inH;
	outW = inW;
	mallocOutputMemory();
	int degree = getIntValue();
	double radian = -degree * 3.141592 / 180.0;

	int cx = inH / 2;
	int cy = inW / 2;

	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			int xd = i;
			int yd = k;

			int xs = (int)(cos(radian) * (xd - cx) + sin(radian) * (yd - cy)) + cx;
			int ys = (int)(-sin(radian) * (xd - cx) + cos(radian) * (yd - cy)) + cy;
			
			if ((0 <= xs && xs < outH) && (0 <= ys && ys < outW))
				outImage[xd][yd] = inImage[xs][ys];
		}
	}
	printImage();
}

void moveImage() { // ���Ͽ��� �˰���
	// �Է� �迭 --> ��� �迭
	freeOutputMemory();
	int xmove, ymove;
	printf("x����");
	xmove = getIntValue();
	printf("y����");
	ymove = getIntValue();
	outH = inH;
	outW = inW;
	mallocOutputMemory();


	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {

			if ((0 <= i - xmove && i - xmove < outH) && (0 <= k - ymove && k - ymove < outW))
				outImage[i][k] = inImage[i - xmove][k - ymove];
		}
	}
	printImage();
}

void mirrorRL() {
	freeOutputMemory();
	outH = inH;
	outW = inW;
	mallocOutputMemory();
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {

			outImage[i][k] = inImage[i][(inW - 1) - k];

		}
	}
	printImage();

}

void mirrorUD() {
	freeOutputMemory();
	outH = inH;
	outW = inW;
	mallocOutputMemory();
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {

			outImage[i][k] = inImage[(inH - 1) - i][k];
		}
	}
	printImage();

}

void zoomOutAvg() {
	int scale = getIntValue();
	//�޸� ����
	freeOutputMemory();
	outH = (int)(inH / scale);
	outW = (int)(inW / scale);
	//�޸� �Ҵ�
	mallocOutputMemory();
	mallocTempMemory();
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			temp[i][k] = 0;
		}
	}
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			temp[(int)(i / scale)][(int)(k / scale)] += (int)inImage[i][k];
		}
	}
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			outImage[i][k] = (unsigned char)(temp[i][k] / (scale * scale));
		}
	}
	freeTempMemory();
	printImage();

}

void histoStretch() {//������׷� ������Ī �˰���

	// �Է� �迭 --> ��� �迭
	freeOutputMemory();
	outH = inH;
	outW = inW;
	mallocOutputMemory();

	//��¥ ����ó�� �˰���
	//new = (old - low) / (high - low) * 255.0
	int high=inImage[0][0], low = inImage[0][0];
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] < low)
				low = inImage[i][k];
			if (inImage[i][k] > high)
				high = inImage[i][k];
		}
	}
	int old, new;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			old = inImage[i][k];
			new = (int)((double)(old - low) / (double)(high -low) * 255.0);

			if (new > 255)
				new = 255;
			if (new < 0)
				new = 0;
			outImage[i][k] = new;
		}
	}
	printImage();


}

void endIn() {//������ �˰���

	// �Է� �迭 --> ��� �迭
	freeOutputMemory();
	outH = inH;
	outW = inW;
	mallocOutputMemory();

	//��¥ ����ó�� �˰���
	//new = (old - low) / (high - low) * 255.0
	int high = inImage[0][0], low = inImage[0][0];
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			if (inImage[i][k] < low)
				low = inImage[i][k];
			if (inImage[i][k] > high)
				high = inImage[i][k];
		}
	}
	high -= 50;
	low += 50;
	int old, new;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			old = inImage[i][k];
			new = (int)((double)(old - low) / (double)(high - low) * 255.0);

			if (new > 255)
				new = 255;
			if (new < 0)
				new = 0;
			outImage[i][k] = new;
		}
	}
	printImage();


}

void histoEqual() { // ������ �׷� ��Ȱȭ �˰���
	// �Է� �迭 --> ��� �迭
	freeOutputMemory();
	outH = inH;
	outW = inW;
	mallocOutputMemory();

	//1�ܰ� : �󵵼� ����(=������׷�) histo[256]
	int histo[256] = { 0, };
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			histo[inImage[i][k]]++;
		}
	}
	//2�ܰ� : ���� ������׷� ����
	int sumHisto[256] = { 0, };
	sumHisto[0] = histo[0];
	for (int i = 1; i < 256; i++) {
		sumHisto[i] = sumHisto[i - 1] + histo[i];
	}

	//3�ܰ� : ����ȭ�� ������׷� ���� normalHisto = sumHisto *(1.0/inH*inW)*255.0;
	double normalHisto[256] = { 1.0, };
	for (int i = 0; i < 256; i++) {
		normalHisto[i] = sumHisto[i] * (1.0 / (inH * inW)) * 255.0;
	}

	//4�ܰ� : inImage�� ����ȭ�� �̹����� ġȯ

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i][k] = (unsigned char)normalHisto[inImage[i][k]];
		}
	}
	printImage();
}

int compare(const void* a, const void* b) {
	int num1 = *(int*)a;    // void �����͸� int �����ͷ� ��ȯ�� �� �������Ͽ� ���� ������
	int num2 = *(int*)b;    // void �����͸� int �����ͷ� ��ȯ�� �� �������Ͽ� ���� ������

	if (num1 < num2)    // a�� b���� ���� ����
		return -1;      // -1 ��ȯ

	if (num1 > num2)    // a�� b���� Ŭ ����
		return 1;       // 1 ��ȯ

	return 0;    // a�� b�� ���� ���� 0 ��ȯ
}

void zoomOutMid() {
	int scale = getIntValue();
	//�޸� ����
	freeOutputMemory();
	outH = (int)(inH / scale);
	outW = (int)(inW / scale);
	//�޸� �Ҵ�

	mallocOutputMemory();
	int* temp;
	temp = (int*)malloc((sizeof(int)) * (scale*scale));//inImage ũ�⸸ŭ�� ������ �迭 �����
	int num = 0;
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			for (int j = 0; j < scale ; j++) {
				for (int m = 0; m < scale; m++) {
					temp[num] = inImage[scale * i + j][scale  * k + m];
					num++;
				}
			}
			num = 0;
			qsort(temp, sizeof(temp) / sizeof(temp[0]), sizeof(temp[0]), compare);
			outImage[i][k] = temp[(scale * scale) / 2];
		}
	}
	
	free(temp);
	printImage();

}

void emboss() { // ȭ�ҿ��� ó�� : ������ �˰���
	// �޸� ����
	freeOutputMemory();
	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;
	// �޸� �Ҵ�	
	mallocOutputMemory();

	////////////////
	// ȭ�� ���� ó��
	/////////////////
	double mask[3][3] = { {-1.0, 0.0, 0.0}, // ������ ����ũ
						  { 0.0, 0.0, 0.0},
						  { 0.0, 0.0, 1.0} };
	// �ӽ� �޸� �Ҵ�(�Ǽ���)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// �ӽ� �Է� �޸𸮸� �ʱ�ȭ(127) : �ʿ�� ��հ�
	for (int i = 0; i < inH + 2; i++)
		for (int k = 0; k < inW + 2; k++)
			tmpInImage[i][k] = 127;

	// �Է� �̹��� --> �ӽ� �Է� �̹���
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// *** ȸ�� ���� ***
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// ����ũ(3x3) �� ������ �߽������� 3x3�� ���ϱ�
			S = 0.0; // ����ũ 9���� �Է°� 9���� ���� ���ؼ� ���� ��.

			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					S += tmpInImage[i + m][k + n] * mask[m][n];

			tmpOutImage[i][k] = S;
		}
	}
	// ��ó�� (����ũ ���� �հ迡 ����...)
	for (int i = 0; i < outH; i++)
		for (int k = 0; k < outW; k++)
			tmpOutImage[i][k] += 127.0;

	// �ӽ� ��� ����--> ��� ����. 
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (tmpOutImage[i][k] < 0.0)
				outImage[i][k] = 0;
			else if (tmpOutImage[i][k] > 255.0)
				outImage[i][k] = 255;
			else
				outImage[i][k] = (unsigned char)tmpOutImage[i][k];
		}
	}
	freeDoubleMemory(tmpInImage, inH + 2);
	freeDoubleMemory(tmpOutImage, outH);
	printImage();
}

void blur() { // ȭ�ҿ��� ó�� : ���� �˰���
	// �޸� ����
	freeOutputMemory();
	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;
	// �޸� �Ҵ�	
	mallocOutputMemory();

	////////////////
	// ȭ�� ���� ó��
	/////////////////
	double mask[3][3] = { {1.0/9, 1.0 / 9, 1.0 / 9}, // ���� ����ũ
						  { 1.0 / 9, 1.0 / 9, 1.0 / 9},
						  { 1.0 / 9, 1.0 / 9, 1.0/9} };
	// �ӽ� �޸� �Ҵ�(�Ǽ���)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// �ӽ� �Է� �޸𸮸� �ʱ�ȭ(127) : �ʿ�� ��հ�
	for (int i = 0; i < inH + 2; i++)
		for (int k = 0; k < inW + 2; k++)
			tmpInImage[i][k] = 127;

	// �Է� �̹��� --> �ӽ� �Է� �̹���
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// *** ȸ�� ���� ***
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// ����ũ(3x3) �� ������ �߽������� 3x3�� ���ϱ�
			S = 0.0; // ����ũ 9���� �Է°� 9���� ���� ���ؼ� ���� ��.

			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					S += tmpInImage[i + m][k + n] * mask[m][n];

			tmpOutImage[i][k] = S;
		}
	}
	//// ��ó�� (����ũ ���� �հ迡 ����...)
	//for (int i = 0; i < outH; i++)
	//	for (int k = 0; k < outW; k++)
	//		tmpOutImage[i][k] += 127.0;

	// �ӽ� ��� ����--> ��� ����. 
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (tmpOutImage[i][k] < 0.0)
				outImage[i][k] = 0;
			else if (tmpOutImage[i][k] > 255.0)
				outImage[i][k] = 255;
			else
				outImage[i][k] = (unsigned char)tmpOutImage[i][k];
		}
	}
	freeDoubleMemory(tmpInImage, inH + 2);
	freeDoubleMemory(tmpOutImage, outH);
	printImage();
}

void sharpning() { // ȭ�ҿ��� ó�� : ������ �˰���
	// �޸� ����
	freeOutputMemory();
	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;
	// �޸� �Ҵ�	
	mallocOutputMemory();

	////////////////
	// ȭ�� ���� ó��
	/////////////////
	double mask[3][3] = { {-1.0, -1.0, -1.0}, // ������ ����ũ
						  {-1.0, 9.0 , -1.0},
						  {-1.0, -1.0, -1.0} };
	// �ӽ� �޸� �Ҵ�(�Ǽ���)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// �ӽ� �Է� �޸𸮸� �ʱ�ȭ(127) : �ʿ�� ��հ�
	for (int i = 0; i < inH + 2; i++)
		for (int k = 0; k < inW + 2; k++)
			tmpInImage[i][k] = 127;

	// �Է� �̹��� --> �ӽ� �Է� �̹���
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// *** ȸ�� ���� ***
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// ����ũ(3x3) �� ������ �߽������� 3x3�� ���ϱ�
			S = 0.0; // ����ũ 9���� �Է°� 9���� ���� ���ؼ� ���� ��.

			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					S += tmpInImage[i + m][k + n] * mask[m][n];

			tmpOutImage[i][k] = S;
		}
	}
	//// ��ó�� (����ũ ���� �հ迡 ����...)
	//for (int i = 0; i < outH; i++)
	//	for (int k = 0; k < outW; k++)
	//		tmpOutImage[i][k] += 127.0;

	// �ӽ� ��� ����--> ��� ����. 
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (tmpOutImage[i][k] < 0.0)
				outImage[i][k] = 0;
			else if (tmpOutImage[i][k] > 255.0)
				outImage[i][k] = 255;
			else
				outImage[i][k] = (unsigned char)tmpOutImage[i][k];
		}
	}
	freeDoubleMemory(tmpInImage, inH + 2);
	freeDoubleMemory(tmpOutImage, outH);
	printImage();
}

void sharpning2() {
	// �޸� ����
	freeOutputMemory();
	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;
	// �޸� �Ҵ�	
	mallocOutputMemory();

	////////////////
	// ȭ�� ���� ó��
	/////////////////
	double mask[3][3] = { {0, -1.0, 0}, // ������ ����ũ2
						  {-1.0, 5.0 ,-1.0},
						  {0, -1.0 , 0} };
	// �ӽ� �޸� �Ҵ�(�Ǽ���)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// �ӽ� �Է� �޸𸮸� �ʱ�ȭ(127) : �ʿ�� ��հ�
	for (int i = 0; i < inH + 2; i++)
		for (int k = 0; k < inW + 2; k++)
			tmpInImage[i][k] = 127;

	// �Է� �̹��� --> �ӽ� �Է� �̹���
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// *** ȸ�� ���� ***
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// ����ũ(3x3) �� ������ �߽������� 3x3�� ���ϱ�
			S = 0.0; // ����ũ 9���� �Է°� 9���� ���� ���ؼ� ���� ��.

			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					S += tmpInImage[i + m][k + n] * mask[m][n];

			tmpOutImage[i][k] = S;
		}
	}

	// �ӽ� ��� ����--> ��� ����. 
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (tmpOutImage[i][k] < 0.0)
				outImage[i][k] = 0;
			else if (tmpOutImage[i][k] > 255.0)
				outImage[i][k] = 255;
			else
				outImage[i][k] = (unsigned char)tmpOutImage[i][k];
		}
	}
	freeDoubleMemory(tmpInImage, inH + 2);
	freeDoubleMemory(tmpOutImage, outH);
	printImage();
}
void rotate3() { // ȸ�� �˰���
	// �Է� �迭 --> ��� �迭
	freeOutputMemory();
	int degree = getIntValue();
	double radian = -degree * 3.141592 / 180.0;
	double radian90 = 90 * 3.141592 / 180.0;
	outH =(int)(inW*cos(radian90 + radian)+ (inH *(cos(radian)))); //outImage �迭�� ũ�⸦ ���� ������ �°� ����
	outW = (int)(inH*cos(radian90 + radian) +(inW *(cos(radian))));
	
	mallocOutputMemory();
	
	//xd = cos*(xs)-sin*(ys)
	//yd = sin*(sx) + cos*(ys)

	int cx = inH / 2;
	int cy = inW / 2;
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			int xd = i;
			int yd = k;

			int xs = (int)(cos(radian) * (xd - outH / 2) + sin(radian) * (yd - outW / 2));
			int ys = (int)(-sin(radian) * (xd - outH / 2) + cos(radian) * (yd - outW / 2));
			xs += inH/2;
			ys += inW/2;
			if ((0 <= xs && xs < inH) && (0 <= ys && ys < inW))
				outImage[xd][yd] = inImage[xs][ys];
		}
	}
	printImage();
}

void zoomIn3() {
	// �Է� �迭 --> ��� �迭
	freeOutputMemory();
	int scale = getIntValue();
	outH = (inH*scale);
	outW = (inW*scale);
	mallocOutputMemory();

	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			outImage[i * scale][k * scale] = inImage[i][k];
		}
	}

	int i_h, i_w;
	double r_H, r_W, s_H, s_W;
	double C1, C2, C3, C4;

	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			r_H = i / scale;								// ���� ��ġ�� scale ũ���� ��� �� �� ��° ������ ����
			r_W = k / scale;

			i_h = (int)floor(r_H);							// ����� ���μ��� ĭ�� ����
			i_w = (int)floor(r_W);

			s_H = r_H - i_h;								// ���� ��� �������� ��ġ�� ���� 
			s_W = r_W - i_w;

			if (i_h < 0 | i_h >= (inH - 1) | i_w < 0 | i_w >= (inW - 1)) {
				outImage[i][k] = 255;						// ���� ĭ�� ������ ������ �� ���ٸ� ������� ����
				// �ַ� ���ʰ� �Ʒ����� �׷� ������ ����
			}
			else {
				C1 = (double)inImage[i_h][i_w];				// �� ����� ����
				C2 = (double)inImage[i_h][i_w + 1];			// �� ����� ����
				C3 = (double)inImage[i_h + 1][i_w + 1];		// �� �ϴ��� ����
				C4 = (double)inImage[i_h + 1][i_w];			// �� �ϴ��� ����
				outImage[i][k] = (unsigned char)(C1 * (1 - s_H) * (1 - s_W)
					+ C2 * s_W * (1 - s_H) + C3 * s_W * s_H + C4 * (1 - s_W) * s_H);
				// �� ������� ��ġ ������ �ݿ��ؼ� ���� �� ���� 
			}

		}
	}
	
	printImage();
}

void edgever() {// ���� ���� ����
	// �޸� ����
	freeOutputMemory();
	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;
	// �޸� �Ҵ�	
	mallocOutputMemory();

	////////////////
	// ȭ�� ���� ó��
	/////////////////
	double mask[3][3] = { {0.0, 0.0, 0.0}, // ���� ���� ����ũ
						  { -1.0, 1.0, 0.0},
						  { 0.0, 0.0, 0.0} };
	// �ӽ� �޸� �Ҵ�(�Ǽ���)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// �ӽ� �Է� �޸𸮸� �ʱ�ȭ(127) : �ʿ�� ��հ�
	for (int i = 0; i < inH + 2; i++)
		for (int k = 0; k < inW + 2; k++)
			tmpInImage[i][k] = 127;

	// �Է� �̹��� --> �ӽ� �Է� �̹���
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// *** ȸ�� ���� ***
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// ����ũ(3x3) �� ������ �߽������� 3x3�� ���ϱ�
			S = 0.0; // ����ũ 9���� �Է°� 9���� ���� ���ؼ� ���� ��.

			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					S += tmpInImage[i + m][k + n] * mask[m][n];

			tmpOutImage[i][k] = S;
		}
	}
	// ��ó�� (����ũ ���� �հ迡 ����...)
	for (int i = 0; i < outH; i++)
		for (int k = 0; k < outW; k++)
			tmpOutImage[i][k] += 50.0;

	// �ӽ� ��� ����--> ��� ����. 
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (tmpOutImage[i][k] < 0.0)
				outImage[i][k] = 0;
			else if (tmpOutImage[i][k] > 255.0)
				outImage[i][k] = 255;
			else
				outImage[i][k] = (unsigned char)tmpOutImage[i][k];
		}
	}
	freeDoubleMemory(tmpInImage, inH + 2);
	freeDoubleMemory(tmpOutImage, outH);
	printImage();

}

void edgehor() { //���� ���� ����
	// �޸� ����
	freeOutputMemory();
	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;
	// �޸� �Ҵ�	
	mallocOutputMemory();

	////////////////
	// ȭ�� ���� ó��
	/////////////////
	double mask[3][3] = { {0.0, -1.0, 0.0}, // ������ ����ũ
						  { 0.0, 1.0, 0.0},
						  { 0.0, 0.0, 0.0} };
	// �ӽ� �޸� �Ҵ�(�Ǽ���)
	double** tmpInImage = mallocDoubleMemory(inH + 2, inW + 2);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// �ӽ� �Է� �޸𸮸� �ʱ�ȭ(127) : �ʿ�� ��հ�
	for (int i = 0; i < inH + 2; i++)
		for (int k = 0; k < inW + 2; k++)
			tmpInImage[i][k] = 127;

	// �Է� �̹��� --> �ӽ� �Է� �̹���
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 1][k + 1] = inImage[i][k];

	// *** ȸ�� ���� ***
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// ����ũ(3x3) �� ������ �߽������� 3x3�� ���ϱ�
			S = 0.0; // ����ũ 9���� �Է°� 9���� ���� ���ؼ� ���� ��.

			for (int m = 0; m < 3; m++)
				for (int n = 0; n < 3; n++)
					S += tmpInImage[i + m][k + n] * mask[m][n];

			tmpOutImage[i][k] = S;
		}
	}
	// ��ó�� (����ũ ���� �հ迡 ����...)
	for (int i = 0; i < outH; i++)
		for (int k = 0; k < outW; k++)
			tmpOutImage[i][k] += 50.0;

	// �ӽ� ��� ����--> ��� ����. 
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (tmpOutImage[i][k] < 0.0)
				outImage[i][k] = 0;
			else if (tmpOutImage[i][k] > 255.0)
				outImage[i][k] = 255;
			else
				outImage[i][k] = (unsigned char)tmpOutImage[i][k];
		}
	}
	freeDoubleMemory(tmpInImage, inH + 2);
	freeDoubleMemory(tmpOutImage, outH);
	printImage();


}

void blur99() { // ȭ�ҿ��� ó�� : ���� �˰���
	// �޸� ����
	freeOutputMemory();
	// (�߿�!) ��� �̹����� ũ�⸦ ���� ---> �˰��� ����
	outH = inH;
	outW = inW;
	// �޸� �Ҵ�	
	mallocOutputMemory();

	////////////////
	// ȭ�� ���� ó��
	/////////////////
	double mask[9][9] = { {1.0 / 81, 1.0 / 81, 1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81}, // ���� ����ũ
						  { 1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81},
						  { 1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81},
							{1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81},
							{1.0 / 81, 1.0 / 81, 1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81},
							{ 1.0 / 81, 1.0 / 81, 1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81 },
							{ 1.0 / 81, 1.0 / 81, 1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81 },
							{ 1.0 / 81, 1.0 / 81, 1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81 },
							{ 1.0 / 81, 1.0 / 81, 1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81,1.0 / 81 } };
	// �ӽ� �޸� �Ҵ�(�Ǽ���)
	double** tmpInImage = mallocDoubleMemory(inH + 8, inW + 8);
	double** tmpOutImage = mallocDoubleMemory(outH, outW);

	// �ӽ� �Է� �޸𸮸� �ʱ�ȭ(127) : �ʿ�� ��հ�
	for (int i = 0; i < inH + 8; i++)
		for (int k = 0; k < inW + 8; k++)
			tmpInImage[i][k] = 127;

	// �Է� �̹��� --> �ӽ� �Է� �̹���
	for (int i = 0; i < inH; i++)
		for (int k = 0; k < inW; k++)
			tmpInImage[i + 3][k + 3] = inImage[i][k];

	// *** ȸ�� ���� ***
	double S;
	for (int i = 0; i < inH; i++) {
		for (int k = 0; k < inW; k++) {
			// ����ũ(3x3) �� ������ �߽������� 9x3�� ���ϱ�
			S = 0.0; // ����ũ 81���� �Է°� 81���� ���� ���ؼ� ���� ��.

			for (int m = 0; m < 9; m++)
				for (int n = 0; n < 9; n++)
					S += tmpInImage[i + m][k + n] * mask[m][n];

			tmpOutImage[i][k] = S;
		}
	}
	//// ��ó�� (����ũ ���� �հ迡 ����...)
	//for (int i = 0; i < outH; i++)
	//	for (int k = 0; k < outW; k++)
	//		tmpOutImage[i][k] += 127.0;

	// �ӽ� ��� ����--> ��� ����. 
	for (int i = 0; i < outH; i++) {
		for (int k = 0; k < outW; k++) {
			if (tmpOutImage[i][k] < 0.0)
				outImage[i][k] = 0;
			else if (tmpOutImage[i][k] > 255.0)
				outImage[i][k] = 255;
			else
				outImage[i][k] = (unsigned char)tmpOutImage[i][k];
		}
	}
	freeDoubleMemory(tmpInImage, inH + 8);
	freeDoubleMemory(tmpOutImage, outH);
	printImage();
}