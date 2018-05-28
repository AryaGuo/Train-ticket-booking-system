#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <cstring>
#include <iostream>

namespace sjtu {

    template<class T>
    T min(T a, T b) {
        return a < b ? a : b;
    }

    template<class T>
    T max(T a, T b) {
        return a < b ? b : a;
    }

    template<class T>
    void swap(T &a, T &b) {
        T tmp = a;
        a = b;
        b = tmp;
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

        string(const int &len): len(len) {
            data = new char[len + 1];
            memset(data, 0, sizeof(0));
        }

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

        string& operator+=(const string &other) {
            char* tmp = new char[len + other.length() + 1];
            for(int i = 0,; i < len; i++)
                tmp[i] = data[i];
            for(int i = 0, j = len; i < other.len; i++, j++)
                tmp[j] = other[i];
            delete data;
            data = tmp;
            len += other.length();
            return *this;
        }

        string& operator+= (const char &ch) {
            char *tmp = new char[len + 2];
            memcpy(tmp, data, sizeof(data));
            tmp[len] = ch;
            len++;
            delete data;
            data = tmp;
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

        char& operator[] (const int &i) {
            return data[i];
        }

        char operator[] (const int &i) const {
            return data[i];
        }

        ~string() {
            delete data;
        }

        int length() const {
            return len;
        }

        string get(int l, int r) {
            char* res = new char[r - l];
            for(int i = l; i <= r; i++)
                res[i - l] = data[i];
            return string(res);
        }

        std::ostream& operator<< (std::ostream &os, const string &obj) const;
        std::istream& operator>> (std::istream &is, string &obj);

    };

    std::ostream &string::operator<<(std::ostream &os, const string &obj) const {
        os << obj.data;
        return os;
    }

    std::istream &string::operator>>(std::istream &is, string &obj) {
        is >> obj.data;
        return is;
    }

    class time {
    private:
        string hour, mini;
    public:

        time(): hour("xx"), mini("xx") {}
        time(int h, int m) {
            hour = string();
            hour += h / 10 + '0';
            hour += h % 10 + '0';
            mini = string();
            mini += m / 10 + '0';
            mini += m % 10 + '0';
        }

        std::ostream& operator<< (std::ostream &os, const time &obj) const;
        std::istream& operator>> (std::istream &is, time &obj);

    };

    std::ostream &time::operator<<(std::ostream &os, const time &obj) const {
        os << obj.hour << ":" << obj.mini;
        return os;
    }

    std::istream &time::operator>>(std::istream &is, time &obj) {
        string tmp(10);
        is >> tmp;
        hour = tmp.get(0, 1);
        mini = tmp.get(3, 4);
        return is;
    }


}

#endif
