//============================================================================
// Name        : main.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <string>
#include <cstring>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <array>
#include <limits>
#include <atomic>
#include <cstdint>

#include <stdarg.h>

using namespace std;


#define PERFORMANCE 0
#define PERFORMANCE_LOOPS (1000*1000*1000)
#define EXAMPLE 9

#if EXAMPLE != 6
#include "Lock.h"
#include "CyclicBuffer.h"
#include "CyclicBufferSimple.h"
#include "Stack.h"
#include "Memory.h"
#include "HardwareC.h"
#include "Hardware.h"
#include "Timers.h"
#endif

#if EXAMPLE == 1

/**
 * Dummy lock
 */
class LockDummySimple {
public:

    LockDummySimple() {
        cout << "Locked context" << endl;
    }

    ~LockDummySimple() {
        cout << "Lock is freed" << endl;
    }

protected:

private:

};// class LockDummy

/*
 * Output of this code is going to be
 * Locked context
 * Lock is freed
 */
int main() {
#if (__cplusplus >= 201103) // use "auto" if C++11 or better
    auto myDummyLock = LockDummySimple();
#else
    LockDummySimple myDummyLock = LockDummySimple();
#endif
    return 0;
}

#endif // EXAMPLE == 1

#if EXAMPLE == 2

/**
 * Dummy lock
 */
class LockDummySimple {
public:

    LockDummySimple() {
        cout << "Locked context" << endl;
    }

    ~LockDummySimple() {
        cout << "Lock is freed" << endl;
    }

protected:

private:

};// class LockDummySimple

/*
 * Output of this code is going to be
 * Locked context
 * Lock is freed
 * End of main
 */
int main() {
    {
        auto myDummyLock = LockDummySimple();
    }

    cout << "End of main" << endl;
    return 0;
}

#endif // EXAMPLE == 2

/**
 * Function returns number of elements in the cyclic buffer.
 * Compiler will fail if the value can not be calculated in compilation time.
 */
constexpr size_t calculateCyclicBufferSize() {
    return 10;
}

static CyclicBufferSimple<uint8_t, LockDummy, calculateCyclicBufferSize()> myCyclicBufferSimple;

int mainExample5() {
    for (int i : { 1, 3, 11 }) {
        myCyclicBufferSimple.add(i);
    }

    uint8_t val;
    while (myCyclicBufferSimple.remove(val)) {
        cout << (int) val << endl;
    }
    return 0;
}


#if EXAMPLE == 5
#include "CyclicBufferC.h"

int mainExample6() {
    for (int i = 0;i < 4;i++) {
        CyclicBufferAdd(&myCyclicBuffer, i);
    }

    uint8_t val;
    while (CyclicBufferRemove(&myCyclicBuffer, &val)) {
        cout << (int) val << endl;
    };
    return 0;
}
#endif



static CyclicBuffer<uint_fast8_t, LockDummy, calculateCyclicBufferSize()> myCyclicBuffer;

static uint32_t myDynamicCyclicBufferData[calculateCyclicBufferSize()];
CyclicBufferDynamic<uint32_t, LockDummy> myDynamicCyclicBuffer(calculateCyclicBufferSize(), &myDynamicCyclicBufferData);

int mainExample7() {
    // I want to inspect assembler code generated by the C++ compiler
    myCyclicBuffer.add(0);
    // I demonstrate here a range based loop from C++11
    for (int i : { 1, 2, 3 }) {
        myCyclicBuffer.add(i);
    }

    while (!myCyclicBuffer.isEmpty()) {
        uint8_t val;
        myCyclicBuffer.remove(val);
        cout << (int) val << endl;
    }
    return 0;
}

constexpr size_t calculateStackSize() {
    return 10;
}

typedef uint8_t DataBlock[64];

static Stack<DataBlock, LockDummy, calculateStackSize()> myMemoryPool;

int mainExample8() {
    DataBlock dummyDataBlock[10];

    myMemoryPool.push(&(dummyDataBlock[0]));
    (dummyDataBlock[0])[0] = 0;

    for (int i = 1;i < 10;i++) {
        myMemoryPool.push(&(dummyDataBlock[i]));
        (dummyDataBlock[i])[0] = i;
    }

    while (!myMemoryPool.isEmpty()) {
        DataBlock* dummyDataBlockRes;
        myMemoryPool.pop(&dummyDataBlockRes);
        cout << (int) (*dummyDataBlockRes)[0] << endl;
    }
    return 0;
}


static uint8_t dmaMemoryDummy[512];
static MemoryRegion dmaMemoryRegion("dmaMem", (uintptr_t)dmaMemoryDummy, sizeof(dmaMemoryDummy));

