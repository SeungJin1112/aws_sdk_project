#include "Util.h"

Aws::Vector<Aws::String> CloudAws::Util::StringSplit(Aws::String& str, Aws::String& strDelimiters)
{
	Aws::Vector<Aws::String> vector;
	Aws::String strBuf;
	Aws::String strTemp = str;
	Aws::String strTempDelimiters;
	char *strContext = NULL;

	if (Aws::Utils::StringUtils::CaselessCompare(strDelimiters.c_str(), "\\n")) {
		strTempDelimiters = "\n";
	}else if (Aws::Utils::StringUtils::CaselessCompare(strDelimiters.c_str(), "\\t")){
		strTempDelimiters = "\t";
	}else {
		strTempDelimiters = strDelimiters;
	}

	strBuf = strtok_s((char*)strTemp.c_str(), strTempDelimiters.c_str(), &strContext);

	while (!strBuf.empty())
	{
		vector.push_back(strBuf);

		if (!Aws::Utils::StringUtils::CaselessCompare(strContext, ""))
		{
			strBuf = strtok_s(NULL, strTempDelimiters.c_str(), &strContext);
		}
		else
		{
			break;
		}
	}

	return vector;
}

Aws::Vector<Aws::String> CloudAws::Util::StringSeparation(Aws::String& str, Aws::String& strDelimiters)
{
	Aws::Vector<Aws::String> vector;
	Aws::String token;
	Aws::String strTemp = str;
	size_t pos = 0;

	while ((pos = strTemp.find(strDelimiters)) != std::string::npos) {
		vector.push_back(strTemp.substr(0, pos));
		strTemp.erase(0, pos + strDelimiters.length());
	}

	return vector;
}