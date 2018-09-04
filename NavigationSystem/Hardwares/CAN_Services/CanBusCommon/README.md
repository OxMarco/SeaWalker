# Commons Library #
This library contains code and definitions that are used from both Arduino and Raspberry Pi

## Important Info ##

* If you change any definitions you will have to recompile all code for both Arduino and RPI to get the updates up and running.

## Can message info ##
* To create a brand new Can message the following files are what should be changed

* Add the message id to the canbus_id_defs.h

* Add any new error messages to the canbus_error_defs.h

* Add new data interval values and data size constants to the canbus_datamappings_defs.h


## Can message basics ##

* Below example shows how to encode/decode data in a canmsg.

* NOTE: if you want to handle exact integer values the functions encodeMessage(LENGTH, DATA) and getData(*DATA, LENGTH) should be used instead. Further documentation exist in the CanMessageHandler.h file

```c++
const int MESSAGE_ID = 435;

const int DATA_INTERVAL_MIN = 0;
const int DATA_INTERVAL_MAX = 13;
const int DATASIZE_IN_BYTES = 2; // data will be encoded to this datasize

CanMessageHandler messageHandler(MESSAGE_ID);

float actualData = 4.56;

/*
This will map the data 4.56 to the interval 0 - 13 by using 2 bytes of the CanMsg
*/
boolean successful = messageHandler.encodeMappedMessage(DATASIZE_IN_BYTES, actualData, DATA_INTERVAL_MIN, DATA_INTERVAL_MAX);
CanMsg message = messageHandler.getMessage();


/*
Get data from message
*/

CanMessageHandler messageBackHandler(message);
float backData = 0;

boolean successful = messageBackHandler.getMappedData(&backData, DATASIZE_IN_BYTES, DATA_INTERVAL_MIN, DATA_INTERVAL_MAX);

// backData should now be 4.56

```


