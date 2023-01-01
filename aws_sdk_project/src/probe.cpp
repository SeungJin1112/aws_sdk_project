#include "probe.h"

void CloudAws::AwsProbe::SetSSMFuncList(void)
{
	this->m_func[0] = &CloudAws::AwsProbe::CheckSSMResultExist;
	this->m_func[1] = &CloudAws::AwsProbe::CheckSSMResult;
	this->m_func[2] = &CloudAws::AwsProbe::CheckSSMResult;

	this->SetSSMProbeList();
}

void CloudAws::AwsProbe::SetSSMProbeList(void)
{
	this->m_probeFunc[1] = &CloudAws::AwsProbe::CheckSSMResultFindValue;
	this->m_probeFunc[2] = &CloudAws::AwsProbe::CheckSSMSizeCompare;
}

CloudAws::AwsProbe::AwsProbe(void)
{

}

CloudAws::AwsProbe::~AwsProbe(void)
{

}

void CloudAws::AwsProbe::AddCommands(Aws::Vector<Aws::Vector<Aws::String>>& vector)
{
	m_vtCommands = vector;
}

//Multi-Threads to be added
void CloudAws::AwsProbe::ExecuteSSMProbes(void)
{
	this->SetSSMFuncList();

	for (int i=0; i < this->m_vtCommands.size(); i++)
	{
		this->m_vtCommand = this->m_vtCommands[i];
		CheckSSM();
		this->m_vtCommand.clear();
	}
}

void CloudAws::AwsProbe::CheckSSM(void)
{
	SQLITE sqlite;
	Aws::String sql;
	Aws::Vector<Aws::String> vtInstanceIdSeq = sqlite.SelectInstanceIdSeq(m_vtCommand[PROBE_ARGS_INSTANCE_ID].c_str());
	int nProbeSeq = atoi(m_vtCommand[POLICY_LIST_COLUMN_PROBE_SEQ].c_str());

	if (Aws::Utils::StringUtils::CaselessCompare(vtInstanceIdSeq[0].c_str(), ""))
	{
		return;
	}

	sql = "INSERT INTO RESULT(INSTANCE_ID_SEQ, POLICY_SEQ, COMMAND_RESULT, INSPECTION_RESULT) VALUES( ";
	sql.append(vtInstanceIdSeq[0] + ", ");
	sql.append(m_vtCommand[POLICY_LIST_COLUMN_SEQ] + ", ");

	{
		Aws::String str = m_vtCommand[PROBE_ARGS_COMMAND_RESULT].substr(0, 1);

		if (Aws::Utils::StringUtils::CaselessCompare(str.c_str(), "\n"))
		{
			str.clear();
			str = m_vtCommand[PROBE_ARGS_COMMAND_RESULT].substr(1, m_vtCommand[PROBE_ARGS_COMMAND_RESULT].size());
			sql.append("'" + str + "', ");
		}
		else
		{
			sql.append("'" + m_vtCommand[PROBE_ARGS_COMMAND_RESULT] + "', ");
		}
	}

	Aws::String strInspectionResult = this->m_func[nProbeSeq - 1](*this);

	if (Aws::Utils::StringUtils::CaselessCompare(strInspectionResult.c_str(), ""))
	{
		return;
	}

	sql.append(strInspectionResult + " )");
	sqlite.InsertResult(sql);

	return;
}

Aws::String CloudAws::AwsProbe::CheckSSMResult(void)
{
	Aws::String strRet;
	int nValueExists = -1;
	int nProbeSeq = atoi(m_vtCommand[POLICY_LIST_COLUMN_PROBE_SEQ].c_str());

	nValueExists = atoi(m_vtCommand[POLICY_LIST_COLUMN_VALUE_EXISTS].c_str());

	if (Aws::Utils::StringUtils::CaselessCompare(m_vtCommand[PROBE_ARGS_COMMAND_RESULT].c_str(), ""))
	{
		strRet = "'Unknown'";
	}
	else
	{
		CloudAws::Util util;
		Aws::String strDelimiters;
		Aws::Vector<Aws::String> vtArgs;
		Aws::Vector<Aws::String> vtOutput;

		strDelimiters = ",";
		vtArgs = util.StringSplit(m_vtCommand[POLICY_LIST_COLUMN_ARGS], strDelimiters);
		strDelimiters = "\n";
		vtOutput = util.StringSplit(m_vtCommand[PROBE_ARGS_COMMAND_RESULT], strDelimiters);

		for (int i = 0; i < vtOutput.size(); i++)
		{
			Aws::Map<Aws::String, Aws::String> mpArgs;
			Aws::Vector<Aws::String> vtField;
			Aws::String strLine;
			int nField = -1;
			int nEnd = 0;

			for (int j = 0; j < vtArgs.size(); j++)
			{
				Aws::Vector<Aws::String> vector;
				strDelimiters = "=";
				vector = util.StringSplit(vtArgs[j], strDelimiters);
				mpArgs.insert(std::pair<Aws::String, Aws::String>(vector[PROBE_KEY], vector[PROBE_VALUE]));
			}

			vtField = util.StringSplit(vtOutput[i], mpArgs.find("delimiters")->second);
			nField = atoi(mpArgs.find("field")->second.c_str());
			strLine = mpArgs.find("line")->second.c_str();

			if (nField == -1)
			{
				strRet = "'Unknown'";
				goto END;
			}
			
			strRet = this->m_probeFunc[nProbeSeq - 1](*this, nValueExists, nField, i, strLine, vtField, mpArgs, nEnd);

			if (nEnd == 1)
			{
				goto END;
			}
		}
	}

END:
	return strRet;
}

