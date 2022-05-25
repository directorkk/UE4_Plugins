#include "Vector3d.h"



Vector3::Vector3()
{
	x = y = z = 0;
}

Vector3::Vector3(double initX)
{
	x = initX;
	y = z = 0;
}

Vector3::Vector3(double initX, double initY)
{
	x = initX;
	y = initY;
	z = 0;
}

Vector3::Vector3(double initX, double initY, double initZ)
{
	x = initX;
	y = initY;
	z = initZ;
}

Vector3::~Vector3()
{

}

void Vector3::Change(Vector3 changeVec)
{
	x = changeVec.x;
	y = changeVec.y;
	z = changeVec.z;
}

void Vector3::Change(double changeX, double changeY, double changeZ)
{
	x = changeX;
	y = changeY;
	z = changeZ;
}

void Vector3::ChangeX(double changeX)
{
	x = changeX;
}

void Vector3::ChangeY(double changeY)
{
	y = changeY;
}

void Vector3::ChangeZ(double changeZ)
{
	z = changeZ;
}

double Vector3::Dot(double dotX, double dotY, double dotZ)
{
	return (x*dotX + y * dotY + z * dotZ);
}

double Vector3::Dot(Vector3 dotVec)
{
	return (x*dotVec.x + y * dotVec.y + z * dotVec.z);
}

double Vector3::Length()
{
	return sqrt(x*x + y * y + z * z);
}

void Vector3::Normalize()
{
	double veclen = sqrt(x*x + y * y + z * z);
	if (veclen != 0) {
		x *= 1 / veclen;
		y *= 1 / veclen;
		z *= 1 / veclen;
	}
}

Vector3 Vector3::operator+(const Vector3& anoVec)
{
	return Vector3(x + anoVec.x, y + anoVec.y, z + anoVec.z);
}

Vector3 Vector3::operator-(const Vector3& anoVec)
{
	return Vector3(x - anoVec.x, y - anoVec.y, z - anoVec.z);
}

Vector3 Vector3::operator*(double num)
{
	return Vector3(x*num, y*num, z*num);
}

Vector3 Vector3::operator/(double num)
{
	return Vector3(x / num, y / num, z / num);
}

Vector3& Vector3::operator+=(const Vector3& anoVec)
{
	x += anoVec.x;
	y += anoVec.y;
	z += anoVec.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& anoVec)
{
	x -= anoVec.x;
	y -= anoVec.y;
	z -= anoVec.z;
	return *this;
}

Vector3& Vector3::operator*=(double num)
{
	x *= num;
	y *= num;
	z *= num;
	return *this;
}

Vector3& Vector3::operator/=(double num)
{
	x /= num;
	y /= num;
	z /= num;
	return *this;
}

bool Vector3::operator==(const Vector3 anoVec)
{
	return (x == anoVec.x && y == anoVec.y && z == anoVec.z);
}

bool Vector3::operator!=(const Vector3 anoVec)
{
	return (x != anoVec.x || y != anoVec.y || z != anoVec.z);
}

std::ostream& operator<<(std::ostream& out, const Vector3& vec)
{
	out << vec.x << ", " << vec.y << ", " << vec.z << std::endl;
	return out;
}

Vector4::Vector4() 
{
	x = y = z = w = 0;
}

Vector4::Vector4(double initX)
{
	x = y = z = w = initX;
}
Vector4::Vector4(double initX, double initY, double initZ, double initW)
{
	x = initX;
	y = initY;
	z = initZ;
	w = initW;
}