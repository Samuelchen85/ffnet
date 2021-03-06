#pragma once

#include <boost/thread/mutex.hpp>
#include <boost/noncopyable.hpp>
namespace ffnet
{
    template< class Ty_>
    class DSWMutex : public boost::noncopyable
    {
    public :
        Ty_ &        D(){return m_oContent;};
        boost::mutex& mutex(){return m_oMutex;};

    protected:
        boost::mutex        m_oMutex;
        Ty_                m_oContent;
    };//end class DSWMutex
}//end namespace utl;
