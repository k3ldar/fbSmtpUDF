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


#include "ManagedThread.h"

namespace ManagedThreads
{
	ThreadList globalThreadList = ThreadList();
	std::mutex globalListLock;
	std::mutex globalListenerLock;

	ManagedThread::ManagedThread()
	{
		static uInt uniqueID = 0;
		managedID = ++uniqueID;
		isCancelled = false;
		runInterval = std::chrono::milliseconds(1000);
		delayStart = 500;
		currentThread = nullptr;
		automaticallyDelete = false;
	}

	ManagedThread::ManagedThread(const uInt interval, const uInt startDelay, const bool autoDelete)
		: ManagedThread()
	{
		runInterval = std::chrono::milliseconds(interval);
		delayStart = startDelay;
		automaticallyDelete = autoDelete;
	}

	ManagedThread::ManagedThread(const std::string &threadName, const uInt interval, const uInt startDelay,
		const bool runImmediately, const bool autoDelete)
		: ManagedThread(interval, startDelay, autoDelete)
	{
		name = threadName;
		runAtStart = runImmediately;
	}

	ManagedThread::~ManagedThread()
	{
		if (currentThread != nullptr && automaticallyDelete)
		{
			delete currentThread;
			currentThread = nullptr;
		}
	}

	// static methods
	Size ManagedThread::activeThreadCount()
	{
		std::lock_guard<std::mutex> guard(globalListLock);
		return (globalThreadList.size());
	}

	ManagedThread* ManagedThread::activeThreadAt(const int index)
	{
		std::lock_guard<std::mutex> guard(globalListLock);
		return (globalThreadList.at(index));
	}

	ManagedThread* ManagedThread::getActiveThread(const std::string &threadName)
	{
		std::lock_guard<std::mutex> guard(globalListLock);

		for (size_t i = 0; i < globalThreadList.size(); i++)
			if (globalThreadList.at(i)->getName().compare(threadName) == 0)
				return (globalThreadList.at(i));

		return (nullptr);
	}

	bool ManagedThread::cancel(const std::string &threadName, bool removeListeners)
	{
		std::lock_guard<std::mutex> guard(globalListLock);

		for (size_t i = 0; i < globalThreadList.size(); i++)
		{
			if (globalThreadList.at(i)->getName().compare(threadName) == 0)
			{
				globalThreadList.at(i)->cancel();

				if (removeListeners)
				{
					std::lock_guard<std::mutex> guard(globalListenerLock);
					globalThreadList.at(i)->listeners.clear();
				}

				return (true);
			}
		}

		return (false);
	}

	void ManagedThread::cancelAll()
	{
		std::lock_guard<std::mutex> guard(globalListLock);

		for (size_t i = 0; i < globalThreadList.size(); i++)
			globalThreadList.at(i)->cancel();
	}

	void ManagedThread::terminateAll(uInt timeout)
	{
		cancelAll();

		using namespace std::chrono;
		 
		// all threads have been requested to cancel, now give them until timeout to finish what
		// they are doing, if they haven't stopped, call terminate method for each thread

		time_point<system_clock> callTerminate = system_clock::now() + milliseconds(timeout);

		// wait for time out, or until threads close
		while (callTerminate > system_clock::now())
		{
			if (activeThreadCount() == 0)
				break;

			std::this_thread::sleep_for(milliseconds(50));
		}

		// if any of the threads remain active, call their terminate method
		std::lock_guard<std::mutex> guard(globalListLock);

		for (size_t i = 0; i < globalThreadList.size(); i++)
			globalThreadList.at(i)->terminate();
	}

	bool ManagedThread::exists(const std::string &name)
	{
		if (name.size() == 0)
			return (false);

		std::lock_guard<std::mutex> guard(globalListLock);

		for (size_t i = 0; i < globalThreadList.size(); i++)
		{
			if (globalThreadList.at(i)->name == name && !globalThreadList.at(i)->getIsCancelled())
				return (true);
		}

		return (false);
	}

	void ManagedThread::addListener(ThreadNotification *listener)
	{
		std::lock_guard<std::mutex> guard(globalListenerLock);
		listeners.push_back(listener);
	}

	void ManagedThread::removeListener(ThreadNotification *listener)
	{
		std::lock_guard<std::mutex> guard(globalListenerLock);

		if (listeners.size() == 0)
			return;

		for (size_t i = 0; i < listeners.size(); i++)
		{
			if (listeners.at(i) == listener)
			{
				listeners.erase(listeners.begin() + i);
				break;
			}
		}
	}

