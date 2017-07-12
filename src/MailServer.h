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


#ifndef FB_SMTP__MAIL_SERVER
#define FB_SMTP__MAIL_SERVER

#include "Global.h"
#include "CSmtp.h"

namespace FBMailUDF
{
	class MailServer
	{
	private:
		std::string serverName;
		std::string userName;
		std::string password;
		std::string database;
		SMTP_SECURITY_TYPE securityType;
		unsigned short port;
		std::string xMailer;
		FB_BIGINT serverID;
	public:
		MailServer();
		MailServer(const FB_BIGINT serverID, const std::string &serverName, const PortNumber port, 
			SMTP_SECURITY_TYPE securityType, const std::string &userName, const std::string &password, 
			const std::string &dbase, std::string &xMailer);
		MailServer(const FB_BIGINT serverID, const std::string &serverName, const PortNumber port, 
			SMTP_SECURITY_TYPE securityType, const std::string &userName, const std::string &password,
			const std::string &dbase);
		~MailServer();

		bool operator ==(const MailServer& value);

		std::string getServerName();
		PortNumber getPortNumber();
		SMTP_SECURITY_TYPE getSecurityType();
		std::string getUserName();
		std::string getUserPassword();
		std::string getXMailer();
		FB_BIGINT getServerID();
		std::string getDatabase();

		EMailResult isValidServer();
	};

}

#endif
