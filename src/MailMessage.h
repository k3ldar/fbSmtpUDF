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


#ifndef FB_SMTP__MAIL_MESSAGE
#define FB_SMTP__MAIL_MESSAGE

#include <regex>

#include "CSmtp.h"
#include "Global.h"
#include "MailServer.h"

namespace FBMailUDF
{
	class MailMessage
	{
		FB_BIGINT id;
		std::string senderName;
		std::string senderEmail;
		std::string recipientName;
		std::string recipientEmail;

		std::string subject;
		std::string message;

		CSmptXPriority priority;

		bool sent;
		time_t sendTime;
		MailServer mailServer;
	public:
		MailMessage();
		MailMessage(MailServer &server, const FB_BIGINT id, const std::string &sendName, const std::string &sendEmail, 
			const std::string &recName, const std::string &recEmail, const std::string &subj, const std::string &msg, 
			const int priority);
		~MailMessage();

		// property wrappers
		FB_BIGINT getMessageID();
		std::string getSenderName();
		std::string getSenderEmail();
		std::string getRecipientName();
		std::string getRecipientEmail();
		std::string getSubject();
		std::string getMessage();
		CSmptXPriority getPriority();
		MailServer getMailServer();

		void messageSent();
		bool isSent();
		time_t sendDateTime();

		//general
		EMailResult canSend();
		bool isHTML();
	};
}

#endif
