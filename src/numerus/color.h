/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

#include "../bug/eabi.h"

namespace numerus
{
#pragma pack(push, 1)

	struct color
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t a = 0;

		template <typename A, typename B>
		static constexpr auto umin(A a, B b)
		{ return (decltype(b))(a < b ? a : b); }

		template <typename value>
		static constexpr value maximum(value a, value b)
		{ return a > b ? a : b; }

		static constexpr uint8_t f2u8(float t)
		{
			t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
			return (uint8_t)(t * 255.9f);
		}

		static constexpr uint8_t sat(uint32_t s) { return s > 255 ? 255 : (uint8_t)s; }

		static constexpr uint8_t mul8(uint8_t a, uint8_t b) { return (uint8_t)((a * b) >> 8); }

		static constexpr uint8_t add8(uint8_t a, uint8_t b) { return (uint8_t)umin(0xFF, (a + b)); }

		static constexpr uint8_t sqr8(uint8_t a) { return mul8(a, a); }

		static constexpr uint8_t gamma(uint8_t a) { return (uint8_t)((sqr8(a) + a) >> 1); }

		constexpr color ABGR() const { return {a, b, g, r}; }

		constexpr color() : r(0), g(0), b(0), a(0xFF) {}

		constexpr color(int r, int g, int b, int a = 0xFF)
		: r((uint8_t)r), g((uint8_t)g), b((uint8_t)b), a((uint8_t)a)
		{}

		constexpr color(uint32_t c)
		: color((uint8_t)(c >> 24), (uint8_t)(c >> 16), (uint8_t)(c >> 8), (uint8_t)(c >> 0))
		{}

		explicit constexpr color(uint8_t i) : color(i, i, i) {}

		explicit constexpr color(int c) : color((uint32_t)c) {}

		explicit constexpr operator uint32_t() const
		{ return (uint32_t)a << 0 | (uint32_t)b << 8 | (uint32_t)g << 16 | (uint32_t)r << 24; }

		constexpr bool operator!=(color c) const
		{ return c.r != r || c.g != g || c.b != b || c.a != a; }

		constexpr bool operator==(color c) const
		{ return c.r == r && c.g == g && c.b == b && c.a == a; }

		constexpr color operator+(color c) const
		{ return color(add8(r, c.r), add8(g, c.g), add8(b, c.b), add8(a, c.a)); }

		constexpr color& operator+=(color c) { return *this = *this + c; }

		constexpr color operator*(float s) const { return this->operator*(f2u8(s)); }

		constexpr color operator*(color c) const
		{ return color(mul8(r, c.r), mul8(g, c.g), mul8(b, c.b), mul8(a, c.a)); }

		constexpr color operator*(uint8_t s) const
		{ return color(mul8(r, s), mul8(g, s), mul8(b, s), a); }

		constexpr color scale(uint16_t s) const { return *this * (uint8_t)(s >> 8); }

		template <typename T>
		constexpr color operator*(T i) const
		{
			uint8_t s = (uint8_t)i;
			return color(mul8(r, s), mul8(g, s), mul8(b, s), a);
		}

		constexpr color apply_alpha() const { return color{mul8(r, a), mul8(g, a), mul8(b, a)}; }

		constexpr color alpha(uint8_t a) const { return color{r, g, b, a}; }

		constexpr color alpha(float a) const { return color{r, g, b, f2u8(a)}; }

		constexpr color alpha_mul(float f) const
		{ return f >= 1.0f ? *this : color{r, g, b, mul8(a, f2u8(f))}; }

		constexpr color gamma() const { return color(gamma(r), gamma(g), gamma(b), a); }

		constexpr uint32_t to_u32() const { return bug::bit_cast<uint32_t>(*this); }

		static constexpr color from_u32(uint32_t v) { return bug::bit_cast<color>(v); }

