/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

/*
 A string formater similar to printf that compiles much smaller on arm thumb than any other option i have found.
 This is intentionally not compatible in corner cases for space and consistency.
 */

#pragma once

#include "arch.h"
#include "compiler.h"

#include <stdarg.h>
#include <stdint.h>
#include <cstddef>
#include <type_traits>

namespace bug
{
	using putc_t = void (*)(char c, void* ctx);

	template <bool FLOAT, bool INT64>
	struct format_t
	{
		struct buf_ctx
		{
			char*		buf;
			std::size_t size;
			std::size_t pos;
		};

		using int_t	 = std::conditional_t<INT64, int64_t, int32_t>;
		using uint_t = std::conditional_t<INT64, uint64_t, uint32_t>;

		static constexpr char digits[] = "0123456789abcdef";

		template <typename T>
		static constexpr char* utoa_rev(T val, char* buf, uint32_t base)
		{
			char* p = buf;

			do
			{
				*p++ = digits[val % base];
				val /= base;
			} while(val);

			return p;
		}

		template <typename T>
		static constexpr char* itoa_rev(T val, char* buf, uint32_t base, bool& is_neg)
		{
			using U = std::make_unsigned_t<T>;
			is_neg	= val < 0 && base == 10;
			return utoa_rev(is_neg ? (U)0 - (U)val : (U)val, buf, base);
		}

		static int_t int_arg(va_list& ap, bool long_long)
		{
			if(long_long)
			{
				return (int_t)va_arg(ap, int64_t);
			}
			return va_arg(ap, int32_t);
		}

		static uint_t uint_arg(va_list& ap, bool long_long)
		{
			if(long_long)
			{
				return (uint_t)va_arg(ap, uint64_t);
			}
			return va_arg(ap, uint32_t);
		}

		using literal_float_t = decltype(1.0);

		static constexpr bool literal_float_is_wide = sizeof(literal_float_t) > sizeof(float);

		static constexpr int float_precision_max = literal_float_is_wide ? 17 : 15;

		static constexpr bool ftoa_int_is_wide = INT64 || sizeof(void*) > sizeof(uint32_t);

		using ftoa_int_t = std::conditional_t<ftoa_int_is_wide, uint64_t, uint32_t>;

		static constexpr literal_float_t ftoa_int_limit
			= ftoa_int_is_wide ? 18446744073709551616.0 : 4294967296.0;

		static constexpr bool float_is_exp_max(float val, bool& is_nan)
		{
			const uint32_t bits = bug::bit_cast<uint32_t>(val);
			const uint32_t exp	= (bits >> 23) & 0xffu;
			is_nan				= exp == 0xffu && (bits & 0x7fffffu) != 0;
			return exp == 0xffu;
		}

		static constexpr bool float_is_exp_max(double val, bool& is_nan)
		{
			const uint64_t bits = bug::bit_cast<uint64_t>(val);
			const uint64_t exp	= (bits >> 52) & 0x7ffu;
			is_nan				= exp == 0x7ffu && (bits & 0xfffffffffffffull) != 0;
			return exp == 0x7ffu;
		}

		static constexpr char* ftoa_rev(literal_float_t val, char* buf, int precision)
		{
			if(precision > float_precision_max)
			{
				precision = float_precision_max;
			}
			char* p = buf;

			bool	   is_nan	  = false;
			const bool is_exp_max = float_is_exp_max(val, is_nan);
			if(is_nan)
			{
				*p++ = 'n';
				*p++ = 'a';
				*p++ = 'n';
				return p;
			}

			bool is_neg = false;
			if(val < 0.0)
			{
				is_neg = true;
				val	   = -val;
			}

			if(is_exp_max)
			{
				*p++ = 'f';
				*p++ = 'n';
				*p++ = 'i';
				if(is_neg)
				{
					*p++ = '-';
				}
				return p;
			}

			literal_float_t rounding = 0.5;
			for(int i = 0; i < precision; i++)
			{
				rounding /= 10.0;
			}
			val += rounding;

			if(!(val < ftoa_int_limit))
			{
				*p++ = 'f';
				*p++ = 'v';
				*p++ = 'o';
				if(is_neg)
				{
					*p++ = '-';
				}
				return p;
			}

			ftoa_int_t		int_part  = (ftoa_int_t)val;
			literal_float_t frac_part = val - (literal_float_t)int_part;

			char frac_digits[float_precision_max] = {};
			for(int i = 0; i < precision; i++)
			{
				frac_part *= 10.0;
				int digit	   = (int)frac_part;
				frac_digits[i] = (char)('0' + digit);
				frac_part -= (literal_float_t)digit;
			}

			for(int i = precision - 1; i >= 0; i--)
			{
				*p++ = frac_digits[i];
			}

			if(precision > 0)
			{
				*p++ = '.';
			}

			p = utoa_rev(int_part, p, 10);

			if(is_neg)
			{
				*p++ = '-';
			}

			return p;
		}

		static constexpr void write_rev(const char* start, const char* end, putc_t putc, void* ctx)
		{
			while(end > start)
			{
				putc(*--end, ctx);
			}
		}

		static constexpr void pad(int count, putc_t putc, void* ctx)
		{
			for(int i = 0; i < count; i++)
			{
				putc(' ', ctx);
			}
		}

		static constexpr void
		padded(const char* s, int len, int width, bool left_align, putc_t putc, void* ctx)
		{
			if(!left_align)
			{
				pad(width - len, putc, ctx);
			}
			for(int i = 0; i < len; i++)
			{
				putc(s[i], ctx);
			}
			if(left_align)
			{
				pad(width - len, putc, ctx);
			}
		}

