#include "BMPLoader.h"

#include "PathManager.h"

CBMPLoader::CBMPLoader()
{
	m_iFormat = -1;
	m_pPixel = nullptr;
}

CBMPLoader::~CBMPLoader()
{
	if (!m_pPixel)
		delete[] m_pPixel;
}

void CBMPLoader::LoadBMPFile(const TCHAR* pFileName, const string& strPathKey)
{
	FILE* pFile = nullptr;

	TCHAR pPath[MAX_PATH] = { 0, };

	lstrcpy(pPath, GET_SINGLE(CPathManager)->FindPath(strPathKey));
	lstrcat(pPath, pFileName);

	char pFullPath[MAX_PATH] = { 0, };
#ifdef UNICODE
	int len = WideCharToMultiByte(CP_ACP, 0, pPath, lstrlen(pPath), 0, 0, 0, 0);
	WideCharToMultiByte(CP_ACP, 0, pPath, lstrlen(pPath), pFullPath, len, 0, 0);
#else
	strcpy(pFullPath, pPath);
#endif	
	fopen_s(&pFile, pFullPath, " rb");
	
	if (!pFile)
		return;
	
	BITMAPFILEHEADER	tFile;
	BITMAPINFOHEADER	tInfo;

	fread(&tFile, sizeof(tFile), 1, pFile);
	fread(&tInfo, sizeof(tInfo), 1, pFile);

	switch (tInfo.biBitCount)
	{
	case 8:
		m_iFormat = 1;
		break;
	case 24:
		m_iFormat = 3;
		break;
	case 32:
		m_iFormat = 4;
		break;
	}

	// image must be m_iFormat 4. 
	// bmp pixel saved as ( b g r ) 3byte

	int iBMPPixelSize = tInfo.biWidth * tInfo.biHeight * (m_iFormat+1);

	m_iWidth = tInfo.biWidth;
	m_iHeight = tInfo.biHeight;

	m_pPixel = new char[iBMPPixelSize];
	memset(m_pPixel, 0, sizeof(char) * iBMPPixelSize);
	fread(m_pPixel, sizeof(char), iBMPPixelSize, pFile);

	// reverse bmp image
	int iLineSize = tInfo.biWidth * m_iFormat;
	char* pLine = new char[iLineSize];

	for (int i = 0; i < tInfo.biHeight / 2; ++i)
	{
		int	iStart = i * iLineSize;
		int	iEnd = (tInfo.biHeight - i - 1) * iLineSize;

		memcpy(pLine, &m_pPixel[iStart], iLineSize);
		memcpy(&m_pPixel[iStart], &m_pPixel[iEnd], iLineSize);
		memcpy(&m_pPixel[iEnd], pLine, iLineSize);
	}

	delete[]	pLine;

	fclose(pFile);
}

void CBMPLoader::LoadBMPFileFromLocal(const char* pFileName, const char* pFilefullpath)
{
	FILE* pFile = nullptr;

	char pPath[MAX_PATH] = { 0, };
	if (pFilefullpath)
	{
		strcpy(pPath, pFilefullpath);
		strcat(pPath, "\\");
		strcat(pPath, pFileName);
	}
	else
	{
		strcpy(pPath, pFileName);
	}

	fopen_s(&pFile, pPath, "rb");
	
	if (!pFile)
		return;

	BITMAPFILEHEADER	tFile;
	BITMAPINFOHEADER	tInfo;

	fread(&tFile, sizeof(tFile), 1, pFile);
	fread(&tInfo, sizeof(tInfo), 1, pFile);

	// image must be m_iFormat 4. 
	// bmp pixel saved as ( b g r ) 3byte
	if(tInfo.biBitCount != 32)
	{
		return;
		MessageBoxEx(0, TEXT("32bit bmp 이미지가 아닙니다."), TEXT("BMPLoader"), MB_OK, 0);
	}

	m_iFormat = 4;
	int iBMPPixelSize = tInfo.biWidth * tInfo.biHeight * m_iFormat;

	m_iWidth = tInfo.biWidth;
	m_iHeight = tInfo.biHeight;

	m_pPixel = new char[iBMPPixelSize];
	memset(m_pPixel, 0, sizeof(char) * iBMPPixelSize);
	fread(m_pPixel, sizeof(char), iBMPPixelSize, pFile);

	// reverse bmp image
	int iLineSize = tInfo.biWidth * m_iFormat;
	char* pLine = new char[iLineSize];

	for (int i = 0; i < tInfo.biHeight / 2; ++i)
	{
		int	iStart = i * iLineSize;
		int	iEnd = (tInfo.biHeight - i - 1) * iLineSize;

		memcpy(pLine, &m_pPixel[iStart], iLineSize);
		memcpy(&m_pPixel[iStart], &m_pPixel[iEnd], iLineSize);
		memcpy(&m_pPixel[iEnd], pLine, iLineSize);
	}

	delete[]	pLine;

	fclose(pFile);
}

Vector3 CBMPLoader::GetPixelColor(int x, int y)
{
	if (x >= m_iWidth || y >= m_iHeight)
		return Vector3(-1, -1, -1);
	
	int Index = y * m_iWidth * (m_iFormat) + x * (m_iFormat);

	char b = m_pPixel[Index];
	char g = m_pPixel[Index+1];
	char r = m_pPixel[Index+2];

	int cr= 0 , cg = 0, cb = 0;
	cr = 0x000000ff & r;
	cg = 0x000000ff & g;
	cb = 0x000000ff & b;

 	return Vector3(cr, cg, cb);
}

void CBMPLoader::GetBMPInfoTextFile(const char* pFilefullpath)
{
	FILE* pPixel;
	if (pFilefullpath)
	{
		fopen_s(&pPixel, pFilefullpath, "wt");
	}
	else
	{
		fopen_s(&pPixel, "../Bin/Texture/bmpinfo.txt", "wt");
	}

	if (pPixel)
	{

		for (int i = 0; i < m_iWidth; ++i)
		{
			fprintf_s(pPixel, "%d번째 줄 : \n", i);
			for (int j = 0; j < m_iHeight; ++j)
			{
				int index = j * m_iWidth * m_iFormat + i * m_iFormat;
				char r = m_pPixel[index];
				char g = m_pPixel[index + 1];
				char b = m_pPixel[index + 2];

				int cr = 0, cg = 0, cb = 0;
				cb = 0x000000ff & r;
				cg = 0x000000ff & g;
				cr = 0x000000ff & b;

				fprintf_s(pPixel, "%d %d %d\t", cr, cg, cb);
			}
			fprintf_s(pPixel,"\n");
		}
		fclose(pPixel);
	}
}
 