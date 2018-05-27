//
// Created by 郭文轩 on 2018/5/26.
//

#ifndef TRAIN_TICKET_BOOKING_SYSTEM_TUPLE_HPP
#define TRAIN_TICKET_BOOKING_SYSTEM_TUPLE_HPP

#include <iostream>

namespace sjtu {
    //////////////////////////////////////////////////////////
    template<typename... TList> struct tuple;

    template<> struct tuple<> {};

    typedef tuple<> nulltuple;

    //////////////////////////////////////////////////////////
    template<typename T, typename... TList>
    struct tuple<T, TList...> : public tuple<TList...>
    {
        typedef T value_type;
        typedef tuple<TList...> base_type;
        typedef tuple<T, TList...> this_type;

        tuple(const T& v, const TList&... tails):base_type(tails...),_value(v) {}

        tuple(T&& v, TList&&... tails):base_type(std::move(tails)...), _value(std::forward<T>(v)) {}
        tuple(T&& v, TList&... tails):base_type(std::move(tails)...), _value(std::forward<T>(v)) {}
        tuple(T& v, TList&&... tails):base_type(std::move(tails)...), _value(std::forward<T>(v)) {}

        tuple(const this_type& other):base_type(static_cast<const base_type&>(other)),_value(other._value)
        {}

        tuple(this_type&& other):base_type(std::move(static_cast<base_type&>(other))),_value(std::forward<T>(other._value))
        {}

        const T& head() const { return this->_value; }
        T& head() { return this->_value; }

        this_type& operator=(const this_type& other)
        {
            base_type::operator=(static_cast<const base_type&>(other));
            _value = other._value;
            return *this;
        }

        this_type& operator=(this_type&& other)
        {
            base_type::operator=(std::move(static_cast<base_type&>(other)));
            _value = other._value;
            return *this;
        }


    protected:
        T _value;
    };

    template<typename T>
    struct tuple<T> : public nulltuple
    {
        typedef T value_type;
        typedef nulltuple base_type;
        typedef tuple<T> this_type;

        tuple(const T& v):_value(v) {}
        tuple(T&& v):_value(std::forward<T>(v)) {}

        tuple(const this_type& other):_value(other._value) {}

        tuple(this_type&& other):_value(std::forward<T>(other._value)) {}

        const T& head() const { return this->_value; }
        T& head() { return this->_value; }
        this_type& operator=(const this_type& other)
        {
            _value = other._value;
            return *this;
        }
        this_type& operator=(this_type&& other)
        {
            _value = other._value;
            return *this;
        }

    protected:
        T _value;
    };


    //////////////////////////////////////////////////////////
    template<unsigned int N, typename... TList> struct tuple_at;

    template<unsigned int N, typename T, typename... TList>
    struct tuple_at< N, tuple<T, TList...> >
    {
        typedef typename tuple_at< N-1, tuple<TList...> >::value_type value_type;
        typedef typename tuple_at< N-1, tuple<TList...> >::tuple_type tuple_type;
    };

    template<typename T, typename... TList>
    struct tuple_at< 0, tuple<T, TList...> >
    {
        typedef T value_type;
        typedef tuple<T, TList...> tuple_type;
    };

    template<>
    struct tuple_at<0, nulltuple>
    {
        typedef nulltuple value_type;
        typedef nulltuple tuple_type;
    };

    //////////////////////////////////////////////////////////
    template<unsigned int N, typename... TList>
    constexpr const typename tuple_at<N, tuple<TList...> >::value_type&
    tuple_get(const tuple<TList...>& tuple_)
    {
        typedef tuple<TList...> tuple_type;
        typedef typename tuple_at<N, tuple_type>::tuple_type base_tuple_type;

        return static_cast<const base_tuple_type&>(tuple_).head();
    }

    template<unsigned int N, typename... TList>
    typename tuple_at<N, tuple<TList...> >::value_type&
    tuple_get(tuple<TList...>& tuple_)
    {
        typedef tuple<TList...> tuple_type;
        typedef typename tuple_at<N, tuple_type>::tuple_type base_tuple_type;

        return static_cast<base_tuple_type&>(tuple_).head();
    }
}

#endif //TRAIN_TICKET_BOOKING_SYSTEM_TUPLE_HPP
