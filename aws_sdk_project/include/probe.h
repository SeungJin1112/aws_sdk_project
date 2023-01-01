#pragma once
#ifndef PROBE_H
#define PROBE_H

#include "CloudAws.h"
#include "Util.h"
#include "sqlite.h"
#include "stdafx.h"

#define FUNC_MAX 256
#define STRING_DELIMITERS "!@#$%" //vertical tab

enum PROBE_ARGS
{
	PROBE_ARGS_INSTANCE_ID = POLICY_LIST_COLUMN_LAST,
	PROBE_ARGS_COMMAND_RESULT
};

enum PROBE_KEY_VALUE
{
	PROBE_KEY = 0,
	PROBE_VALUE
};

namespace CloudAws
{
	class AwsProbe
	{
	/* function */
	public:
		AwsProbe(void);
		~AwsProbe(void);

		void AddCommands(Aws::Vector<Aws::Vector<Aws::String>>& vector);
		Aws::Map<Aws::String, Aws::String> GetArgs();

		void ExecuteSSMProbes(void);
		void SetSSMFuncList(void);
		void SetSSMProbeList(void);

		void CheckSSM(void);
		Aws::String CheckSSMResultExist(void);
		Aws::String CheckSSMResultFindValue(int nValueExists,
			int nField,
			int nOutputLine,
			Aws::String& strLine,
			Aws::Vector<Aws::String>& vtField,
			Aws::Map<Aws::String, Aws::String>& mpArgs,
			int& nEnd);
		Aws::String CheckSSMSizeCompare(int nValueExists,
			int nField,
			int nOutputLine,
			Aws::String& strLine,
			Aws::Vector<Aws::String>& vtField,
			Aws::Map<Aws::String, Aws::String>& mpArgs,
			int& nEnd);

		Aws::String CheckSSMResult(void);

	/* variable */
	private:
		Aws::Vector<Aws::String> m_vtCommand;
		Aws::Vector<Aws::Vector<Aws::String>> m_vtCommands;
		std::function<Aws::String(CloudAws::AwsProbe&)> m_func[FUNC_MAX];
		std::function<Aws::String(CloudAws::AwsProbe&, int, int, int, Aws::String&, Aws::Vector<Aws::String>&, Aws::Map<Aws::String, Aws::String>&, int&)> m_probeFunc[FUNC_MAX];
	};
}

void RunSSMProbe(Aws::Vector<Aws::Vector<Aws::String>>& vtCommands,
	Aws::Vector< Aws::Map<Aws::String, Aws::String>>& vtCommandResult);

#endif /* PROBE_H */