#pragma once
#ifndef SSM_H
#define SSM_H

#include "CloudAws.h"
#include <aws/ssm/SSMClient.h>
#include <aws/ssm/model/SendCommandRequest.h>
#include <aws/ssm/model/SendCommandResult.h>
#include <aws/ssm/model/ListCommandInvocationsRequest.h>
#include <aws/ssm/model/ListCommandInvocationsResult.h>
#include "sqlite.h"
#include "probe.h"
#include "stdafx.h"

namespace CloudAws
{
	class AwsSSM : AwsContext
	{
    /* function */
    public:
        AwsSSM(void);
        AwsSSM(Aws::Vector<Aws::String>& vtInstanceIds,
            Aws::String& strAccessKeyId,
            Aws::String& strSecretAccessKey,
            Aws::String& strRegion,
            Aws::Utils::Logging::LogLevel LogLevel);
        AwsSSM(CloudAws::AwsContext& context);
        ~AwsSSM(void);

        void SetCommandsLinux(Aws::Vector<Aws::String>& value);
        void SetCommandsWindows(Aws::Vector<Aws::String>& value);
        void AddParameters(Aws::Vector<Aws::String>& value);
        Aws::String SendCommands(void);
        Aws::Vector< Aws::Map<Aws::String, Aws::String>> ListCommandsInvocations(Aws::String& strCommandId);

    /* variable */
    private:
        Aws::Client::ClientConfiguration m_clientConfig;
        Aws::SSM::SSMClient *m_ssm;
        Aws::SSM::Model::SendCommandRequest m_sendRequest;
	};
}

void RunSSM(Aws::String& strAccessKeyId,
    Aws::String& strSecretAccessKey,
    Aws::String& strRegion,
    Aws::Utils::Logging::LogLevel LogLevel);


#endif /* SSM_H */