static_assert((sizeof(dmaMemoryDummy) >= MemoryAllocatorRaw::predictMemorySize(63, 3, 2)), "DmaMemoryDummy region is not large enough");
static MemoryAllocatorRaw dmaAllocator(dmaMemoryRegion, 63, 3, 2);

static MemoryPoolRaw<LockDummy, 7> dmaPool("dmaPool", dmaAllocator);

static int mainExample9() {

    uint8_t* block;
    cout << "base=" << reinterpret_cast<uintptr_t>(dmaMemoryDummy) << endl;
    bool res;
    for (int i = 0;i < 5;i++)
    {
        res = dmaPool.allocate(&block);
        if (res) {
            cout << "\t" << i << " block=" << reinterpret_cast<uintptr_t>(block) << endl;
            dmaPool.free(block);
        }
    }

    return 0;
}


#ifdef REAL_HARDWARE
static struct PIO *pios = (PIO*)0xFFFFF200;
#else
static struct PIO pioDummy[5];
static struct PIO *pios = pioDummy;
#endif


static void enableOutput(PIO_NAME name, int pin, int value) {
    struct PIO *pio = &pios[name];
    uint32_t mask = 1 << pin;
    if (value) {
        pio->PIO_SODR = mask;
    }
    else {
        pio->PIO_CODR = mask;
    }
    pio->PIO_PER = mask;
    pio->PIO_OER = mask;
}

static void mainExample10() {
    enableOutput(PIO_A, 2, 1);
}


class HardwarePIO : HardwareModule {
public:
    HardwarePIO(const uintptr_t address) {
        interface = (struct Interface*)address;
    }
    ~HardwarePIO() {}

    struct Interface {
        HardwareRegister32WO PIO_PER  ;
        HardwareRegister32WO PIO_PDR  ;
        HardwareRegister32RO PIO_PSR  ;
        HardwareRegister32NotUsed RESERVED ;
        HardwareRegister32WO PIO_OER  ;
        HardwareRegister32WO PIO_ODR  ;
        HardwareRegister32RO PIO_OSR  ;
        HardwareRegister32NotUsed RESERVED ;
        HardwareRegister32NotUsed RESERVED ;
        HardwareRegister32NotUsed RESERVED ;
        HardwareRegister32NotUsed RESERVED ;
        HardwareRegister32NotUsed RESERVED ;
        HardwareRegister32WO PIO_SODR ;
        HardwareRegister32WO PIO_CODR ;
    };
    //static_assert((sizeof(struct Interface) == (14*sizeof(uint32_t))), "struct interface is of wrong size, broken alignment?");
    enum Name {A, B, C, D, E, F, LAST};

    inline Interface& getInterface(Name name) const {return interface[name];};

    inline void enableOutput(Name name, int pin, int value);
protected:
    struct Interface* interface;
};

inline void HardwarePIO::enableOutput(Name name, int pin, int value)
{
    struct Interface& interface = getInterface(name);
    uint32_t mask = 1 << pin;
    if (value) {
        interface.PIO_SODR = mask;
    }
    else {
        interface.PIO_CODR = mask;
    }
    interface.PIO_PER = mask;
    interface.PIO_OER = mask;
}

static HardwarePIO hardwarePIO(reinterpret_cast<uintptr_t>(pioDummy));

static void mainExample11() {
    hardwarePIO.enableOutput(HardwarePIO::A, 2, 1);
    //uint32_t per = hardwarePIO.getInterface(HardwarePIO::A).PIO_PER;
    cout << pioDummy[0].PIO_SODR << endl;
    cout << pioDummy[0].PIO_CODR << endl;
    cout << pioDummy[0].PIO_PER << endl;
    cout << pioDummy[0].PIO_OER << endl;
}

static const char *helloWorldStr = 0;

#if 0
class HelloWorld {
public:
    HelloWorld() {
        if (helloWorldStr == 0)
            cout << "Hello, world!" << endl;
        else
            cout << helloWorldStr << endl;
    }
};
#endif

class HelloWorld {
public:
    HelloWorld() {
        printHello();
    }
protected:
    void printHello() {
        cout << "Hello, world!" << endl;
    }
};

static HelloWorld helloWorld;

int mainExample12()
{
    cout << "Hello from main()!" << endl;
    return 0;
}

static void mainExpirationHandler(const Timer& timer) {
    TimerID timerId = timer.getId();
    uintptr_t data = timer.getApplicationData();
    cout << "Expired id=" << timerId << ",appdata=" << data << endl;
}


