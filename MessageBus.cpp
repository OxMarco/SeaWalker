/****************************************************************************************
 *
 * File:
 * 		MessageBus.cpp
 *
 * Purpose:
 *		The message bus manages message distribution to nodes allowing nodes to
 *		communicate with one another.
 *
 ***************************************************************************************/

#include "MessageBus.h"
#include "logger/Logger.h"
#include <sys/time.h>

// For std::this_thread
#include <chrono>
#include <thread>


#define SLEEP_TIME_MS	50


MessageBus::MessageBus()
	:m_Running(false)
{
	m_FrontMessages = new std::queue<Message*>();
	m_BackMessages = new std::queue<Message*>();
}

MessageBus::~MessageBus()
{
	for (auto it = m_RegisteredNodes.begin(); it != m_RegisteredNodes.end(); ++it) {
    	delete *it;
	}

	delete m_FrontMessages;
	delete m_BackMessages;
}

void MessageBus::registerNode(Node* node)
{
	if(not m_Running)
	{
		// Don't care about the return type, only want to register the node.
		getRegisteredNode(node);
	}
}

void MessageBus::registerNode(Node* node, MessageType msgType)
{
	if(not m_Running)
	{
		RegisteredNode* regNode = getRegisteredNode(node);
		regNode->subscribe(msgType);
	}
}

void MessageBus::sendMessage(Message* msg)
{
	if(msg != NULL)
	{
		m_FrontQueueMutex.lock();
		m_FrontMessages->push(msg);
		logMessageReceived(msg);
		m_FrontQueueMutex.unlock();
	}
}

void MessageBus::run()
{
	// Prevent nodes from being registered now
	m_Running = true;

	startMessageLog();

	while(m_Running)
	{
		// TODO - Jordan: Wake up only when messages have been pushed into the queue.

		std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME_MS));

		// If we have messages, flip the two queues and begin processing messages
		if(m_FrontMessages->size() > 0)
		{
			std::queue<Message*>* tmpPtr;

			m_FrontQueueMutex.lock();
			
			tmpPtr = m_FrontMessages;
			m_FrontMessages = m_BackMessages;

			m_FrontQueueMutex.unlock();

			m_BackMessages = tmpPtr;

			processMessages();
		}
	}
}

MessageBus::RegisteredNode* MessageBus::getRegisteredNode(Node* node)
{
	for(auto regNode : m_RegisteredNodes)
	{
		if(regNode->nodePtr == node)
		{
			return regNode;
		}
	}

	Logger::info("New node registered");
	RegisteredNode* newRegNode = new RegisteredNode(node);
	m_RegisteredNodes.push_back(newRegNode);
	return newRegNode;
}

void MessageBus::processMessages()
{
	while(m_BackMessages->size() > 0)
	{
		Message* msg = m_BackMessages->front();

		logMessage(msg);

		for(auto node : m_RegisteredNodes)
		{
			// Distribute to everyone interested
			if(msg->destinationID() == NodeID::None)
			{
				if(node->isInterested( msg->messageType() ))
				{
					node->nodePtr->processMessage(msg);
					logMessageConsumer(node->nodePtr->nodeID());
				}
			}
			// Distribute to the node the message is directed at then move onto the next message
			else
			{
				if(node->nodePtr->nodeID() == msg->destinationID())
				{
					node->nodePtr->processMessage(msg);
					logMessageConsumer(node->nodePtr->nodeID());
					continue;
				}
			}
		}

		m_BackMessages->pop();
		delete msg;
	}
}

void MessageBus::startMessageLog()
{
#ifdef LOG_MESSAGES
	m_LogFile = new std::ofstream("./Messages.log", std::ios::out | std::ios::trunc);
	if(m_LogFile->is_open())
	{
		Logger::info("Message log file created!");
	}
	else
	{
		Logger::error("Message log file not created!");
		delete m_LogFile;
		m_LogFile = NULL;
	}
#endif
}

void MessageBus::logMessageReceived(Message* msg)
{
#ifdef LOG_MESSAGES
	msg->timeReceived = Logger::unixTime();
#endif
}

void MessageBus::logMessage(Message* msg)
{
#ifdef LOG_MESSAGES
	if(m_LogFile != NULL)
	{
		char buff[256];
		char timeNow[14];
		char timeReceived[14];

		messageTimeStamp(Logger::unixTime(), timeNow);
		messageTimeStamp(msg->timeReceived, timeReceived);

		snprintf(buff, 256, "[%s ID=%d] Type=%s(%d) SourceID=%d Destination=%d Received=%s", timeNow, msg->messageID, msgToString(msg->messageType()).c_str(), msg->messageType(), msg->sourceID(), msg->destinationID(), timeReceived);
		*m_LogFile << buff << "\n";
		m_LogFile->flush();
	}
#endif
}

void MessageBus::logMessageConsumer(NodeID id)
{
#ifdef LOG_MESSAGES
	if(m_LogFile != NULL)
	{
		char buff[256];
		char timeNow[14];

		messageTimeStamp(Logger::unixTime(), timeNow);

		snprintf(buff, 256, "\t%s Consumed by Node: %s(%d)", timeNow, nodeToString(id).c_str(), id);
		*m_LogFile << buff << "\n";
		m_LogFile->flush();
	}
#endif
}

void MessageBus::messageTimeStamp(unsigned long unixTime, char* buffer)
{
	char buff[9];
	time_t unix_time = (time_t)unixTime;
	strftime(buff, 9, "%H:%M:%S", gmtime(&unix_time));

	// Get Milliseconds
	timeval curTime;
	gettimeofday(&curTime, NULL);
	int milli = curTime.tv_usec / 1000;

	sprintf(buffer, "%s:%d", buff, milli);
}
