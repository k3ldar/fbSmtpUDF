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


#include "MessageServer.h"

namespace FBMailUDF
{
	std::mutex serverListLockMutex;
	std::mutex resultListLockMutex;

	MessageServer::MessageServer()
	{
		
	}

	MessageServer::MessageServer(const MessageServer &copy)
	{

	}

	MessageServer::~MessageServer()
	{
		mailThread.terminate();
	}


	FB_BIGINT MessageServer::addServer(const std::string &serverName, const PortNumber port, const std::string &userName,
		const std::string &password, const std::string database, int securityType)
	{
		std::lock_guard<std::mutex> guard(serverListLockMutex);

		switch (securityType)
		{
			case 0:
			case 1:
			case 2:
			case 3:
				break;
			default:
				securityType = 0;
		}

		time_t currentTime;
		time(&currentTime);

		MailServer newServer = MailServer(static_cast<FB_BIGINT>(currentTime), serverName, port, 
			static_cast<SMTP_SECURITY_TYPE>(securityType), userName, password, database);
		EMailResult isValid = newServer.isValidServer();
		
		if (isValid != EMailResult::Success)
			return (isValid);

		for (size_t i = 0; i < messageServers.size(); i++)
		{
			if (messageServers.at(i) == newServer)
			{
				return (messageServers.at(i).getServerID());
			}
		}

		messageServers.push_back(newServer);
		return (newServer.getServerID());
	}

	EMailResult MessageServer::removeServer(FB_BIGINT mailServer)
	{
		std::lock_guard<std::mutex> guard(serverListLockMutex);

		for (size_t i = 0; i < messageServers.size(); i++)
		{
			if (messageServers.at(i).getServerID() == mailServer)
			{
				messageServers.erase(messageServers.cbegin() + i);

				return (EMailResult::Success);
			}
		}

		return (EMailResult::ServerNotFound);
	}

	EMailResult MessageServer::sendMessage(const FB_BIGINT serverID, const FB_BIGINT id, const std::string &senderName, 
		const std::string &senderEmail, const std::string &recipientName, const std::string &recipientEmail, 
		const std::string &subject, const std::string &message, const int priority, const bool immediate)
	{
		MailServer *server = nullptr;

		{
			std::lock_guard<std::mutex> guard(serverListLockMutex);

			for (size_t i = 0; i < messageServers.size(); i++)
			{
				if (messageServers.at(i).getServerID() == serverID)
				{
					server = &messageServers.at(i);
					break;
				}
			}
		}

		if (server == nullptr)
			return (EMailResult::InvalidServer);

		FBMailUDF::MailMessage msg = FBMailUDF::MailMessage(*server, id, senderName, senderEmail, 
			recipientName, recipientEmail, subject, message, priority);

		FBMailUDF::EMailResult check = msg.canSend();

		if (check == EMailResult::Success)
		{
			if (immediate)
			{
				return (mailThread.sendImmediate(msg));
			}
			else
			{
				MessageSendThread::messageAdd(msg);

				if (!ManagedThreads::ManagedThread::exists(SMTP_THREAD_NAME))
				{
					mailThread.addMailListener(this);
					mailThread.start();
				}
			}
		}

		return (check);
	}

	EMailResult MessageServer::messageSendResult(const FB_BIGINT serverID, const FB_BIGINT emailID, const bool eraseMessage, 
		std::string &sendResult, int &errorCode)
	{
		std::lock_guard<std::mutex> guard(resultListLockMutex);
		
		for (size_t i = 0; i < resultList.size(); i++)
		{
			MailSendResult result = resultList.at(i);

			if (result.getServerID() == serverID && result.getMessageID() == emailID)
			{
				sendResult = result.getErrorMessage();
				errorCode = result.getErrorCode();

				if (eraseMessage)
					resultList.erase(resultList.cbegin() + i);

				return (result.getSendResult());
			}
		}

		return (EMailResult::NotFound);
	}

	int MessageServer::messageCount(const std::string &database, const bool cancelAll, const int sleepDelay)
	{
		int Result = mailThread.messageQueueCount(database, cancelAll);

		if (Result > 0 && sleepDelay > 0)
			std::this_thread::sleep_for(chrono::milliseconds(sleepDelay > MAX_SLEEP_DELAY ? MAX_SLEEP_DELAY : sleepDelay));

		return (Result);
	}

	void MessageServer::Notify(MailSendResult messageResult)
	{
		std::lock_guard<std::mutex> guard(resultListLockMutex);
		resultList.push_back(messageResult);
	}
}
