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
* Description: Message management class
*
* Date: 12/07/2017
*
*/


#ifndef FB_SMTP__MESSAGE_SERVER
#define FB_SMTP__MESSAGE_SERVER

#include <mutex>
#include <chrono>
#include "Global.h"
#include "ManagedThread.h"
#include "MailServer.h"
#include "MailMessage.h"
#include "MessageSendThread.h"


namespace FBMailUDF
{
	class MessageServer : private MailSendNotification
	{
	private:
		MailServerList messageServers;
		MessageSendThread mailThread;
		MailSendResultList resultList;
	public:
		MessageServer();
		~MessageServer();
		FB_BIGINT addServer(const std::string &serverName, const PortNumber port, const std::string &userName,
			const std::string &password, const std::string database, int securityType);
		EMailResult removeServer(FB_BIGINT mailServer);
		EMailResult sendMessage(const FB_BIGINT serverID, const FB_BIGINT id, const std::string &senderName, 
			const std::string &senderEmail, const std::string &recipient, const std::string &subject, 
			const std::string &message, const int priority, const bool immediate);
		int messageCount(const std::string &database, const bool cancelAll, const int sleepDelay);
		EMailResult messageSendResult(const FB_BIGINT serverID, const FB_BIGINT emailID, const bool eraseMessage,
			std::string &sendResult, int &errorCode);

		void Notify(MailSendResult messageResult);
	};
}

#endif