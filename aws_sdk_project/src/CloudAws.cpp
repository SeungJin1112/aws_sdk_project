#include "CloudAws.h"

CloudAws::AwsContext::AwsContext(void)
	: m_strAccessKeyId(""), 
	m_strSecretAccessKey(""), 
	m_strRegion(""), 
	m_logLevel(Aws::Utils::Logging::LogLevel::Off)
{
	AwsContext::Initialize();
}

CloudAws::AwsContext::AwsContext( Aws::Vector<Aws::String>& vtInstanceIds,
	Aws::String& strAccessKeyId,
	Aws::String& strSecretAccessKey,
	Aws::String& strRegion,
	Aws::Utils::Logging::LogLevel logLevel)
	: m_vtInstanceIds(vtInstanceIds),
	m_strAccessKeyId(strAccessKeyId),
	m_strSecretAccessKey(strSecretAccessKey),
	m_strRegion(strRegion),
	m_logLevel(logLevel)
{
	AwsContext::Initialize();
}

CloudAws::AwsContext::~AwsContext(void)
{
	Aws::ShutdownAPI(m_options);
}

void CloudAws::AwsContext::Initialize(void)
{
	m_options.loggingOptions.logLevel = m_logLevel;

	Aws::InitAPI(m_options);

	if (!Aws::Utils::StringUtils::CaselessCompare(m_strAccessKeyId.c_str(), ""))
	{
		m_credentials.SetAWSAccessKeyId(m_strAccessKeyId);
	}

	if (!Aws::Utils::StringUtils::CaselessCompare(m_strSecretAccessKey.c_str(), ""))
	{
		m_credentials.SetAWSSecretKey(m_strSecretAccessKey);
	}
}

Aws::Utils::Logging::LogLevel SetLogLevel(Aws::String strLogLevel)
{
	Aws::Utils::Logging::LogLevel logLevel = Aws::Utils::Logging::LogLevel::Off;

	if (Aws::Utils::StringUtils::CaselessCompare(strLogLevel.c_str(), "Off")){
		logLevel = Aws::Utils::Logging::LogLevel::Off;
	}else if (Aws::Utils::StringUtils::CaselessCompare(strLogLevel.c_str(), "Fatal")){
		logLevel = Aws::Utils::Logging::LogLevel::Fatal;
	}else if (Aws::Utils::StringUtils::CaselessCompare(strLogLevel.c_str(), "Error")){
		logLevel = Aws::Utils::Logging::LogLevel::Error;
	}else if (Aws::Utils::StringUtils::CaselessCompare(strLogLevel.c_str(), "Warn")){
		logLevel = Aws::Utils::Logging::LogLevel::Warn;
	}else if (Aws::Utils::StringUtils::CaselessCompare(strLogLevel.c_str(), "Info")){
		logLevel = Aws::Utils::Logging::LogLevel::Info;
	}else if (Aws::Utils::StringUtils::CaselessCompare(strLogLevel.c_str(), "Debug")){
		logLevel = Aws::Utils::Logging::LogLevel::Debug;
	}else if (Aws::Utils::StringUtils::CaselessCompare(strLogLevel.c_str(), "Trace")){
		logLevel = Aws::Utils::Logging::LogLevel::Trace;
	}

	return logLevel;
}

Aws::Vector<Aws::String> CloudAws::AwsContext::GetInstanceIds(void)
{
	return m_vtInstanceIds;
}

Aws::String CloudAws::AwsContext::GetAccessKeyId(void)
{
	return m_strAccessKeyId;
}

Aws::String CloudAws::AwsContext::GetSecretAccessKey(void)
{
	return m_strSecretAccessKey;
}

Aws::String CloudAws::AwsContext::GetRegion(void)
{
	return m_strRegion;
}

Aws::Auth::AWSCredentials CloudAws::AwsContext::GetCredentials(void)
{
	return m_credentials;
}