TimerLockDummy timerLock;
static TimerList timerList(3, 3, mainExpirationHandler, timerLock);
int mainExample13()
{
    SystemTime currentTime = 0;
    for (int i = 0;i < 3;i++) {
        SystemTime nearestExpirationTime;
        TimerError err = timerList.startTimer(currentTime, nearestExpirationTime, i);
        if (err == TimerError::Ok) {
            cout << "nearestExpirationTime=" << nearestExpirationTime << endl;
        }
        else {
            cout << "timer start failed for timer " << i << endl;
        }
    }

    timerList.processExpiredTimers(3);
    return 0;
}

template<const uint32_t N> struct factorial
{
    static constexpr uint32_t value = N * factorial<N - 1>::value;
};

template<>struct factorial<0>
{
    static constexpr uint32_t value = 1;
};

static const char *LOG_LEVEL_NAME[] = {"INFO", "ERROR"};

#define LOG_INFO(fmt, ...) log_print(__LINE__, LOG_LEVEL_INFO, fmt, ##__VA_ARGS__ )
#define LOG_ERROR(fmt, ...) log_print(__LINE__, LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__ )

static inline void log_print(int line, int level, const char *fmt, ...)
{
    va_list ap;

    printf("%s: line=%d, msg=", LOG_LEVEL_NAME[level], line);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}


#undef LOG_INFO
#undef LOG_ERROR
#if 0
template <int Level> class Log {
public:
    Log(int line, const char *fmt, ...) {
        va_list ap;

        printf("%s: line=%d, msg=", LOG_LEVEL_NAME[Level], line);
        va_start(ap, fmt);
        vprintf(fmt, ap);
        va_end(ap);
    }
};
#endif

#define LOG_INFO(fmt, ...) Log<LOG_LEVEL_INFO>(__LINE__, fmt, ##__VA_ARGS__ )
#define LOG_ERROR(fmt, ...) Log<LOG_LEVEL_ERROR>(__LINE__, fmt, ##__VA_ARGS__ )
#undef LOG_INFO
#undef LOG_ERROR


class Log {
public:
    Log(const char *level) : level(level) {}

    void print(int line, const char *fmt, ...) const {
        va_list ap;

        printf("%s: line=%d, msg=", level, line);
        va_start(ap, fmt);
        vprintf(fmt, ap);
        va_end(ap);
    }
protected:
    const char *level;
};

const Log LogInfo("INFO");
const Log LogError("ERROR");
#define LOG_INFO(fmt, ...) LogInfo.print(__LINE__, fmt, ##__VA_ARGS__ )
#define LOG_ERROR(fmt, ...) LogError.print(__LINE__, fmt, ##__VA_ARGS__ )

void testLog(void) {
    LOG_INFO("This is info %d", 1);
    LOG_ERROR("This is error %d", 2);
}

int sum()
{
    return 0;
}
#undef LOG_INFO
#undef LOG_ERROR

template<typename ... Types>
int sum (int first, Types ... rest)
{
    return first + sum(rest...);
}

const int SUM = sum(1, 2, 3, 4, 5);




void sendData(const int *data, int count) {
    for (int i = 0;i < count;i++) {
        cout << data[i] << " ";
    }
}

template<int MAX_ARGUMENTS_COUNT> class BinaryLog {
public:
    BinaryLog(int fileId, int line, int count, ...) {
        const int HEADER_SIZE = 3;
        int args[MAX_ARGUMENTS_COUNT+HEADER_SIZE];

        if (count > MAX_ARGUMENTS_COUNT) {
            count = MAX_ARGUMENTS_COUNT;
        }
        args[0] = fileId;
        args[1] = line;
        args[2] = count;
        va_list ap;
        va_start(ap, count);
        int arguments = count+HEADER_SIZE;
        for (int j=HEADER_SIZE; j < arguments; j++) {
            args[j] = va_arg(ap, int);
        }
        va_end(ap);
        sendData(args, arguments);
    }
};

#define ARGUMENTS_COUNT(...)  (sizeof((int[]){__VA_ARGS__})/sizeof(int))
#define LOG_INFO(fmt, ...) BinaryLog<3>(FILE_ID, __LINE__, ARGUMENTS_COUNT(__VA_ARGS__), __VA_ARGS__ )
#define LOG_ERROR(fmt, ...) BinaryLog<3>(FILE_ID, __LINE__, ARGUMENTS_COUNT(__VA_ARGS__), __VA_ARGS__ )

constexpr int hashData(const char* s, int accumulator) {
    return *s ? hashData(s + 1, (accumulator << 1) | *s) : accumulator;
}

constexpr int hashMetafunction(const char* s) {
    return hashData(s, 0);
}

constexpr int FILE_ID = hashMetafunction(__FILE__);

void testBinaryLog(void) {
    LOG_INFO("This is info %d %d", 1, 2);
    LOG_ERROR("This is error %d %d %d", 0, 1, 2);
}

int main()
{
    testBinaryLog();
    return 0;
}
