// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <iostream>
#include <math.h>

/**
 *
 */
class Vector2 {
public:
	double x, y;
	Vector2() { x = y = 0; }
	Vector2(double initX) { x = initX; }
	Vector2(double initX, double initY) { x = initX;	y = initY; }
};

class Vector3 : public Vector2
{
public:
	double z;
	Vector3();
	Vector3(double initX);
	Vector3(double initX, double initY);
	Vector3(double initX, double initY, double initZ);
	~Vector3();

	void Change(Vector3 changeVec);
	void Change(double changeX, double changeY, double changeZ);
	void ChangeX(double changeX);
	void ChangeY(double changeY);
	void ChangeZ(double changeZ);
	double Dot(double dotX, double dotY, double dotZ);
	double Dot(Vector3 dotVec);
	double Length();
	void Normalize();

	Vector3 operator+(const Vector3& anoVec);
	Vector3 operator-(const Vector3& anoVec);
	Vector3 operator*(double num);
	Vector3 operator/(double num);

	Vector3& operator+=(const Vector3& anoVec);
	Vector3& operator-=(const Vector3& anoVec);
	Vector3& operator*=(double num);
	Vector3& operator/=(double num);

	bool operator==(const Vector3 anoVec);
	bool operator!=(const Vector3 anoVec);

	friend std::ostream& operator<<(std::ostream& out, const Vector3& vec);
};

class Vector4 : public Vector3
{
public:
	double w;
	Vector4();
	Vector4(double initX);
	Vector4(double initW, double initX, double initY, double initZ);
	~Vector4() {}
};