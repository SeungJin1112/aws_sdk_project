#pragma once
#ifndef CLOUD_AWS_H
#define CLOUD_AWS_H

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>

namespace CloudAws
{
	class AwsContext
	{
	/* function */
	public:
		AwsContext(void);
		AwsContext(Aws::Vector<Aws::String>& vtInstanceIds,
			Aws::String& strAccessKeyId,
			Aws::String& strSecretAccessKey,
			Aws::String& strRegion,
			Aws::Utils::Logging::LogLevel LogLevel);
		~AwsContext(void);

		Aws::Vector<Aws::String> GetInstanceIds(void);
		Aws::String GetAccessKeyId(void);
		Aws::String GetSecretAccessKey(void);
		Aws::String GetRegion(void);
		Aws::Auth::AWSCredentials GetCredentials(void);
		
	protected:
	private:
		void Initialize(void);

	/* variable */
	public:
	protected:
		Aws::Vector<Aws::String> m_vtInstanceIds;
		Aws::String m_strAccessKeyId;
		Aws::String m_strSecretAccessKey;
		Aws::String m_strRegion;
		Aws::Utils::Logging::LogLevel m_logLevel;
		Aws::Auth::AWSCredentials m_credentials;
	private:
		Aws::SDKOptions m_options;
	};
}

#endif /* CLOUD_AWS_H */

Aws::Utils::Logging::LogLevel SetLogLevel(Aws::String strLogLevel);