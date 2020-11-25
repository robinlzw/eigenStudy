#ifndef __EASPRT_VSHARED_H__
#define __EASPRT_VSHARED_H__
 
#include <assert.h>

#define VD_DECLARE_NO_COPY_TEMPLATE_CLASS(classname, arg)  \
        private:                                              \
            classname(const classname<arg>&);                 \
            classname& operator=(const classname<arg>&)

#define VD_DECLARE_NO_COPY_CLASS(classname)      \
    private:                                    \
        classname(const classname&);            \
        classname& operator=(const classname&)


template <class T>
class VScopedArray
{
public:
    typedef T element_type;

    explicit VScopedArray(T * array = NULL) : m_array(array) { }

    ~VScopedArray() { delete [] m_array; }

    // test for pointer validity: defining conversion to unspecified_bool_type
    // and not more obvious bool to avoid implicit conversions to integer types
    typedef T *(VScopedArray<T>::*unspecified_bool_type)() const;
    operator unspecified_bool_type() const
    {
        return m_array ? &VScopedArray<T>::get : NULL;
    }

    void reset(T *array = NULL)
    {
        if ( array != m_array )
        {
            delete [] m_array;
            m_array = array;
        }
    }

    T& operator[](size_t n) const { return m_array[n]; }

    T *get() const { return m_array; }

    void swap(VScopedArray &other)
    {
        T * const tmp = other.m_array;
        other.m_array = m_array;
        m_array = tmp;
    }

private:
    T *m_array;

private : // ·ÀÖ¹copy²Ù×÷
    VD_DECLARE_NO_COPY_TEMPLATE_CLASS( VScopedArray , T ) ;
};


namespace std 
{
    template <class T>
    class shared_ptr
    {
    public:
        typedef T element_type;

        explicit shared_ptr( T* ptr = NULL )
            : m_ref(NULL)
        {
            if (ptr)
                m_ref = new reftype(ptr);
        }

        ~shared_ptr()                           { Release(); }
        shared_ptr(const shared_ptr& tocopy)   { Acquire(tocopy.m_ref); }

        shared_ptr& operator=( const shared_ptr& tocopy )
        {
            if (this != &tocopy)
            {
                Release();
                Acquire(tocopy.m_ref);
            }
            return *this;
        }

        shared_ptr& operator=( T* ptr )
        {
            if (get() != ptr)
            {
                Release();
                if (ptr)
                    m_ref = new reftype(ptr);
            }
            return *this;
        }

        // test for pointer validity: defining conversion to unspecified_bool_type
        // and not more obvious bool to avoid implicit conversions to integer types
        typedef T *(shared_ptr<T>::*unspecified_bool_type)() const;
        operator unspecified_bool_type() const
        {
            if (m_ref && m_ref->m_ptr)
                return  &shared_ptr<T>::get;
            else
                return NULL;
        }

        T& operator*() const
        {
            assert(m_ref != NULL);
            assert(m_ref->m_ptr != NULL);
            return *(m_ref->m_ptr);
        }

        T* operator->() const
        {
            assert(m_ref != NULL);
            assert(m_ref->m_ptr != NULL);
            return m_ref->m_ptr;
        }

        T* get() const
        {
            return m_ref ? m_ref->m_ptr : NULL;
        }

        void reset( T* ptr = NULL )
        {
            Release();
            if (ptr)
                m_ref = new reftype(ptr);
        }

        bool unique()   const    { return (m_ref ? m_ref->m_count == 1 : true); }
        long use_count() const   { return (m_ref ? (long)m_ref->m_count : 0); }

    private:

        struct reftype
        {
            reftype( T* ptr = NULL, unsigned count = 1 ) : m_ptr(ptr), m_count(count) {}
            T*          m_ptr;
            int m_count;
        }* m_ref;

        void Acquire(reftype* ref)
        {
            m_ref = ref;
            if (ref)
                ref->m_count ++ ;
        }

        void Release()
        {
            if (m_ref)
            {
                m_ref->m_count -- ;
                if (m_ref->m_count == 0)
                {
                    delete m_ref->m_ptr;
                    delete m_ref;
                }
                m_ref = NULL;
            }
        }
    };

    template <class T, class U>
    bool operator == (shared_ptr<T> const &a, shared_ptr<U> const &b )
    {
        return a.get() == b.get();
    }

    template <class T, class U>
    bool operator != (shared_ptr<T> const &a, shared_ptr<U> const &b )
    {
        return a.get() != b.get();
    }
}
#endif //__EASPRT_VSHARED_H__