		static constexpr uint8_t lerp8(uint8_t t, uint8_t a, uint8_t b)
		{
			const uint32_t s  = 0xFF - t;
			const uint32_t pa = t * b;
			const uint32_t pb = s * a;
			const uint32_t p  = (pa + pb) >> 8;
			return (uint8_t)p;
		}

		static constexpr color blend(uint8_t t, const color& a, const color& b)
		{
			return color(
				lerp8(t, a.r, b.r), lerp8(t, a.g, b.g), lerp8(t, a.b, b.b), lerp8(t, a.a, b.a));
		}

		static constexpr color blend(uint16_t t16, const color& a, const color& b)
		{ return blend((uint8_t)(t16 >> 8), a, b); }

		static constexpr color blend(float t, const color& a, const color& b)
		{
			t = t < 0.0f ? 0.0f : (t > 1.0f ? 1.0f : t);
			return blend((uint8_t)(t * 255.0f), a, b);
		}

		constexpr color max(const color& c) const
		{ return color(maximum(r, c.r), maximum(g, c.g), maximum(b, c.b), maximum(a, c.a)); }

		static constexpr color lerp(uint8_t t, color a, color b)
		{ return color(lerp8(t, a.r, b.r), lerp8(t, a.g, b.g), lerp8(t, a.b, b.b), a.a); }

		static constexpr color lerp(float t, color a, color b) { return lerp(f2u8(t), a, b); }

		static constexpr color clear() { return {0x00, 0x00, 0x00, 0x00}; }

		static constexpr color red() { return {0xFF, 0x00, 0x00}; }

		static constexpr color white() { return {0xFF, 0xFF, 0xFF, 0xFF}; }

		static constexpr color gray6() { return {0xC8, 0xC8, 0xC8, 0xFF}; }

		static constexpr color gray4() { return {0x88, 0x88, 0x88, 0xFF}; }

		static constexpr color gray2() { return {0x44, 0x44, 0x44, 0xFF}; }

		static constexpr color yellow() { return {0xFF, 0xFF, 0x00, 0xFF}; }

		static constexpr color chartreuse() { return {0x80, 0xFF, 0x00, 0xFF}; }

		static constexpr color magenta() { return {0xFF, 0x00, 0xFF, 0xFF}; }

		static constexpr color cyan() { return {0x00, 0xFF, 0xFF, 0xFF}; }

		static constexpr color green() { return {0x00, 0xFF, 0x00, 0xFF}; }

		static constexpr color spring_green() { return {0x00, 0xFF, 0x80, 0xFF}; }

		static constexpr color blue() { return {0x00, 0x00, 0xFF, 0xFF}; }

		static constexpr color dodger_blue() { return {0x00, 0x80, 0xFF, 0xFF}; }

		static constexpr color black() { return {0x00, 0x00, 0x00, 0xFF}; }

		static constexpr color orange() { return {0xFF, 0x45, 0x00, 0xFF}; }

		static constexpr color indigo() { return {0x40, 0x00, 0x89, 0xFF}; }

		static constexpr color violet() { return {0x9f, 0x00, 0xFF, 0xFF}; }

		static constexpr color purple() { return {0x40, 0x00, 0xFF, 0xFF}; }

		static constexpr color from_hsv(float h, float s, float v)
		{
			const float h6 = (h - (float)(int)h) * 6.0f;
			const int	i  = (int)h6;
			const float f  = h6 - (float)i;
			const float p  = v * (1.0f - s);
			const float q  = v * (1.0f - s * f);
			const float t  = v * (1.0f - s * (1.0f - f));
			switch(i)
			{
				case 0: return color{f2u8(v), f2u8(t), f2u8(p)};
				case 1: return color{f2u8(q), f2u8(v), f2u8(p)};
				case 2: return color{f2u8(p), f2u8(v), f2u8(t)};
				case 3: return color{f2u8(p), f2u8(q), f2u8(v)};
				case 4: return color{f2u8(t), f2u8(p), f2u8(v)};
				default: return color{f2u8(v), f2u8(p), f2u8(q)};
			}
		}

