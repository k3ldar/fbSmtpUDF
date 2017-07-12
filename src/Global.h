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
* Description: Global Definitions
*
* Date: 12/07/2017
*
*/


#ifndef FB_SMTP__MAIL_GLOBAL
#define FB_SMTP__MAIL_GLOBAL

#include <string>
#include <time.h>
#include <vector>
#include <list>
#include <stdint.h>

#include "ManagedThread.h"

namespace FBMailUDF
{
	typedef unsigned short PortNumber;
	typedef int64_t FB_BIGINT;

	#define SMTP_THREAD_NAME "FB UDF SMTP Send Thread"

	const int MIN_SUBJECT_LENGTH = 10;				// minimum mail subject length
	const int MAX_SERVER_STRING_LENGHT = 100;		// max length of server name/user/pass
	const int MAIL_SEND_DELAY = 200;				// 200 ms between each mail sent
	const int THREAD_RUN_INTERVAL_SECONDS = 10000;	// send thread runs every 10 seconds
	const int MAX_ERROR_MESSAGE_LENGTH = 300;		// maximum length of an error message
	const int MAX_SLEEP_DELAY = 1000;				// maximum sleep delay when checking message count

	enum EMailResult
	{
		Success = 0,

		InvalidSenderEmail = -1,

		InvalidRecipientEmail = -2,

		InvalidSubject = -3,

		InvalidContent = -4,

		InvalidServer = -5,

		ServerNotFound = -6,

		InvalidServerName = -7,

		InvalidServerUser = -8,

		InvalidServerPassword = -9,

		InvalidServerPort = -10,

		InvalidDatabaseName = -11,

		NotSent = -12,

		NotFound = -13,

		InvalidMessageBuffer = -14,

		GeneralError = -999
	};

	class MailMessage;
	class MailServer;
	class MailSendNotification;
	class MailSendResult;

	typedef std::vector<MailMessage> MailMessageList;
	typedef std::vector<MailServer> MailServerList;
	typedef std::vector<MailSendNotification*> MailSendNotificationList;
	typedef std::vector<MailSendResult> MailSendResultList;
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#ifdef __FB_MESSAGE_SERVER_FUNCTIONS
#define FBUDF_API __declspec(dllexport)
#else
#define FBUDF_API __declspec(dllimport)
#endif
#endif

#endif
