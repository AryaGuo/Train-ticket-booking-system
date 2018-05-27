#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <cstring>
#include <iostream>
//#include <utility>

namespace sjtu {

    template<class T>
    T min(T a, T b) {
        return a < b ? a : b;
    }

    template<class T>
    T max(T a, T b) {
        return a < b ? b : a;
    }

    template<class T1, class T2>
    class pair {
    public:
        T1 first;
        T2 second;
        constexpr pair() : first(), second() {}
        pair(const pair &other) = default;
        pair(pair &&other) = default;
        pair(const T1 &x, const T2 &y) : first(x), second(y) {}
        template<class U1, class U2>
        pair(U1 &&x, U2 &&y) : first(x), second(y) {}
        template<class U1, class U2>
        pair(const pair<U1, U2> &other) : first(other.first), second(other.second) {}
        template<class U1, class U2>
        pair(pair<U1, U2> &&other) : first(other.first), second(other.second) {}
    };

    class string {
    private:
        char* data;
        int len;
        /*
         * 0-base
         * len = length + 1 for'\0'
         */

    public:

        string():data(nullptr), len(0) {}

        string(char* ch){
            len = strlen(ch);
            data = new char[len + 1];
            memcpy(data, ch, sizeof(char) * (len + 1));
        }

        string(const string &other):len(other.len) {
            data = new char[len + 1];
            memcpy(data, other.data, sizeof(char) * (len + 1));
        }

        string& operator=(const string &other) {
            delete data;
            len = other.len;
            data = new char[len + 1];
            memcpy(data, other.data, sizeof(char) * (len + 1));
        }

        bool operator== (const string &other) const{
            if(len != other.len)
                return 0;
            for(int i = 0; i < len; i++)
                if(data[i] != other.data[i])
                    return 0;
            return 1;
        }

        bool operator< (const string &other) const{
            int l = min(len, other.len);
            for(int i = 0; i < l; i++)
                if(data[i] != other.data[i])
                    return data[i] < other.data[i];
            return len < other.len;
        }

        ~string() {
            delete data;
        }

        int length() const{
            return len;
        }

        std::ostream& operator<< (std::ostream &os, const string &obj) const;
        std::istream& operator>> (std::istream &is, string &obj);

    };

    class time {

    };

    std::ostream& string::operator<<(std::ostream &os, const string &obj) const{
        for(int i = 0; i < len; i++)
            os << data[i];
        return os;
    }

    std::istream& string::operator>>(std::istream &is, string &obj) {
        for(int i = 0; i < len; i++)
            is >> data[i];
        return is;
    }

}

#endif
