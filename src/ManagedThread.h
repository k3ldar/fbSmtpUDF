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
* Description: Managed Thread, base class for managed threads, derived classes
*			   can override the run method to execute code in a thread.
*
* Date: 12/07/2017
*
*/


#ifndef __FBTASK_MANAGED_THREAD__
#define __FBTASK_MANAGED_THREAD__

#define _WINSOCKAPI_
#include <windows.h>

#include <winnt.h>
#include <thread>
#include <mutex>
#include <vector>

namespace ManagedThreads
{
	enum ThreadPriority
	{
		AboveNormal = 1,
		BelowNormal = -1,
		Normal = 0,
		Highest = 2,
		Critical = 15
	};

	enum EventType
	{
		Start,			// thread has started
		Run,			// thread is about to run/execute (call derived thread Run)
		Stop,			// thread is about to stop
		Error,			// thread has encountered an exception
		Cancelled		// thread has been cancelled
	};


	class ManagedThread;
	class ThreadNotification;

	typedef std::vector<ManagedThread*> ThreadList;
	typedef std::vector<ThreadNotification*> NotificationList;
	typedef unsigned int uInt;

#ifdef _WIN64
	typedef size_t Size;
#else
	typedef int Size;
#endif

	class ManagedThread
	{
	private:
		std::thread *currentThread;
		ThreadPriority priority;
		bool runAtStart;
		bool isCancelled;
		bool isTerminated;
		bool isDetached;
		bool isJoined;
		bool automaticallyDelete;
		time_t cancelledTime;
		time_t threadStarted;
		uInt delayStart;
		uInt managedID;
		std::chrono::milliseconds runInterval;
		std::string name;

		void runThread();

		void notifyListeners(const EventType &type, ManagedThread *thread, const std::string &error = "");

		// prevent class copying
		ManagedThread(const ManagedThread&); 
		ManagedThread& operator=(const ManagedThread&); 
	protected:
		NotificationList listeners;
		virtual bool run() = 0;
		void setIsTerminated(bool value);
	public:
		ManagedThread();
		ManagedThread(const uInt interval, const uInt startDelay, bool autoDelete);
		ManagedThread(const std::string &name, const uInt interval, const uInt startDelay, 
			const bool runImmediately, const bool autoDelete);
		~ManagedThread();

		// operator overloads for comparisons
		bool operator ==(const ManagedThread& val) const { return (managedID == val.managedID); }
		bool operator !=(const ManagedThread& val) const { return (managedID != val.managedID); }
		bool operator <(const ManagedThread& val) const { return (managedID < val.managedID); }
		bool operator >(const ManagedThread& val) const { return (managedID > val.managedID); }

		// virtual methods
		virtual void terminate() = 0;
		virtual void cancel();

		//static methods
		static Size activeThreadCount();
		static ManagedThread* activeThreadAt(const int index);
		static ManagedThread* getActiveThread(const std::string &threadName);
		static void cancelAll();
		static bool cancel(const std::string &threadName, bool removeListeners);
		static void terminateAll(uInt timeout = 5000);
		static bool exists(const std::string &name);
		

		void addListener(ThreadNotification *listener);
		void removeListener(ThreadNotification *listener);

		// property wrappers
		bool getIsCancelled();
		time_t getTimeStarted();
		time_t getTimeCancelled();
		uInt getThreadID();
		void setPriority(const ThreadPriority &value);
		ThreadPriority getPriority();
		bool isRunning();
		std::string getName();

		// standard methods
		void start();
		void start(const ThreadPriority &priority, bool autoDetach = true);
		void join();
		bool joinable();
		void detach();
	};

	class ThreadNotification
	{
	public:
		virtual void Notify(const EventType &type, ManagedThread *thread, const std::string &data) = 0;
	};
}

#endif

