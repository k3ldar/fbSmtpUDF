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
* Description: External UDF API Functions
*
* Date: 12/07/2017
*
*/
#include "fbSmtpUDF.h"

#ifndef __FB_MESSAGE_SERVER
#define __FB_MESSAGE_SERVER
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

BOOL APIENTRY DllMain(HANDLE, DWORD ul_reason_for_call, LPVOID)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

#endif

namespace FBMailUDF
{
	MessageServer __messageServerInstance = MessageServer();
}

FBUDF_API FB_BIGINT fbSMTPServerAdd(const int &port, const int &securityType, const char *serverName,
	const char *userName, const char *password, const char *database)
{
	try
	{
		return FBMailUDF::__messageServerInstance.addServer(serverName ? std::string(serverName) : "", port,
			userName ? std::string(userName) : "", password ? std::string(password) : "",
			database ? std::string(database) : "", securityType);
	}
	catch (...)
	{
		return FBMailUDF::EMailResult::GeneralError;
	}
}

FBUDF_API int fbSMTPServerRemove(const FB_BIGINT &serverID)
{
	try
	{
		return FBMailUDF::__messageServerInstance.removeServer(serverID);
	}
	catch (...)
	{
		return FBMailUDF::EMailResult::GeneralError;
	}
}

FBUDF_API int fbSMTPMessageSend(const FB_BIGINT &serverID, const FB_BIGINT &id, const int &priority,
	const int &sendImmediate, const char *senderName, const char *senderEmail, const char *recipient,
	const char *subject, const char *message)
{
	try
	{
		return FBMailUDF::__messageServerInstance.sendMessage(serverID, id,	senderName ? std::string(senderName) : "",
			senderEmail ? std::string(senderEmail) : "", recipient ? std::string(recipient) : "",
			recipient ? std::string(recipient) : "", subject ? std::string(subject) : "", 
			message ? std::string(message) : "", priority, sendImmediate != 0);
	}
	catch (...)
	{
		return FBMailUDF::EMailResult::GeneralError;
	}
}

FBUDF_API int fbSMTPMessageSendEx(const FB_BIGINT &serverID, const FB_BIGINT &id, const int &priority,
	const int &sendImmediate, const char *senderName, const char *senderEmail, const char *recipientName,
	const char *recipientEmail, const char *subject, const char *message)
{
	try
	{
		return FBMailUDF::__messageServerInstance.sendMessage(serverID, id, senderName ? std::string(senderName) : "",
			senderEmail ? std::string(senderEmail) : "", recipientName ? std::string(recipientName) : "",
			recipientEmail ? std::string(recipientEmail): "", subject ? std::string(subject) : "", 
			message ? std::string(message) : "", priority, sendImmediate != 0);
	}
	catch (...)
	{
		return FBMailUDF::EMailResult::GeneralError;
	}
}


FBUDF_API int fbSMTPMessageResult(const FB_BIGINT &serverID, const FB_BIGINT &emailID, const int &eraseMessage)
{
	try
	{
		std::string messageResult = "";
		int errCode = 0;

		FBMailUDF::EMailResult result = FBMailUDF::__messageServerInstance.messageSendResult(serverID, emailID,
			eraseMessage != 0, messageResult, errCode);

		return result;
	}
	catch (...)
	{
		return FBMailUDF::EMailResult::GeneralError;
	}
}

FBUDF_API int fbSMTPMessageResultText(const FB_BIGINT &serverID, const FB_BIGINT &emailID, char *message)
{
	try
	{
		if (!message)
			return (EMailResult::InvalidMessageBuffer);

		std::string messageResult = "";
		int errCode = 0;

		FBMailUDF::EMailResult result = FBMailUDF::__messageServerInstance.messageSendResult(serverID, emailID,
			true, messageResult, errCode);

		if (result == EMailResult::Success || result == EMailResult::NotFound)
			return result;

		if (messageResult.length() > MAX_ERROR_MESSAGE_LENGTH -1)
		{
			messageResult = messageResult.substr(0, MAX_ERROR_MESSAGE_LENGTH -1);
		}

		std::strcpy(message, messageResult.c_str());

		return errCode;
	}
	catch (...)
	{
		return FBMailUDF::EMailResult::GeneralError;
	}
}

FBUDF_API int fbSMTPMessageCount(const char *database, const int &cancelAll, const int &sleep)
{
	try
	{
		return FBMailUDF::__messageServerInstance.messageCount(database ? std::string(database) : "", cancelAll != 0, sleep);
	}
	catch (...)
	{
		return FBMailUDF::EMailResult::GeneralError;
	}
}
