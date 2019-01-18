#include "PThreadedPlugin.h"

/**
 * This primitive copy the memory from an object to the other.
 * The parameters could be ByteArrays or ExternalAddress.
 *
 * It receives three parameters:
 *
 * - A From
 * - A To
 * - A Size
 */
PrimitiveWithDepth(primitiveCopyFromTo, 1){
	sqInt from;
	sqInt to;
	sqInt size;

	void* fromAddress;
	void* toAddress;

	size = interpreterProxy->stackIntegerValue(0);
	checkFailed();

	to = interpreterProxy->stackObjectValue(1);
	checkFailed();

	from = interpreterProxy->stackObjectValue(2);
	checkFailed();


	fromAddress = getAddressFromExternalAddressOrByteArray(from);
	checkFailed();

	toAddress = getAddressFromExternalAddressOrByteArray(to);
	checkFailed();

	memcpy(toAddress, fromAddress, size);

	primitiveEnd();
}

/*
 * This primitive returns the address of an image object.
 * It fails if the object is not pinned.
 *
 * Receives an OOP as parameter and returns an SmallInteger
 */
Primitive(primitiveGetAddressOfOOP){
	sqInt oop;

	oop = interpreterProxy->stackValue(0);
	checkFailed();

	if(!interpreterProxy->isPinned(oop)){
		interpreterProxy->primitiveFail();
		return;
	}

    primitiveEndReturnInteger(oop + BaseHeaderSize);
}

/*
 * This primitive returns the object in an oop passed as integer.
 */
Primitive(primitiveGetObjectFromAddress){
	sqInt oop;

	oop = interpreterProxy->integerValueOf(interpreterProxy->stackValue(0)) - BaseHeaderSize;
	checkFailed();

    primitiveEndReturn(oop);
}