Aws::String CloudAws::AwsProbe::CheckSSMResultExist(void)
{
	Aws::String strRet;
	int nValueExists = -1;

	nValueExists = atoi(m_vtCommand[POLICY_LIST_COLUMN_VALUE_EXISTS].c_str());

	if (Aws::Utils::StringUtils::CaselessCompare(m_vtCommand[PROBE_ARGS_COMMAND_RESULT].c_str(), ""))
	{
		if (nValueExists == 0) {
			strRet = "'Success'";
		}else if (nValueExists == 1) {
			strRet = "'Failed'";
		}else {
			strRet = "'Unknown'";
		}
	}
	else
	{
		if (nValueExists == 0) {
			strRet = "'Failed'";
		}else if (nValueExists == 1) {
			strRet = "'Success'";
		}else {
			strRet = "'Unknown'";
		}
	}

	return strRet;
}

Aws::String CloudAws::AwsProbe::CheckSSMResultFindValue(int nValueExists,
	int nField, 
	int nOutputLine,
	Aws::String& strLine,
	Aws::Vector<Aws::String>& vtField,
	Aws::Map<Aws::String, Aws::String>& mpArgs,
	int& nEnd)
{
	Aws::String strRet;
	int nFind = 0;

	if (Aws::Utils::StringUtils::CaselessCompare(vtField[nField - 1].c_str(),
		mpArgs.find("value")->second.c_str()))
	{
		if (Aws::Utils::StringUtils::CaselessCompare(strLine.c_str(), "all") || (atoi(strLine.c_str()) - 1) == nOutputLine)
		{
			nFind = 1;

			if (nValueExists == 0) {
				strRet = "'Failed'";
				nEnd = 1;
				goto END;
			}else if (nValueExists == 1) {
				strRet = "'Success'";
			}else {
				strRet = "'Unknown'";
				nEnd = 1;
				goto END;
			}
		}
	}

	if (nFind == 0)
	{
		if (nValueExists == 1) {
			strRet = "'Failed'";
			nEnd = 1;
		}else if (nValueExists == 0) {
			strRet = "'Success'";
		}else {
			strRet = "'Unknown'";
			nEnd = 1;
		}
	}

END:
	return strRet;
}

Aws::String CloudAws::AwsProbe::CheckSSMSizeCompare(int nValueExists,
	int nField,
	int nOutputLine,
	Aws::String& strLine,
	Aws::Vector<Aws::String>& vtField,
	Aws::Map<Aws::String, Aws::String>& mpArgs,
	int& nEnd)
{
	Aws::String strRet;

	if (Aws::Utils::StringUtils::CaselessCompare(strLine.c_str(), "all") || (atoi(strLine.c_str()) - 1) == nOutputLine)
	{
		if (mpArgs.find("max") != mpArgs.end() && mpArgs.find("min") != mpArgs.end())
		{
			//Add it later
		}
		else if (mpArgs.find("max") != mpArgs.end())
		{
			if (atoi(mpArgs.find("max")->second.c_str()) < atoi(vtField[nField - 1].c_str())) {
				strRet = "'Failed'";
				nEnd = 1;
				goto END;
			}else {
				strRet = "'Success'";
			}
		}
		else if (mpArgs.find("min") != mpArgs.end())
		{
			if (atoi(mpArgs.find("min")->second.c_str()) > atoi(vtField[nField - 1].c_str())) {
				strRet = "'Failed'";
				nEnd = 1;
				goto END;
			}else {
				strRet = "'Success'";
			}
		}
		else
		{
			strRet = "'Unknown'";
			nEnd = 1;
			goto END;
		}
	}

END:
	return strRet;
}

//-Multi-Threads to be added

void RunSSMProbe(Aws::Vector<Aws::Vector<Aws::String>>& vtCommands,
	Aws::Vector<Aws::Map<Aws::String, Aws::String>>& vtCommandResult)
{
	CloudAws::AwsProbe probe;
	Aws::Vector<Aws::Map<Aws::String, Aws::String>>::iterator it;

	for (it = vtCommandResult.begin(); it != vtCommandResult.end(); it++)
	{
		Aws::Vector<Aws::Vector<Aws::String>> vector = vtCommands;

		Aws::String strOutput = it->find("Output")->second;
		Aws::String strDelimiters = STRING_DELIMITERS;
		Aws::Vector<Aws::String> vtDelimiters;

		{
			CloudAws::Util util;
			vtDelimiters = util.StringSeparation(strOutput, strDelimiters);
		}

		for(int i = 0; i < vtDelimiters.size(); i++)
		{
			vector[i].push_back(it->find("InstanceId")->second);
			vector[i].push_back(vtDelimiters[i]);
		}

		probe.AddCommands(vector);
		vector.clear();
	}
	probe.ExecuteSSMProbes();
}