		static void arg(putc_t	 putc,
						void*	 ctx,
						char	 spec,
						int		 width,
						int		 precision,
						bool	 zero_pad,
						bool	 left_align,
						bool	 long_long,
						va_list& ap)
		{
			char  buf[48];
			char* end;
			bool  is_neg = false;

			switch(spec)
			{
				case 'd': [[fallthrough]];
				case 'i': {
					end = itoa_rev(int_arg(ap, long_long), buf, 10, is_neg);
					break;
				}
				case 'u': {
					end = utoa_rev(uint_arg(ap, long_long), buf, 10);
					break;
				}
				case 'x': {
					end = utoa_rev(uint_arg(ap, long_long), buf, 16);
					break;
				}
				case 'p': {
					putc('0', ctx);
					putc('x', ctx);
					width	 = sizeof(void*) * 2;
					zero_pad = true;
					end		 = utoa_rev((uintptr_t)va_arg(ap, void*), buf, 16);
					break;
				}
				case 'c': {
					char c = (char)va_arg(ap, int);
					padded(&c, 1, width, left_align, putc, ctx);
					return;
				}
				case 's': {
					const char* s = va_arg(ap, const char*);
					if(!s)
					{
						s = "(null)";
					}
					int len = 0;
					while(s[len])
					{
						len++;
					}
					padded(s, len, width, left_align, putc, ctx);
					return;
				}
				case 'f': {
					if constexpr(FLOAT)
					{
						literal_float_t val = (literal_float_t)va_arg(ap, double);
						if(precision < 0)
						{
							precision = 6;
						}
						end = ftoa_rev(val, buf, precision);
						break;
					}
					else
					{
						(void)va_arg(ap, double);
						putc('%', ctx);
						putc('f', ctx);
						return;
					}
				}
				case '%': {
					putc('%', ctx);
					return;
				}
				default: {
					putc('%', ctx);
					putc(spec, ctx);
					return;
				}
			}

			int len = (int)(end - buf) + (is_neg ? 1 : 0);

			if(left_align)
			{
				zero_pad = false;
			}
			else if(!zero_pad)
			{
				pad(width - len, putc, ctx);
			}

			if(is_neg)
			{
				putc('-', ctx);
			}

			if(zero_pad)
			{
				for(int i = len; i < width; i++)
				{
					putc('0', ctx);
				}
			}

			write_rev(buf, end, putc, ctx);

			if(left_align)
			{
				pad(width - len, putc, ctx);
			}
		}

		static void core(putc_t putc, void* ctx, const char* fmt, va_list& ap)
		{
			while(*fmt)
			{
				if(*fmt != '%')
				{
					putc(*fmt++, ctx);
					continue;
				}

				fmt++;

				bool zero_pad	= false;
				bool left_align = false;
				while(*fmt == '0' || *fmt == '-')
				{
					zero_pad   = zero_pad || *fmt == '0';
					left_align = left_align || *fmt == '-';
					fmt++;
				}

				int width = 0;
				while(*fmt >= '0' && *fmt <= '9')
				{
					width = width * 10 + (*fmt++ - '0');
				}

				int precision = -1;
				if(*fmt == '.')
				{
					fmt++;
					precision = 0;
					while(*fmt >= '0' && *fmt <= '9')
					{
						precision = precision * 10 + (*fmt++ - '0');
					}
				}

				bool long_long = false;
				if(*fmt == 'z')
				{
					fmt++;
					long_long = sizeof(std::size_t) > sizeof(uint32_t);
				}
				else if(*fmt == 'l')
				{
					fmt++;
					if(*fmt == 'l')
					{
						fmt++;
						long_long = true;
					}
					else
					{
						long_long = sizeof(long) > sizeof(uint32_t);
					}
				}
				else if(*fmt == 'h')
				{
					fmt++;
					if(*fmt == 'h')
					{
						fmt++;
					}
				}

				if(*fmt == '\0')
				{
					break;
				}

				char spec = *fmt++;

				arg(putc, ctx, spec, width, precision, zero_pad, left_align, long_long, ap);
			}
		}

		static int to(putc_t putc, void* ctx, const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			core(putc, ctx, fmt, ap);
			va_end(ap);
			return 0;
		}

		static int va(putc_t putc, void* ctx, const char* fmt, va_list& ap)
		{
			core(putc, ctx, fmt, ap);
			return 0;
		}

		static void buf_putc(char c, void* ctx_ptr)
		{
			buf_ctx* ctx = (buf_ctx*)ctx_ptr;
			if(ctx->pos < ctx->size - 1)
			{
				ctx->buf[ctx->pos++] = c;
			}
		}

		static int buf_va(char* buf, std::size_t size, const char* fmt, va_list& ap)
		{
			if(!buf || size == 0)
			{
				return 0;
			}

			buf_ctx ctx = {buf, size, 0};
			core(buf_putc, &ctx, fmt, ap);
			buf[ctx.pos] = '\0';
			return (int)ctx.pos;
		}

		static int buf(char* buf, std::size_t size, const char* fmt, ...)
		{
			va_list ap;
			va_start(ap, fmt);
			const int n = buf_va(buf, size, fmt, ap);
			va_end(ap);
			return n;
		}
	};

	using format = format_t<has_fpu(), !is_bare_metal()>;

} // namespace bug
