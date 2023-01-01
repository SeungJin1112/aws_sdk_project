#include "SSM.h"

CloudAws::AwsSSM::AwsSSM(void)
{
}

CloudAws::AwsSSM::AwsSSM(Aws::Vector<Aws::String>& vtInstanceIds,
    Aws::String& strAccessKeyId,
    Aws::String& strSecretAccessKey,
    Aws::String& strRegion,
    Aws::Utils::Logging::LogLevel LogLevel)
    : AwsContext(vtInstanceIds, strAccessKeyId, strSecretAccessKey, strRegion, LogLevel)
{
    m_clientConfig.region = this->GetRegion();
    m_ssm = new Aws::SSM::SSMClient(this->GetCredentials(), this->m_clientConfig);
    m_sendRequest.SetInstanceIds(this->GetInstanceIds());
}

CloudAws::AwsSSM::AwsSSM(CloudAws::AwsContext& context)
{
    m_clientConfig.region = context.GetRegion();
    m_ssm = new Aws::SSM::SSMClient(context.GetCredentials(), m_clientConfig);
    m_sendRequest.SetInstanceIds(context.GetInstanceIds());
}

Aws::String CloudAws::AwsSSM::SendCommands(void)
{
    Aws::String strCommandId;
    auto outcome = m_ssm->SendCommand(m_sendRequest);

    if (outcome.IsSuccess())
    {
        Aws::SSM::Model::SendCommandResult result = outcome.GetResult();
        strCommandId = result.GetCommand().GetCommandId();
    }

    Sleep(5000);
    return strCommandId;
}

Aws::Vector< Aws::Map<Aws::String, Aws::String>> CloudAws::AwsSSM::ListCommandsInvocations(Aws::String& strCommandId)
{
    Aws::Vector< Aws::Map<Aws::String, Aws::String>> vector;

    for (int i = 0; i < this->GetInstanceIds().size(); i++)
    {
        Aws::SSM::Model::ListCommandInvocationsRequest listRequest;
        listRequest.SetCommandId(strCommandId);
        listRequest.SetInstanceId(this->GetInstanceIds()[i]);
        listRequest.SetDetails(true);

        auto outcome = m_ssm->ListCommandInvocations(listRequest);

        if (outcome.IsSuccess())
        {
            auto  result = outcome.GetResult().GetCommandInvocations();
            
            for (int j = 0; j < result.size(); j++)
            {
                for (int k = 0; k < result[j].GetCommandPlugins().size(); k++)
                {
                    Aws::String status = result[j].GetCommandPlugins()[k].GetStatusDetails();

                    if (Aws::Utils::StringUtils::CaselessCompare(status.c_str(), "Success"))
                    {
                        Aws::Map<Aws::String, Aws::String> map;

                        Aws::String strInstanceId = result[j].GetInstanceId();
                        Aws::String strOutput = result[j].GetCommandPlugins()[k].GetOutput();

                        map.insert(std::pair<Aws::String, Aws::String>("InstanceId", strInstanceId));
                        map.insert(std::pair<Aws::String, Aws::String>("CommandId", strCommandId));
                        map.insert(std::pair<Aws::String, Aws::String>("Output", strOutput));
                        map.insert(std::pair<Aws::String, Aws::String>("Status", status));

                        vector.push_back(map);
                    }
                }
            }
        }
    }

    return vector;
}

void CloudAws::AwsSSM::SetCommandsLinux(Aws::Vector<Aws::String>& value)
{
    Aws::String strDocName = "AWS-RunShellScript";

    m_sendRequest.SetDocumentName(strDocName);
    //m_sendRequest.AddParameters("commands", value);
    AddParameters(value);
}

void CloudAws::AwsSSM::SetCommandsWindows(Aws::Vector<Aws::String>& value)
{
    Aws::String strDocName = "AWS-RunPowerShellScript";

    m_sendRequest.SetDocumentName(strDocName);
    m_sendRequest.AddParameters("commands", value);
}

void CloudAws::AwsSSM::AddParameters(Aws::Vector<Aws::String>& value)
{
    Aws::Vector<Aws::String>::iterator it;
    Aws::String strCommand = "echo ";

    strCommand.append(STRING_DELIMITERS);

    for (it = value.begin()+1;; it += 2)
    {
        int nEnd = it == value.end() ? 1 : 0;

        it = value.insert(it, strCommand);

        if (nEnd)
        {
            break;
        }
    }

    m_sendRequest.AddParameters("commands", value);
}

CloudAws::AwsSSM::~AwsSSM(void)
{
    delete m_ssm;
}

void RunSSM(Aws::String& strAccessKeyId,
    Aws::String& strSecretAccessKey,
    Aws::String& strRegion,
    Aws::Utils::Logging::LogLevel LogLevel)
{
    Aws::Vector<Aws::String> vtInstanceIds;
    Aws::Vector<Aws::String> vtCommand;
    Aws::Vector<Aws::Vector<Aws::String>> vtCommands;

    {
        SQLITE sqlite;
        vtInstanceIds = sqlite.SelectInstanceIds();
        vtCommand = sqlite.SelectCommands();
        vtCommands = sqlite.SelectsCommands();
    }

    if (vtCommand.size())
    {
        Aws::String strCommandId;
        Aws::Vector< Aws::Map<Aws::String, Aws::String>> vtCommandResult;

        CloudAws::AwsSSM ssm(vtInstanceIds, strAccessKeyId, strSecretAccessKey, strRegion, LogLevel);
        
        ssm.SetCommandsLinux(vtCommand);
        strCommandId = ssm.SendCommands();

        //EC2 Add later
        vtCommandResult = ssm.ListCommandsInvocations(strCommandId);

        RunSSMProbe(vtCommands, vtCommandResult);
    }
}