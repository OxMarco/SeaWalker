/****************************************************************************************
 *
 * File:
 *    CanMessageHandler.cpp
 *
 * Purpose:
 *    The purpose of this class is a unified use of CanMsg handling
 *    from both Arduino and RPI
 *
 * Developer Notes:
 *    There is only 7 bytes of data that can be encoded by using this class,
 *    because the last byte of the CanMsg is reserved for an error message.
 *
 ***************************************************************************************/

#include "CanMessageHandler.h"

CanMessageHandler::CanMessageHandler(CanMsg message) : m_message(message){

}

CanMessageHandler::CanMessageHandler(uint32_t messageId) {
    m_message.id = messageId;
    m_message.header.ide = 0;
    m_message.header.length = 8;
    for(auto& byteData : m_message.data) {
        byteData = 0;
    }

    m_message.data[INDEX_ERROR_CODE] = NO_ERRORS;
}

uint32_t CanMessageHandler::getMessageId() {
    return m_message.id;
}

CanMsg CanMessageHandler::getMessage() {
    return m_message;
}

uint8_t CanMessageHandler::getErrorMessage() {
    return m_message.data[INDEX_ERROR_CODE];
}

void CanMessageHandler::setErrorMessage(uint8_t errorMessage) {
    if(m_message.data[INDEX_ERROR_CODE] == NO_ERRORS) {
        m_message.data[INDEX_ERROR_CODE] = errorMessage;
    }
}

/*
bool getCSData(float *dataToSet, uint8_t targetData) {
    *dataToSet = 0;
    Float16Compressor fltCompressor;
    switch (targetData) {
        case 0: //sensor id/m_element
            *dataToSet = (uint8_t)m_message.data[0:1];
            return true;
        case 1: //current
            *dataToSet = fltCompressor.decompress(m_message.data[6:8]);
            return true;
        case 2: //voltage
            *dataToSet = fltCompressor.decompress(m_message.data[4:6]);
            return true;
        default:
            return false;
    }

}

bool encodeFloatData(uint8_t position, bool isHalfPrecision, float data) {
    if (isHalfPrecision) {
        Float16Compressor fltCompressor;
        uint16_t compressedFloatData = fltCompressor.compress(data);
        // Maybe take care of bad positioning (position should be 2,4,6 in 99% of cases),
        // and not only overflowing.
        if (position + 2 =< 8 and position >= 2) {
            m_message.data[position] = compressedFloatData;
            return true;
        } else {
            setErrorMessage(ERROR_CANMSG_INDEX_OUT_OF_INTERVAL);
            return false;
        }
    } else {
        if (position + 4 =< 8 and position >= 2) {
            m_message.data[position] = data;
            return true;
        } else {
            setErrorMessage(ERROR_CANMSG_INDEX_OUT_OF_INTERVAL);
            return false;
        }
        
    }
    
}

bool encodeCSMessage(int lengthInBytes, float data, uint8_t position) {
// Kind of encode Float Message now, where we choose 16 or 32 bits.
// position is 0 or 1 (from lower bit to higher bit -> 0 is right half), length is 16 or 32.
// If length is 32, position value is not used (but still rise error if not 0 or 1).
    Float16Compressor fltCompressor;
    int bitMaskLeft  = 0xffff0000;
    int bitMaskRight = 0x0000ffff;
    if (encodeFloatData())
    if (position>=2) {
        // Create error message?
        //setErrorMessage(SOMETHING);
    } else {
        if (lengthInBytes==16) {
            uint16_t compressedFloatData = fltCompressor.compress(data);
            if (position==0) {
                m_message.data += 
            }
        } else if (lengthInBytes==32) {
            
        }
    }


    for (int i = 0; i < lengthInBytes; i++) {
      int dataIndex = currentDataWriteIndex + i;
      m_message.data[dataIndex] = (data >> 8 * i) & 0xff;
    }
    currentDataWriteIndex += lengthInBytes;
    return true;
}

bool generateHeader(int msgType) {
    // Add definition of canMsgType or just use the already defined ID defs
    switch (canMsgType) {
        case CURRENT_SENSOR_MSG:
            m_message.header.ide = CURRENT_SENSOR_ID
            m_message.
            return true;
    }
}*/
