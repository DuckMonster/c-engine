#pragma once

struct Color_32
{
	// Aligned with little-endianess
	u8 a, b, g, r;
};

inline Color_32 color_make(u8 r, u8 g, u8 b, u8 a)
{
	Color_32 c;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;

	return c;
}
inline Color_32 color_hex(u32 hex)
{
	Color_32 result;
	*((u32*)&result) = hex;

	return result;
}

inline Vec4 color_to_vec(const Color_32& color)
{
	return Vec4(
		color.r / 255.f,
		color.g / 255.f,
		color.b / 255.f,
		color.a / 255.f
	);
}

inline Color_32 vec_to_color(const Vec4& vec)
{
	return color_make(
		clamp(vec.x * 255, 0, 255),
		clamp(vec.y * 255, 0, 255),
		clamp(vec.z * 255, 0, 255),
		clamp(vec.w * 255, 0, 255)
	);
}

const Color_32 Color_Red_32 = color_hex(0xFF0000FF);
const Color_32 Color_Green_32 = color_hex(0x00FF00FF);
const Color_32 Color_Blue_32 = color_hex(0x0000FFFF);
const Color_32 Color_White_32 = color_hex(0xFFFFFFFF);
const Color_32 Color_Light_Gray_32 = color_hex(0xCCCCCCFF);
const Color_32 Color_Gray_32 = color_hex(0x888888FF);
const Color_32 Color_Dark_Gray_32 = color_hex(0x444444FF);
const Color_32 Color_Black_32 = color_hex(0x000000FF);
const Color_32 Color_Transparent_32 = color_hex(0x0);

const Vec4 Color_Red = color_to_vec(Color_Red_32);
const Vec4 Color_Green = color_to_vec(Color_Green_32);
const Vec4 Color_Blue = color_to_vec(Color_Blue_32);
const Vec4 Color_White = color_to_vec(Color_White_32);
const Vec4 Color_Light_Gray = color_to_vec(Color_Light_Gray_32);
const Vec4 Color_Gray = color_to_vec(Color_Gray_32);
const Vec4 Color_Dark_Gray = color_to_vec(Color_Dark_Gray_32);
const Vec4 Color_Black = color_to_vec(Color_Black_32);
const Vec4 Color_Transparent = color_to_vec(Color_Transparent_32);