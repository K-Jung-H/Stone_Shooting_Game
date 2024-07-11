#pragma once
#include "Camera.h"

class UICamera : public CCamera
{
public:
	RECT Monitor_Area;
	bool Active = false;
	UICamera(RECT& monitor_area);
	virtual ~UICamera() {}

	virtual int Update(float fTimeElapsed, bool sign);
	virtual void Reset();
	void SetOrthographicProjection(float viewWidth, float viewHeight, float nearZ, float farZ);
};

class BAR_UI : public UICamera
{
public:
	BAR_UI(RECT& monitor_area);
	~BAR_UI();
	virtual int Update(float fTimeElapsed, bool power_charge);
	void Reset();
protected:
	int Max_Width;
	int Max_Height;

	int Degree = 0;
	bool Degree_increase = true;
};