		static constexpr color rainbow(int i)
		{
			switch(i & 7)
			{
				default:
				case 0: return red();
				case 1: return orange();
				case 2: return yellow();
				case 3: return green();
				case 4: return blue();
				case 5: return indigo();
				case 6: return violet();
				case 7: return white();
			}
		}

		static constexpr color rainbow12(uint32_t ver)
		{
			constexpr color rb[12] = {
				{0xFF, 0x00, 0x00, 0xFF},
				{0xF2, 0x51, 0x00, 0xFF},
				{0xB4, 0xB5, 0x00, 0xFF},
				{0x4F, 0xF3, 0x00, 0xFF},
				{0x00, 0xFF, 0x00, 0xFF},
				{0x00, 0xF2, 0x51, 0xFF},
				{0x00, 0xB4, 0xB5, 0xFF},
				{0x00, 0x4F, 0xF3, 0xFF},
				{0x00, 0x00, 0xFF, 0xFF},
				{0x51, 0x00, 0xF2, 0xFF},
				{0xB5, 0x00, 0xB4, 0xFF},
				{0xF3, 0x00, 0x4F, 0xFF},
			};

			return rb[ver % 12];
		}

		static constexpr color rainbow13(uint32_t ver)
		{
			constexpr color rb[13] = {
				color::red(),
				color::orange(),
				color::yellow(),
				color::chartreuse(),
				color::green(),
				color::spring_green(),
				color::cyan(),
				color::dodger_blue(),
				color::blue(),
				color::purple(),
				color::violet(),
				color::magenta(),
				color::white(),
			};

			return rb[ver % 13];
		}

		static constexpr color fracbow13(uint16_t ver) { return rainbow13((ver * 13) >> 16); }

		static constexpr color rainbow16(uint32_t ver)
		{
			constexpr color rb[16] = {
				{0xFF, 0x00, 0x00, 0xFF},
				{0xF1, 0x55, 0x00, 0xFF},
				{0xCE, 0x96, 0x00, 0xFF},
				{0xAA, 0xBF, 0x00, 0xFF},
				{0x74, 0xE3, 0x00, 0xFF},
				{0x22, 0xFD, 0x00, 0xFF},
				{0x00, 0xF9, 0x39, 0xFF},
				{0x00, 0xDA, 0x85, 0xFF},
				{0x00, 0xB6, 0xB3, 0xFF},
				{0x00, 0x88, 0xD8, 0xFF},
				{0x00, 0x3F, 0xF7, 0xFF},
				{0x1D, 0x00, 0xFE, 0xFF},
				{0x71, 0x00, 0xE5, 0xFF},
				{0xA8, 0x00, 0xC1, 0xFF},
				{0xCC, 0x00, 0x99, 0xFF},
				{0xF7, 0x00, 0x3f, 0xFF},
			};

			return rb[ver % 16];
		}

		static constexpr color stagger16(uint32_t r) { return rainbow16(r * 3 + 8); }

		static constexpr color continuum(uint32_t ver)
		{
			ver = ver % 0x100;

			const uint32_t idx = ver / 16;
			const uint32_t ofs = (ver % 16) * 16;

			return color::blend((uint8_t)ofs, rainbow16(idx), rainbow16(idx + 1));
		}

		static constexpr color get_vu(float x)
		{
			x = 10.0f * (x >= 0.0f ? x : -x);

			if(x > 8.5f)
			{
				return color::red();
			}
			else if(x > 5.5f)
			{
				return color::yellow();
			}
			else if(x >= 1.0f)
			{
				return color::green();
			}
			else if(x < 0.01f)
			{
				return color::black();
			}

			const uint8_t t = (uint8_t)(x * 0xFF);

			return color::blend(t, color::black(), color::green());
		}
	};

#pragma pack(pop)

	static_assert(sizeof(color) == sizeof(uint32_t), "color wrong size");
} // namespace numerus