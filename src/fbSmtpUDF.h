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


#include "Global.h"
#include "MessageServer.h"


extern "C"
{
	using namespace FBMailUDF;

	FBUDF_API FB_BIGINT fbSMTPServerAdd(const int &port, const int &securityType, const char *serverName, 
		const char *userName, const char *password, const char *database);

	FBUDF_API int fbSMTPServerRemove(const FB_BIGINT &serverID);

	FBUDF_API int fbSMTPMessageSend(const FB_BIGINT &serverID, const FB_BIGINT &id, const int &priority, 
		const int &sendImmediate, const char *senderName, const char *senderEmail, const char *recipient, 
		const char *subject, const char *message);

	FBUDF_API int fbSMTPMessageResult(const FB_BIGINT &serverID, const FB_BIGINT &emailID, const int &eraseMessage);

	FBUDF_API int fbSMTPMessageResultText(const FB_BIGINT &serverID, const FB_BIGINT &emailID, char *message);

	FBUDF_API int fbSMTPMessageCount(const char *database, const int &cancelAll, const int &sleep);
}
