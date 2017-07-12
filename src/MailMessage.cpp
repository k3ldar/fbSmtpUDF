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
* Description: Class to hold an smtp message
*
* Date: 12/07/2017
*
*/


#include "MailMessage.h"


namespace FBMailUDF
{
	MailMessage::MailMessage()
	{
	
	}
	
	MailMessage::MailMessage(MailServer &server, const FB_BIGINT id, const std::string &sendName, const std::string &sendEmail,
		const std::string &recName,	const std::string &recEmail, const std::string &subj, const std::string &msg, 
		const int priority)
	{
		mailServer = server;
		this->id = id;
		senderName = sendName;
		senderEmail = sendEmail;
		recipientName = recName;
		recipientEmail = recEmail;
		subject = subj;
		message = msg;

		switch (priority)
		{
		case 2:
			this->priority = CSmptXPriority::XPRIORITY_HIGH;
			break;
		case 0:
			this->priority = CSmptXPriority::XPRIORITY_LOW;
		default:
			this->priority = CSmptXPriority::XPRIORITY_NORMAL;
			break;
		}

		sent = false;
	}

	MailMessage::~MailMessage()
	{

	}

	FB_BIGINT MailMessage::getMessageID()
	{
		return (id);
	}

	std::string MailMessage::getSenderName()
	{
		return (senderName);
	}

	std::string MailMessage::getSenderEmail()
	{
		return (senderEmail);
	}

	std::string MailMessage::getRecipientName()
	{
		return (recipientName);
	}

	std::string MailMessage::getRecipientEmail()
	{
		return (recipientEmail);
	}

	std::string MailMessage::getSubject()
	{
		return (subject);
	}

	std::string MailMessage::getMessage()
	{
		return (message);
	}

	CSmptXPriority MailMessage::getPriority()
	{
		return (priority);
	}

	MailServer MailMessage::getMailServer()
	{
		return (mailServer);
	}

	void MailMessage::messageSent()
	{
		sent = true;
		time(&sendTime);
	}

	EMailResult MailMessage::canSend()
	{
		// a few basic validation checks on the message
		if (message.empty())
			return (EMailResult::InvalidContent);

		if (senderEmail.empty())
			return (EMailResult::InvalidSenderEmail);

		if (recipientEmail.empty())
			return (EMailResult::InvalidRecipientEmail);

		std::regex emailCheck = std::regex("^[_a-z0-9-]+(\\.[_a-z0-9-]+)*@[a-z0-9-]+(\\.[a-z0-9-]+)*(\\.[a-z]{2,4})$");

		if (!regex_match(senderEmail.cbegin(), senderEmail.cend(), emailCheck))
			return (EMailResult::InvalidSenderEmail);

		if (!regex_match(recipientEmail.cbegin(), recipientEmail.cend(), emailCheck))
			return (EMailResult::InvalidRecipientEmail);

		if (subject.empty() || subject.size() < MIN_SUBJECT_LENGTH)
			return (EMailResult::InvalidSubject);

		if (senderName.empty())
			senderName = senderEmail;

		if (recipientName.empty())
			recipientName = recipientEmail;

		return (EMailResult::Success);
	}

	bool MailMessage::isHTML()
	{
		std::string start = message.substr(0, 6);
		std::transform(start.cbegin(), start.cend(), start.begin(), ::tolower);
		return (start.compare("<html>") == 0);
	}

	bool MailMessage::isSent()
	{
		return (sent);
	}

	time_t MailMessage::sendDateTime()
	{
		return (sendTime);
	}
}
