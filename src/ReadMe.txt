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
* Description: Readme text file
*
* Date: 12/07/2017
*
*/

fbSmtpUDF is a Firebird UDF that can send SMTP messages.

The UDF can work in 2 ways, you can either send messages directly, or queue messages for sending via another internally managed thread.

If you send messages directly you could possibly delay the execution of the SP or trigger whilst communication with remote SMTP server takes place.

Version 1.2.0.0

Version 1.2.0.0 bug fixes
=========================

Reply to specified name when sending direct email instead of thread queueing.


Version 1.1.0.0

Version 1.1.0.0 bug fixes
=========================

Reply to specifies sender email, not name.
New external function to specify senders name and email, not just email.
Updated sample (see below).
Upgrade to latest C++ toolset



External Function Definitions
=============================
/*

SMTPServerAdd 
=============

Description: used to add server details, each message being sent requires server details.

Parameters:
	port - SMTP port (i.e. 25)
	securityType - See security types below
	serverName - SMTP Server Ip address or name (i.e. smtp.google.com) 
	userName - SMTP username
	password - SMTP password
	database - name of database for ease use RDB$GET_CONTEXT('SYSTEM', 'DB_NAME')

Returns:
if the call worked a unique id, greater than zero, identifying the server, this is required 
when removing a server, sending or queueing messages or querying message status.  If the 
function failed see Global Return Values below.

Declaration:

DECLARE EXTERNAL FUNCTION SMTPServerAdd (INTEGER, INTEGER, CSTRING(100), CSTRING(100), CSTRING(100), CSTRING(100))
RETURNS BIGINT BY VALUE
ENTRY_POINT 'fbSMTPServerAdd'
MODULE_NAME 'fbSmtpUDF';



SMTPServerRemove
================

Description: Removes an SMTP server details

Parameters:
	serverID - unique server id obtained by calling SMTPServerAdd

Returns:
See Global Return Values below.

Declaration:

DECLARE EXTERNAL FUNCTION SMTPServerRemove (BIGINT)
RETURNS INTEGER BY VALUE
ENTRY_POINT 'fbSMTPServerRemove'
MODULE_NAME 'fbSmtpUDF';



SMTPSendEmail
=============

Description: Called to send an email via SMTP.

Parameters:
	serverID - unique server id obtained by calling SMTPServerAdd
	id - unique user defined id to identify this email when querying for results
	priority - 0 is low, 2 is high, anything else is normal priority
	sendImmediate - 0 is add to queue anything else is sent immediately
	senderName - name of sender as appearing in the email header on client 
	senderEmail - sender's email address
	recipient - recipient email address
	subject - message subject
	message - message body

Returns:

If sendImmediate is 0, then the Global Return Value will be success if the message was succesfully queud for sending, if 
sendImmediate is any other value Global Return Value will be success if the message was actually sent, for any other
value see Global Return Values below.

Declaration:

DECLARE EXTERNAL FUNCTION SMTPSendEmail (BIGINT, BIGINT, INTEGER, INTEGER, CSTRING(100), CSTRING(100), CSTRING(100), CSTRING(100), CSTRING(32767))
RETURNS INTEGER BY VALUE
ENTRY_POINT 'fbSMTPMessageSend'
MODULE_NAME 'fbSmtpUDF';


SMTPSendEmailEx
===============

Description: Called to send an email via SMTP.  Allows users to specify recipient name and email

Parameters:
	serverID - unique server id obtained by calling SMTPServerAdd
	id - unique user defined id to identify this email when querying for results
	priority - 0 is low, 2 is high, anything else is normal priority
	sendImmediate - 0 is add to queue anything else is sent immediately
	senderName - name of sender as appearing in the email header on client 
	senderEmail - sender's email address
	recipientName - recipient name
	recipientEmail - recipient email address
	subject - message subject
	message - message body

Returns:

If sendImmediate is 0, then the Global Return Value will be success if the message was succesfully queud for sending, if 
sendImmediate is any other value Global Return Value will be success if the message was actually sent, for any other
value see Global Return Values below.

Declaration:

DECLARE EXTERNAL FUNCTION SMTPSendEmailEx (BIGINT, BIGINT, INTEGER, INTEGER, CSTRING(100), CSTRING(100), CSTRING(100), CSTRING(100), CSTRING(100), CSTRING(32767))
RETURNS INTEGER BY VALUE
ENTRY_POINT 'fbSMTPMessageSendEx'
MODULE_NAME 'fbSmtpUDF';



SMTPMessageCount
================

Description:  Determines how many messages are queued for a database (across all connections)

Parameters:
	Database Name -  name of database for ease use RDB$GET_CONTEXT('SYSTEM', 'DB_NAME') 
	Cancel All - 0 is false, any other value is true, if true then all queued messages will be removed.
	Sleep - number of milliseconds to sleep, only valid if the number of messages in the queue is greater than zero.  This could
			be used if you need to loop whilst waiting for the results to prevent heavy CPU usage.  (max 1000 ms)

Returns:

Number of messages in the queue

DECLARE EXTERNAL FUNCTION SMTPMessageCount(CSTRING(100), INT, INT)
RETURNS INTEGER BY VALUE
ENTRY_POINT 'fbSMTPMessageCount'
MODULE_NAME 'fbSmtpUDF';


SMTPSendResult
==============

Description:  Used to obtain the result of a message that has been sent.

Parameters:
	serverID - id of server, obtained by calling SMTPServerAdd
	emailID - unique user defined id of the message that has been sent
	eraseMessage - 0 is false, anything else true, if true then will erase the message result if found, after returning the results.

Returns:

See Global Return Values below.

Declaration:

DECLARE EXTERNAL FUNCTION SMTPSendResult(BIGINT, BIGINT, INT)
RETURNS INTEGER BY VALUE 
ENTRY_POINT 'fbSMTPMessageResult'
MODULE_NAME 'fbSmtpUDF';



SMTPSendResultText
==================

Description:  Used to obtain the result, as a string, if the message failed to send.

Parameters:
	serverID - id of server, obtained by calling SMTPServerAdd
	emailID - unique user defined id of the message that has been sent

Returns:
	error message if not sent, null or empty string if email was sent.

Decalration:

DECLARE EXTERNAL FUNCTION SMTPSendResultText(BIGINT, BIGINT, CSTRING(300))
RETURNS PARAMETER 3
ENTRY_POINT 'fbSMTPMessageResultText'
MODULE_NAME 'fbSmtpUDF';


Global Return Values
====================

Success = 0 -- the operation worked

InvalidSenderEmail = -1 -- sender email invalid, null or empty

InvalidRecipientEmail = -2 -- recipient email invalid, null or empty

InvalidSubject = -3 -- email subject null or empty

InvalidContent = -4 -- email content null or empty

InvalidServer = -5 -- invalid server, server has not been added 

ServerNotFound = -6 -- server could not be found

InvalidServerName = -7 -- smtp server name is null or empty

InvalidServerUser = -8 -- smtp user name is null or empty

InvalidServerPassword = -9 -- smtp password is null or empty

InvalidServerPort = -10 -- invalid server port, must be between 1 and 65535

InvalidDatabaseName = -11 -- invalid database name

NotSent = -12 -- message has not been sent (still in a queue)

NotFound = -13  --Item was not found

InvalidMessageBuffer = -14  -- Message buffer can not be null

GeneralError = -999 - something unknown went wrong!!!!



Security Types:

NO_SECURITY = 0
USE_TLS = 1
USE_SSL = 2
DO_NOT_SET = 3



Example Usage:

SET TERM ^ ;

CREATE OR ALTER PROCEDURE TEST_EMAIL 
RETURNS 
(
  MSG VARCHAR(1000), 
  RESULT BIGINT
)
AS 
DECLARE VARIABLE vServerID BIGINT; 
DECLARE VARIABLE vImmediate INTEGER;
DECLARE VARIABLE vCount INTEGER;
DECLARE VARIABLE vResultText VARCHAR(1000);
BEGIN
    vServerID = SMTPSERVERADD(25, 0, 'smtp.domain.com', 'account@domain.com', 'passw0rd', RDB$GET_CONTEXT('SYSTEM', 'DB_NAME'));
    Result = vServerID;
    MSG = 'Add Server';
    suspend;

    vImmediate = 1; -- 0 = queue in thread anything else send immediately

	RESULT = SMTPSENDEMAILEX(:vServerID, 10, 5, vImmediate, 'account@domain.com', 'account@domain.com', 'Another User', 'user@anotherdomain.com',
		'test from fbudf smtp in FB3', 'this is a test message only');
    MSG = 'Send Email';
    SUSPEND;
    
    RESULT = 1;
    
    WHILE (RESULT > 0) DO
    BEGIN
        RESULT = SMTPMESSAGECOUNT(RDB$GET_CONTEXT('SYSTEM', 'DB_NAME'), 0, 50);
        MSG = 'Queue Count';
        SUSPEND;
    END
    
    RESULT = -13; -- not found
    
    WHILE (RESULT = -13) DO
    BEGIN
        RESULT = SMTPSendResult(vServerID, 10, 0);
        MSG = 'Retrieve Send Result';
        SUSPEND;
    END
    
    IF (RESULT = 0) THEN
    BEGIN 
        MSG = vResultText;
        SUSPEND;
    END ELSE
    BEGIN
        MSG = SMTPSendResultText(vServerID, 10);
        SUSPEND;
    END
    
	-- only remove the server when you have finished using it
	SMTPSERVERREMOVE(vServerID);
    MSG = 'Remove Server';
    SUSPEND;
END^

SET TERM ; ^


-- to call
SELECT MSG, RESULT 
FROM TEST_EMAIL;

Results:

Add Server	1499859179
Send Email	0
Queue Count	0
Retrieve Send Result	0
[null]	0
Remove Server	0
