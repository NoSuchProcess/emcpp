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



static CyclicBuffer<uint8_t, LockDummy, calculateCyclicBufferSize()> myCyclicBuffer;

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

/*
0x0000 PIO Enable Register PIO_PER Write-only
0x0004 PIO Disable Register PIO_PDR Write-only
0x0008 PIO Status Register PIO_PSR Read-only
0x000C Reserved
0x0010 Output Enable Register PIO_OER Write-only
0x0014 Output Disable Register PIO_ODR Write-only
0x0018 Output Status Register PIO_OSR Read-only
0x001C Reserved
0x0020 Not used
0x0024 Not used
0x0028 Not used
0x002C Reserved
0x0030 Set Output Data Register PIO_SODR Write-only
0x0034 Clear Output Data Register PIO_CODR Write-only
*/

#define TOKEN_CAT(x, y) x ## y
#define TOKEN_CAT2(x, y) TOKEN_CAT(x, y)
#define RESERVED TOKEN_CAT2(reserved, __COUNTER__)


struct __attribute__ ((__packed__)) PIO {
    volatile uint32_t PIO_PER  ;
    volatile uint32_t PIO_PDR  ;
    volatile uint32_t PIO_PSR  ;
    volatile uint32_t RESERVED ;
    volatile uint32_t PIO_OER  ;
    volatile uint32_t PIO_ODR  ;
    volatile uint32_t PIO_OSR  ;
    volatile uint32_t RESERVED ;
    volatile uint32_t RESERVED ;
    volatile uint32_t RESERVED ;
    volatile uint32_t RESERVED ;
    volatile uint32_t RESERVED ;
    volatile uint32_t PIO_SODR ;
    volatile uint32_t PIO_CODR ;
};

#ifdef REAL_HARDWARE
static struct PIO *pios = (PIO*)0xFFFFF200;
#else
static struct PIO pioDummy[5];
static struct PIO *pios = pioDummy;
#endif

typedef enum {
    PIO_A,
    PIO_B,
    PIO_C,
    PIO_D,
    PIO_E,
    PIO_F,
} PIO_NAME;


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

class HardwareModule {
protected:
    HardwareModule(const uintptr_t address): address(address) {}
    ~HardwareModule() {}
    const uintptr_t address;
};

class HardwareRegister32 {
protected:
    volatile uint32_t value;
    inline uint32_t get() const {return value;}
    inline void set(uint32_t value) {this->value = value;}
};
static_assert((sizeof(HardwareRegister32) == sizeof(uint32_t)), "HardwareRegister32 is not 32 bits");



class HardwareRegister32RO : public HardwareRegister32{
public:
    uint32_t operator=(const HardwareRegister32RO& r) const {
        return get();
    }
};
static_assert((sizeof(HardwareRegister32RO) == sizeof(uint32_t)), "HardwareRegister32RO is not 32 bits");



class HardwareRegister32WO : public HardwareRegister32{
public:
    uint32_t operator=(uint32_t value) {
        set(value);
        return value;
    }
};
static_assert((sizeof(HardwareRegister32WO) == sizeof(uint32_t)), "HardwareRegister32WO is not 32 bits");



class HardwareRegister32RW : public HardwareRegister32 {
public:
    uint32_t operator=(const HardwareRegister32RO& r) const {
        return get();
    }
    uint32_t operator=(uint32_t value) {
        set(value);
        return value;
    }
};
static_assert((sizeof(HardwareRegister32RW) == sizeof(uint32_t)), "HardwareRegister32RW is not 32 bits");

class HardwareRegister32NotUsed : HardwareRegister32 {
public:
    HardwareRegister32NotUsed() {}
    ~HardwareRegister32NotUsed() {}
};
static_assert((sizeof(HardwareRegister32NotUsed) == sizeof(uint32_t)), "HardwareRegister32NotUsed is not 32 bits");


class HardwarePIO : HardwareModule {
public:
    HardwarePIO(const uintptr_t address): HardwareModule(address) {}
    ~HardwarePIO() {}

    struct interface {
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
    } interface;
    enum Name {A, B, C, D, E, F};
    static_assert((sizeof(struct interface) == (14*sizeof(uint32_t))), "struct interface is of wrong size, broken alignment?");

    inline void enableOutput(Name name, int pin, int value);

};

inline void HardwarePIO::enableOutput(Name name, int pin, int value)
{
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
}

int main()
{
    mainExample11();
    return 0;
}
