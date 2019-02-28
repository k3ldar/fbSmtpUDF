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
* Description: Thread class for sending SMTP messages.
*
* Date: 12/07/2017
*
*/


#include "MessageSendThread.h"

namespace FBMailUDF
{
	std::mutex listenerLock;
	std::mutex sendListLockMutex;
	std::mutex queueLockMutex;
	MailMessageList messagesToSend;
	MailMessageList messageQueue;

	MessageSendThread::MessageSendThread()
		: ManagedThread(SMTP_THREAD_NAME, THREAD_RUN_INTERVAL_SECONDS, 0, true, false)
	{
	}

	MessageSendThread::~MessageSendThread()
	{
	}

	void MessageSendThread::terminate()
	{
		setIsTerminated(true);
	}

	bool MessageSendThread::run()
	{
		std::lock_guard<std::mutex> guard(sendListLockMutex);

		while (messagesToSend.size() > 0)
		{
			// check to see if cancelled
			if (getIsCancelled())
				return (false);

			MailMessage message = messagesToSend.front();

			MailSendResult result = MailSendResult(message.getMessageID(), 
				message.getMailServer().getServerID(), EMailResult::NotSent);
			try
			{
				CSmtp mail;

				mail.SetSMTPServer(message.getMailServer().getServerName().c_str(), 
					message.getMailServer().getPortNumber());
				mail.SetSecurityType(message.getMailServer().getSecurityType());
				mail.SetLogin(message.getMailServer().getUserName().c_str());
				mail.SetPassword(message.getMailServer().getUserPassword().c_str());
				mail.SetXMailer(message.getMailServer().getXMailer().c_str());
				mail.SetXPriority(message.getPriority());

				mail.SetSenderName(message.getSenderName().c_str());
				mail.SetSenderMail(message.getSenderEmail().c_str());
				mail.SetReplyTo(message.getSenderEmail().c_str());

				mail.SetSubject(message.getSubject().c_str());
				mail.AddRecipient(message.getRecipientEmail().c_str(), message.getRecipientName().c_str());

				std::istringstream f(message.getMessage());
				std::string line;

				while (std::getline(f, line))
				{
					mail.AddMsgLine(line.c_str());
				}

				mail.Send();
				result.setSendResult(EMailResult::Success);
			}
			catch (const ECSmtp &e)
			{
				result.setErrorCode(e.GetErrorNum());
				result.setErrorMessage(e.GetErrorText().c_str());
			}
			catch (const std::exception &e)
			{
				result.setErrorMessage(e.what());
			}

			notifyMailListeners(result);
			messagesToSend.erase(messagesToSend.cbegin());

			std::this_thread::sleep_for(chrono::milliseconds(MAIL_SEND_DELAY));
		}

		{
			// if more messages have been queud since this thread started running
			// copy them to the primary send list now
			std::lock_guard<std::mutex> guard(queueLockMutex);

			while (messageQueue.size() > 0)
			{
				MailMessage message = messageQueue.front();
				messagesToSend.push_back(message);
				messageQueue.erase(messageQueue.cbegin());
			}
		}

		return (messagesToSend.size() > 0);
	}

	// private methods

	EMailResult MessageSendThread::sendImmediate(MailMessage &message)
	{
		MailSendResult result = MailSendResult(message.getMessageID(),
			message.getMailServer().getServerID(), EMailResult::NotSent);
		try
		{
			CSmtp mail;

			mail.SetSMTPServer(message.getMailServer().getServerName().c_str(),
				message.getMailServer().getPortNumber());
			mail.SetSecurityType(message.getMailServer().getSecurityType());
			mail.SetLogin(message.getMailServer().getUserName().c_str());
			mail.SetPassword(message.getMailServer().getUserPassword().c_str());
			mail.SetXMailer(message.getMailServer().getXMailer().c_str());
			mail.SetXPriority(message.getPriority());

			mail.SetSenderName(message.getSenderName().c_str());
			mail.SetSenderMail(message.getSenderEmail().c_str());
			mail.SetReplyTo(message.getSenderName().c_str());

			mail.SetSubject(message.getSubject().c_str());
			mail.AddRecipient(message.getRecipientEmail().c_str());

			std::istringstream f(message.getMessage());
			std::string line;

			while (std::getline(f, line))
			{
				mail.AddMsgLine(line.c_str());
			}

			mail.Send();
			result.setSendResult(EMailResult::Success);
		}
		catch (const ECSmtp &e)
		{
			result.setErrorCode(e.GetErrorNum());
			result.setErrorMessage(e.GetErrorText().c_str());
		}
		catch (const std::exception &e)
		{
			result.setErrorMessage(e.what());
		}

		notifyMailListeners(result);

		return (result.getSendResult());
	}

	void MessageSendThread::notifyMailListeners(MailSendResult notification)
	{
		std::lock_guard<std::mutex> guard(listenerLock);

		for (size_t i = 0; i < emailResultListeners.size(); i++)
		{
			try
			{
				emailResultListeners.at(i)->Notify(notification);
			}
			catch (...){} // ignore all listener errors
		}
	}

	// public methods
	void MessageSendThread::addMailListener(MailSendNotification *listener)
	{
		std::lock_guard<std::mutex> guard(listenerLock);
		emailResultListeners.push_back(listener);
	}

	void MessageSendThread::removeMailListener(MailSendNotification *listener)
	{
		std::lock_guard<std::mutex> guard(listenerLock);

		if (emailResultListeners.size() == 0)
			return;

		for (size_t i = 0; i < listeners.size(); i++)
		{
			if (emailResultListeners.at(i) == listener)
			{
				listeners.erase(listeners.begin() + i);
				break;
			}
		}
	}

	// static methods
	void MessageSendThread::messageAdd(MailMessage message)
	{
		std::lock_guard<std::mutex> guard(queueLockMutex);
		messageQueue.push_back(message);
	}

	int MessageSendThread::messageQueueCount(const std::string &database, const bool removeAll)
	{
		int Result = 0;

		{
			std::lock_guard<std::mutex> guard(sendListLockMutex);
			
			for (size_t i = messagesToSend.size(); i > 0; i--)
			{
				if (messagesToSend.at(i -1).getMailServer().getDatabase().compare(database) == 0)
					Result++;

				if (removeAll)
					messagesToSend.erase(messagesToSend.cbegin() + (i -1));
			}
		}

		std::lock_guard<std::mutex> guard(queueLockMutex);

		for (size_t i = messageQueue.size(); i > 0; i--)
		{
			if (messageQueue.at(i -1).getMailServer().getDatabase().compare(database) == 0)
				Result++;

			if (removeAll)
				messageQueue.erase(messageQueue.cbegin() + (i -1));
		}

		return (Result);
	}

	void MessageSendThread::start()
	{
		ManagedThreads::ManagedThread::start(ManagedThreads::ThreadPriority::BelowNormal);
	}

	void MessageSendThread::cancel()
	{
		ManagedThreads::ManagedThread::cancel();
	}

	void MessageSendThread::cancelAll(const std::string &database)
	{
		std::lock_guard<std::mutex> guard(sendListLockMutex);

		if (messagesToSend.size() == 0)
			return;

		for (size_t i = messagesToSend.size() - 1; i >= 0; i--)
		{
			if (messagesToSend.at(i).getMailServer().getDatabase().compare(database) == 0)
				messagesToSend.erase(messagesToSend.cend() - i);
		}
	}
}