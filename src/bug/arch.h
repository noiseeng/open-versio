/* Copyright (C) Noise Engineering
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace bug
{
	constexpr inline bool is_arch_x64()
	{
#if defined(__x86_64__) || defined(_M_X64)
		return true;
#else
		return false;
#endif
	}

	constexpr inline bool is_arch_arm64()
	{
#if defined(__aarch64__) || defined(_M_ARM64)
		return true;
#else
		return false;
#endif
	}

	constexpr inline bool is_arch_x86()
	{
#if defined(__i386__) || defined(_M_IX86)
		return true;
#else
		return false;
#endif
	}

	constexpr inline bool is_arch_arm32()
	{
#if defined(__arm__) || defined(_M_ARM)
		return true;
#else
		return false;
#endif
	}

	constexpr inline bool has_fpu()
	{
#if defined(__arm__) && !defined(__ARM_FP)
		return false;
#else
		return true;
#endif
	}

	constexpr inline bool is_bare_metal()
	{
#if defined(__arm__) && !defined(__linux__) && !defined(__APPLE__) && !defined(_WIN32)
		return true;
#else
		return false;
#endif
	}

	constexpr inline const char *get_arch()
	{
		if constexpr(is_arch_x64())
		{
			return "x64";
		}
		else if constexpr(is_arch_arm64())
		{
			return "arm64";
		}
		else if constexpr(is_arch_x86())
		{
			return "x86";
		}
		else if constexpr(is_arch_arm32())
		{
			return "arm";
		}
		else
		{
			return "unknown";
		}
	}

	bool cpu_has_sse41();

} // namespace bug
