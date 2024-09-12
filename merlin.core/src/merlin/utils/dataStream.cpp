#include "glpch.h"
#include "dataStream.h"


namespace Merlin{
    DataStream::ByteOrder  DataStream::systemByteOrder() {
        const uint32_t t = 1;
        return (reinterpret_cast<const uint8_t*>(&t)) ? LittleEndian : BigEndian;
    }

    bool  DataStream::has(size_t count) const { 
        return m_idx + count <= m_data->size(); 
    }

    template <typename T> DataStream& DataStream::read(T& i) {
        if (has(sizeof(T)) && Ok == m_status) {
            T result = *reinterpret_cast<const T*>(&(*m_data)[m_idx]);
            m_idx += sizeof(T);
            if (m_byteOrder != m_systemOrder) {
                T tmp = 0;
                for (uint8_t i = 0; i < sizeof(T); ++i) {
                    tmp = (tmp << 8) | (result & 0xFF);
                    result = result >> 8;
                }
                i = tmp;
            }
            else
                i = result;
        }
        else {
            m_status = ReadPastEnd;
        }
        return *this;
    }

    bool DataStream::atEnd() const { 
        return m_idx == m_data->size(); 
    }

    DataStream& DataStream::operator>>(int8_t& i) {
        return read(i);
    }
    DataStream& DataStream::operator>>(int16_t& i) {
        return read(i);
    }
    DataStream& DataStream::operator>>(int32_t& i) {
        return read(i);
    }
    DataStream& DataStream::operator>>(uint8_t& i) {
        return read(i);
    }
    DataStream& DataStream::operator>>(uint16_t& i) {
        return read(i);
    }
    DataStream& DataStream::operator>>(uint32_t& i) {
        return read(i);
    }

    void DataStream::setByteOrder(ByteOrder b) { m_byteOrder = b; }
    DataStream::ByteOrder DataStream::byteOrder() const { return m_byteOrder; }


}