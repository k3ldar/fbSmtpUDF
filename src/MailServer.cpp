/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* MIT License
*
* Copyright (c) 2017 Simon Carter (s1cart3r@gmail.com)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* The Original Code was created by Simon Carter (s1cart3r@gmail.com).
*
* Module:  fbSMTPUDF - Firebird UDF Library for sending emails.
*
* Description: Class holding mail server connection details
*
* Date: 12/07/2017
*
*/


#include "MailServer.h"

namespace FBMailUDF
{
	MailServer::MailServer()
	{

	}

	MailServer::MailServer(const FB_BIGINT serverID, const std::string &serverName, const PortNumber port,
		SMTP_SECURITY_TYPE securityType, const std::string &userName, const std::string &password,
		const std::string &dbase, std::string &xMailer)
		: MailServer(serverID, serverName, port, securityType, userName, password, dbase)
	{
		this->xMailer = xMailer;
	}

	MailServer::MailServer(const FB_BIGINT serverID, const std::string &serverName, const PortNumber port, 
		SMTP_SECURITY_TYPE securityType, const std::string &userName, const std::string &password,
		const std::string &dbase)
	{
		this->serverName = serverName;
		this->port = port;
		this->securityType = securityType;
		this->userName = userName;
		this->password = password;
		this->database = dbase;
		this->xMailer = "Firebird SMTP UDF (v1.x)";

		this->serverID = static_cast<FB_BIGINT>(serverID);
	}

	MailServer::~MailServer()
	{

	}

	bool MailServer::operator==(const MailServer &value)
	{
		return (value.port == port && value.securityType == securityType &&
			value.serverName.compare(serverName) == 0 &&
			value.userName.compare(userName) == 0 &&
			value.password.compare(password) == 0);
	}

	std::string MailServer::getServerName()
	{
		return (serverName);
	}

	PortNumber MailServer::getPortNumber()
	{
		return (port);
	}

	SMTP_SECURITY_TYPE MailServer::getSecurityType()
	{
		return (securityType);
	}

	std::string MailServer::getUserName()
	{
		return (userName);
	}

	std::string MailServer::getUserPassword()
	{
		return (password);
	}

	std::string MailServer::getXMailer()
	{
		return (xMailer);
	}

	FB_BIGINT MailServer::getServerID()
	{
		return (serverID);
	}

	std::string MailServer::getDatabase()
	{
		return (database);
	}

	EMailResult MailServer::isValidServer()
	{
		if (database.empty())
			return (EMailResult::InvalidDatabaseName);

		if (serverName.empty() || serverName.size() > MAX_SERVER_STRING_LENGHT)
			return (EMailResult::InvalidServerName);

		if (userName.empty() || userName.size() > MAX_SERVER_STRING_LENGHT)
			return (EMailResult::InvalidServerUser);

		if (password.empty() || password.size() > MAX_SERVER_STRING_LENGHT)
			return (EMailResult::InvalidServerPassword);

		if (port < 1 || port > 65535)
			return (EMailResult::InvalidServerPort);

		return (EMailResult::Success);
	}
}