	void ManagedThread::notifyListeners(const EventType &type, ManagedThread *thread, const std::string &error)
	{
		std::lock_guard<std::mutex> guard(globalListenerLock);

		for (size_t i = 0; i < listeners.size(); i++)
		{
			try
			{
				listeners.at(i)->Notify(type, thread, error);
			}
			catch (...){} // ignore all listener errors
		}
	}

	// private methods
	void ManagedThread::runThread()
	{
		try
		{
			notifyListeners(EventType::Start, this);

			time(&threadStarted);
			using namespace std::chrono;

			// is the start being delayed
			if (delayStart > 0)
			{
				time_point<system_clock> continueTime = system_clock::now() + milliseconds(delayStart);

				// need to ensure that if the thread is cancelled prior to the thread being
				// run then we provide a mechanism for the thread to close
				while (continueTime > system_clock::now())
				{
					std::this_thread::sleep_for(milliseconds(100));

					if (isCancelled)
						break;
				}
			}

			time_point<system_clock> lastRun = system_clock::now();

			if (runAtStart)
				lastRun -= hours(24);
					
			notifyListeners(EventType::Run, this);

			while (true) // always loop
			{
				try
				{
					// are we terminating?
					if (isTerminated)
						return;

					//have we been asked to cancel the thread?
					if (isCancelled)
					{
						notifyListeners(EventType::Cancelled, this);
						break;
					}

					duration<system_clock::rep, system_clock::period> timeSinceLastRun = system_clock::now() - lastRun;
					
					// run the thread
					if (timeSinceLastRun > runInterval)
					{
						if (!run())
							break;

						lastRun = system_clock::now();
					}

					// play niceley with everyone else
					std::this_thread::sleep_for(milliseconds(100));
				}
				catch (const std::exception &error)
				{
					notifyListeners(EventType::Error, this, std::string(error.what()));
				}
			}

			isCancelled = true;
		}
		catch (const std::exception &error)
		{
			// notify listeners
			notifyListeners(EventType::Error, this, std::string(error.what()));
		}

		if (isTerminated)
			return;

		notifyListeners(EventType::Stop, this);

		// remove ourself from the global list
		std::lock_guard<std::mutex> guard(globalListLock);

		for (size_t i = 0; i < globalThreadList.size(); i++)
		{
			if (globalThreadList.at(i) == this)
			{
				globalThreadList.erase(globalThreadList.begin() + i);

				if (automaticallyDelete && isDetached && !isJoined)
					delete this;

				break;
			}
		}
	}

	// public methods
	time_t ManagedThread::getTimeCancelled()
	{
		return (cancelledTime);
	}

	// property wrappers
	bool ManagedThread::getIsCancelled()
	{
		return (isCancelled);
	}

	void ManagedThread::setIsTerminated(bool value)
	{
		isTerminated = value;
	}

	time_t ManagedThread::getTimeStarted()
	{
		return (threadStarted);
	}

	void ManagedThread::setPriority(const ThreadPriority &value)
	{
		if (!currentThread)
			return;

		if (SetThreadPriority(currentThread->native_handle(), static_cast<int>(priority)))
			priority = value;
	}

	ThreadPriority ManagedThread::getPriority()
	{
		return (priority);
	}

	void ManagedThread::start()
	{
		start(ThreadPriority::BelowNormal);
	}

	void ManagedThread::start(const ThreadPriority &priority, bool autoDetach)
	{
		isCancelled = false;
		isTerminated = false;
		currentThread = new std::thread(&ManagedThread::runThread, this);
		SetThreadPriority(currentThread->native_handle(), static_cast<int>(priority));
		std::lock_guard<std::mutex> guard(globalListLock);
		globalThreadList.push_back(this);

		if (autoDetach)
		{
			currentThread->detach();
			isDetached = true;
		}
	}

	bool ManagedThread::isRunning()
	{
		return (currentThread != nullptr);
	}

	// methods
	void ManagedThread::cancel()
	{
		if (!currentThread)
			return;

		isCancelled = true;
		time(&cancelledTime);
	}

	void ManagedThread::join()
	{
		if (!joinable())
			return;

		isJoined = true;
		currentThread->join();
	}

	bool ManagedThread::joinable()
	{
		if (!currentThread)
			return (false);

		return (currentThread->joinable());
	}

	void ManagedThread::detach()
	{
		if (!currentThread)
			return;

		isDetached = true;
		currentThread->detach();
	}

	std::string ManagedThread::getName()
	{
		return (name);
	}
}