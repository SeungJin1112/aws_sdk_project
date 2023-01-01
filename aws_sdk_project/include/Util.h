#pragma once
#ifndef UTIL_H
#define UTIL_H

#define _CRT_SECURE_NO_WARNINGS

#include "CloudAws.h"

namespace CloudAws
{
	class Util
	{
	/* function */
	public:
		Aws::Vector<Aws::String> StringSeparation(Aws::String& str, Aws::String& strDelimiters);
		Aws::Vector<Aws::String> StringSplit(Aws::String& str, Aws::String& strDelimiters);
	};
}

#endif /* UTIL_H */