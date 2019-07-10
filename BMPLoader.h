#pragma once
#include "Component\Component.h"
#include "GameObject.h"
#include <iostream>

ENGINE_USING

class CBMPLoader 
{
public:
	CBMPLoader();
	~CBMPLoader();

private:
	int		m_iFormat;
	int		m_iWidth;
	int		m_iHeight;

	char*	m_pPixel;

public:
	void LoadBMPFile(const TCHAR* pFileName, const string& strPathKey = TEXTURE_PATH);
	void LoadBMPFileFromLocal(const char* pFileName, const char* pFilefullpath = nullptr);

	Vector3 GetPixelColor(int x, int y);

public:
	void GetBMPInfoTextFile(const char* pFilefullpath = nullptr);
};

