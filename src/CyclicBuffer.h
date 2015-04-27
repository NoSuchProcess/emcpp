#pragma once

class CyclicBufferBase {

public:
    bool isEmpty() {
        bool res = (this->head == this->tail);
        return res;
    }

    bool isFull() {
        size_t tail = increment(this->tail);
        bool res = (this->head == tail);
        return res;
    }

protected:
    CyclicBufferBase(size_t size) {
        this->size = size;
        this->head = 0;
        this->tail = 0;
    }

    void errorOverflow() {
    }

    void errorUnderflow() {
    }

    size_t increment(size_t index) {
        if (index < this->size) {
            return (index + 1);
        } else {
            return 0;
        }
    }

    size_t head;
    size_t tail;
    size_t size;

};// CyclicBufferBase

template<typename ObjectType, typename Lock, std::size_t Size> class CyclicBuffer: public CyclicBufferBase {
public:

    CyclicBuffer() :
        CyclicBufferBase(Size) {
        static_assert(std::numeric_limits<ObjectType>::is_integer, "CyclicBuffer is intended to work only with integer types");
    }

    ~CyclicBuffer() {
    }

    inline bool add(const ObjectType object) {
        Lock();
        if (!isFull()) {
            data[this->tail] = object;
            this->tail = increment(this->tail);
            return true;
        } else {
            errorOverflow();
            return false;
        }

    }

    inline bool remove(ObjectType &object) {
        Lock();
        if (!isEmpty()) {
            object = data[this->head];
            this->head = this->increment(this->head);
            return true;
        } else {
            errorUnderflow();
            return false;
        }
    }

private:

    ObjectType data[Size + 1];
};// class CyclicBuffer
