#pragma once
#include "merlin/core/core.h"

#include <vector>
#include <memory>
#include <cstdint>
#include <cassert>

namespace Merlin {
    class DataStream {
    public:
        typedef std::vector<uint8_t> Storage;
        enum ByteOrder { BigEndian, LittleEndian };
        enum Status { Ok, ReadPastEnd };
    private:
        std::shared_ptr<const Storage> m_ptr;
        const Storage* m_data;
        size_t m_idx;
        Status m_status;
        ByteOrder m_byteOrder, m_systemOrder;

        static ByteOrder systemByteOrder();
        bool has(size_t count) const;
        template <typename T> DataStream& read(T& i);
    public:

        DataStream(const std::vector<uint8_t>* data) :
            m_data(data), m_idx(0), m_status(Ok),
            m_byteOrder(BigEndian), m_systemOrder(systemByteOrder()) {}
        DataStream(std::shared_ptr<Storage> data) :
            m_ptr(data), m_data(m_ptr.get()), m_idx(0), m_status(Ok),
            m_byteOrder(BigEndian), m_systemOrder(systemByteOrder()) {}
        DataStream(std::unique_ptr<Storage>&& data) :
            m_ptr(data.release()), m_data(m_ptr.get()), m_idx(0), m_status(Ok),
            m_byteOrder(BigEndian), m_systemOrder(systemByteOrder()) {}
        DataStream(Storage&& data) :
            m_ptr(new Storage(std::move(data))), m_data(m_ptr.get()),
            m_idx(0), m_status(Ok), m_byteOrder(BigEndian), m_systemOrder(systemByteOrder()) {}

        bool atEnd() const;

        DataStream& operator>>(int8_t& i);
        DataStream& operator>>(int16_t& i);
        DataStream& operator>>(int32_t& i);
        DataStream& operator>>(uint8_t& i);
        DataStream& operator>>(uint16_t& i);
        DataStream& operator>>(uint32_t& i);
        void setByteOrder(ByteOrder b);
        ByteOrder byteOrder() const;
    };
}