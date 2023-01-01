#include "CloudAws.h"
#include "SSM.h"
#include "stdafx.h"

void help(void);
void version(void);

/*
* <sample instance>
* instance id: i-04498294f0ec08400
* access key id: AKIASSMMN6GKL3NW53A6
* secret access key: q7ExPsBUhLVObTsydaNlTfRADygv8WoIEjsXqcJJ
* region: ap-northeast-2
*/

int main(int argc, char** argv)
{
	Aws::String strAccessKeyId = "";
	Aws::String strSecretAccessKey = "";
	Aws::String strRegion = "";
	Aws::Utils::Logging::LogLevel LogLevel = Aws::Utils::Logging::LogLevel::Off;

	if(argc < 2)
	{
		help();
		return 0;
	}

	for(int i = 0; i < argc; i++)
	{
		if(Aws::Utils::StringUtils::CaselessCompare(argv[i], "-a") && (i+1) < argc){
			strAccessKeyId = argv[i+1];
		}else if(Aws::Utils::StringUtils::CaselessCompare(argv[i], "-s") && (i+1) < argc){
			strSecretAccessKey = argv[i+1];
		}else if(Aws::Utils::StringUtils::CaselessCompare(argv[i], "-r") && (i+1) < argc){
			strRegion = argv[i+1];
		}else if(Aws::Utils::StringUtils::CaselessCompare(argv[i], "-l") && (i+1) < argc){
			LogLevel = SetLogLevel(argv[i+1]);
		}

		if(Aws::Utils::StringUtils::CaselessCompare(argv[i], "-h")){
			help();
			return 0;	
		}else if(Aws::Utils::StringUtils::CaselessCompare(argv[i], "-v")){
			version();
			return 0;			
		}
	}

	RunSSM(strAccessKeyId, strSecretAccessKey, strRegion, LogLevel);
}

void help(void)
{
	std::cout << "./aws_sdk_project.exe [-a] [-s] [-r] [-l] [-h] [-v] \n" << std::endl;

	std::cout << " <instruction> \n" <<
		" * -a [Access Key Id]: (Access key id) in (Security Credentials) of a user with (AdministratorAccess) privilege \n" <<
		" * -s [Secret Access Key]: (Secret Access Key) in (Security Credentials) of a user with (AdministratorAccess) privilege \n" <<
		" * -r [Region]: The region that created the instance. Check at the top right of the aws (https://ap-northeast-2.console.aws.amazon.com/) URL \n" <<
		" * -l [LogLevel]: Rating of aws sdk log \n" <<
		""<< std::endl;

	std::cout << " <example> \n" <<
		"./aws_sdk_project.exe -a AKIASSMMN6GKL3NW53A6 -s q7ExPsBUhLVObTsydaNlTfRADygv8WoIEjsXqcJJ -r ap-northeast-2 -l Info \n" << 
		" * -a [Access Key Id]:  AKIASSMMN6GKL3NW53A6 \n" <<
		" * -s [Secret Access Key]: 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 \n" <<
		" * -r [Region]: ap-northeast-2 \n" <<
		" * -l [LogLevel]: Off, Fatal, Error, Warn, Info, Debug, Trace \n" << std::endl;
}

void version(void)
{
	std::cout << "aws_sdk_project version: 1.0.0 \n" << std::endl;
}