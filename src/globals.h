#pragma once
enum OperationSize {
	kByte = 1,
	kWord = 2,
	kLong = 4
};

enum TransferType
{
	kRead,
	kWrite
};

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);               \
	void operator=(const TypeName&)

template <typename T1, typename T2>
struct Pair
{
	T1 first;
	T2 second;
};