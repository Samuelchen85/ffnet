#pragma once

#include "common.h"

namespace ffnet {
//!Note, we can not use a base class and sub-classes to handle this, because we
//have to use template method archive for different types. Remind that a template
//method can't be a virtual method.
//Also, we can't make marshaler as a template class, because the calling point of
//Archive as parameter is a virtual method. So we can't involve any template parameter
//for Archive.
//Finally, our only choice is to use arch_type and switch-case to gain polymorphism.
    class marshaler {
    public:
        enum marshaler_type {
            seralizer,
            deseralizer,
            length_retriver
        };

        marshaler(const char *buf, size_t len, marshaler_type at);

        marshaler(char *buf, size_t len, marshaler_type at);

        marshaler(marshaler_type at);

#include "common/archive_data.h"

        size_t getLength() { return m_iBase; }

        marshaler_type get_marshaler_type() { return m_iAT; }

        bool is_serializer() const { return m_iAT == seralizer; }

        bool is_deserializer() const { return m_iAT == deseralizer; }

        bool is_lengther() const { return m_iAT == length_retriver; }

        size_t &internal_m_iBase() { return m_iBase; }

        char *internal_m_pWriteBuf() { return m_pWriteBuf; }

        const char *internal_m_pReadBuf() { return m_pReadBuf; }

    protected:
        marshaler_type m_iAT;
        size_t m_iBase;
        char *m_pWriteBuf;
        const char *m_pReadBuf;
        size_t m_iBufLen;
    };//end  class marshaler


    template<class Ty_>
    typename std::enable_if<std::is_arithmetic<Ty_>::value, size_t>::type
    seralize(const Ty_ &val, char *pBuf) {
        std::memcpy(pBuf, (const char *) &val, sizeof(Ty_));
        return sizeof(Ty_);
    }

    template<class Ty_>
    typename std::enable_if<std::is_arithmetic<Ty_>::value, size_t>::type
    deseralize(const char *pBuf, Ty_ &val) {
        std::memcpy((char *) &val, pBuf, sizeof(Ty_));
        return sizeof(Ty_);
    }

    template<class Ty_>
    typename std::enable_if<std::is_arithmetic<Ty_>::value, size_t>::type
    length(const Ty_ &val) {
        return sizeof(Ty_);
    }

}//end namespace ffnet

