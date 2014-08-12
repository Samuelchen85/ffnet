#ifndef _NETWORK_ARCHIVE_ARCHIVE_H_
#define _NETWORK_ARCHIVE_ARCHIVE_H_

#include "common.h"
#include <boost/concept_check.hpp>
#include <boost/utility.hpp>

namespace ffnet
{


namespace archive
{
template<class Ty_>
size_t    seralize(const Ty_ & val, char *pBuf,
                 typename boost::enable_if<boost::is_arithmetic<Ty_> >::type * p = 0)
{
    std::memcpy(pBuf, (const char *)&val, sizeof(Ty_));
    return sizeof(Ty_);
}
template <class Ty_>
size_t    deseralize(const char *pBuf, Ty_ & val,
                   typename boost::enable_if<boost::is_arithmetic<Ty_> >::type * p = 0)
{
    std::memcpy((char *)&val, pBuf, sizeof(Ty_));
    return sizeof(Ty_);
}
template<class Ty_>
size_t    length(const Ty_ & val,
               typename boost::enable_if<boost::is_arithmetic<Ty_> >::type * p = 0)
{
    return sizeof(Ty_);
}


size_t seralize(const String &val, char *pBuf);

size_t    deseralize(const char *pBuf, String &val);

size_t length(const String &val);

template<class Ty_, size_t N>
size_t    seralize(const Ty_ (& val)[N], char *pBuf,
                 typename boost::enable_if<boost::is_arithmetic<Ty_> >::type * p = 0)
{
    size_t offset = seralize(static_cast<size_t>(N), pBuf);
    std::memcpy(pBuf + offset, (const char *)val, sizeof(Ty_)*N);
    return offset + sizeof(Ty_)*N;
}
template <class Ty_, size_t N>
size_t deseralize(const char *pBuf, Ty_ (& val)[N],
                  typename boost::enable_if<boost::is_arithmetic<Ty_> >::type * p = 0)
{
    size_t s;
    size_t offset = deseralize(pBuf, s);
    assert(N == s && "Not same array length");
    std::memcpy((char *)val, pBuf + offset, sizeof(Ty_) *N);
    return sizeof(Ty_) * N + offset;
}
template<class Ty_, size_t N>
size_t    length(const Ty_ (& val)[N],
               typename boost::enable_if<boost::is_arithmetic<Ty_> >::type * p = 0)
{
    return length(static_cast<size_t>(N)) + sizeof(Ty_) * N;
}


template<class Ty_>
size_t seralize(const Ty_ * val, size_t count, char *pBuf,
                typename boost::enable_if<boost::is_arithmetic<Ty_> >::type * p = 0)
{
    std::memcpy(pBuf, (const char *)val, sizeof(Ty_) * count);
    return sizeof(Ty_) * count;
}
template<class Ty_>
size_t deseralize(const char *pBuf, Ty_ *val, size_t count,
                  typename boost::enable_if<boost::is_arithmetic<Ty_> >::type * p = 0)
{
    std::memcpy((char *)val, pBuf , sizeof(Ty_) * count);
    return count * sizeof(Ty_);
}
template<class Ty_>
size_t length(const Ty_ * val, size_t count,
              typename boost::enable_if<boost::is_arithmetic<Ty_> >::type * p = 0)
{
    return count * sizeof(Ty_);
}

}//end namespace archive

//!Note, we can not use a base class and sub-classes to handle this, because we 
//have to use template method archive for different types. Remind that a template 
//method can't be a virtual method.
//Also, we can't make Archive as a template class, because the calling point of
//Archive as parameter is a virtual method. So we can't involve any template parameter
//for Archive.
//Finally, our only choice is to use arch_type and switch-case to gain polymorphism.
class Archive
{
public:
    enum arch_type{
        seralizer,
        deseralizer,
        length_retriver
    };
    Archive(const char *buf,size_t len, arch_type at)
    : m_pReadBuf(NULL)
    , m_iBufLen(len)
    , m_iAT(at)
    , m_iBase(0)
    , m_pWriteBuf(NULL)
    {
        if(m_iAT == deseralizer)
        {
            m_pReadBuf = buf;
        }
        else if(m_iAT == seralizer)
        {
            assert(0);
        }
    }
    Archive(char *buf, size_t len, arch_type at)
    : m_pReadBuf(NULL)
    , m_iBufLen(len)
    , m_iAT(at)
    , m_iBase(0)
    , m_pWriteBuf(buf)
    {
        if(m_iAT == deseralizer)
        {
            m_pReadBuf = buf;
        }
        else if(m_iAT == seralizer)
        {
            m_pWriteBuf = buf;
        }
    }
    Archive(arch_type at)
    : m_pReadBuf(NULL)
    , m_iBufLen(0)
    , m_iAT(at)
    , m_iBase(0)
    , m_pWriteBuf(NULL)
    {
        assert(m_iAT == length_retriver);
    }
    
#define ARCH_ONE(val) \
        switch(m_iAT) \
        { \
            case seralizer:\
                m_iBase += archive::seralize(val, m_pWriteBuf + m_iBase); \
                break; \
            case deseralizer: \
                m_iBase += archive::deseralize(m_pReadBuf + m_iBase, val); \
                break; \
            case length_retriver: \
                m_iBase += archive::length(val); \
                break; \
        }
        
#define ARCH_TWO(val, len) \
        switch(m_iAT) \
        { \
            case seralizer:\
                m_iBase += archive::seralize(val,len, m_pWriteBuf + m_iBase); \
                break; \
            case deseralizer: \
                m_iBase += archive::deseralize(m_pReadBuf + m_iBase, val, len); \
                break; \
            case length_retriver: \
                m_iBase += archive::length(val, len); \
                break; \
        }        
    template<class Ty_>
    void        archive(Ty_ & val,typename boost::enable_if<boost::is_arithmetic<Ty_> >::type * p = 0)
    {
        ARCH_ONE(val)
    }
    
    
    template <class Ty_, size_t N>
    void archive(Ty_ (& val)[N],
                  typename boost::enable_if<boost::is_arithmetic<Ty_> >::type * p = 0)
    {
        ARCH_ONE(val)    
    }
    
    void archive(String &val)
    {
        ARCH_ONE(val)
    }
    
    template<class Ty_>
    void archive(Ty_ * val, size_t  count,
              typename boost::enable_if<boost::is_arithmetic<Ty_> >::type * p = 0)
    {
        ARCH_TWO(val, count)
    }
    size_t     getLength(){return m_iBase;}
    arch_type        getArTy(){return m_iAT;}
        bool            is_serializer() const {
          return m_iAT == seralizer;}
        bool            is_deserializer() const{return m_iAT == deseralizer;}
        bool            is_lengther()  const{return m_iAT == length_retriver;}
#undef ARCH_ONE
#undef ARCH_TWO
protected:
        arch_type            m_iAT;
        size_t             m_iBase;
        char *            m_pWriteBuf;
        const char *        m_pReadBuf;
        size_t            m_iBufLen;
};//end  class Archive

}//end namespace ffnet
#endif

