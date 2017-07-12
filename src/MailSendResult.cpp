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
* Description: Class for holding results of messages sent
*
* Date: 12/07/2017
*
*/


#include "MailSendResult.h"

namespace FBMailUDF
{
	MailSendResult::MailSendResult(FB_BIGINT message, FB_BIGINT server, EMailResult result)
	{
		time(&resultTime);

		messageID = message;
		serverID = server;
		sendResult = result;
		errorMessage = "";
		errorCode = 0;
	}

	MailSendResult::MailSendResult(FB_BIGINT message, FB_BIGINT server, EMailResult result, 
		std::string errorMessage, int errorCode)
		: MailSendResult(message, server, result)
	{
		time(&resultTime);

		messageID = message;
		serverID = server;
		sendResult = result;
	}


	MailSendResult::~MailSendResult()
	{
	}

	FB_BIGINT MailSendResult::getMessageID()
	{
		return (messageID);
	}

	FB_BIGINT MailSendResult::getServerID()
	{
		return (serverID);
	}

	EMailResult MailSendResult::getSendResult()
	{
		return (sendResult);
	}

	void MailSendResult::setSendResult(EMailResult result)
	{
		sendResult = result;
	}

	time_t MailSendResult::getResultTime()
	{
		return (resultTime);
	}

	std::string MailSendResult::getErrorMessage()
	{
		return (errorMessage);
	}

	void MailSendResult::setErrorMessage(std::string message)
	{
		errorMessage = message;
	}

	int MailSendResult::getErrorCode()
	{
		return (errorCode);
	}

	void MailSendResult::setErrorCode(int code)
	{
		errorCode = code;
	}
}
