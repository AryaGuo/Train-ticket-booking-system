#ifndef SJTU_UTILITY_HPP
#define SJTU_UTILITY_HPP

#include <cstring>
#include <iostream>

namespace sjtu {

    template<class T1, class T2>
    class pair;
    class string;
    class time;

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
    double stringToDouble(string str);

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
        friend std::ostream &operator<<(std::ostream &os, const string &obj);
        friend std::istream &operator>>(std::istream &is, string &obj);

    private:
        char* data;
        int len;
        /*
         * 0-base
         * arrayLength = len + 1 for '\0'
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
            return *this;
        }

        string& operator+=(const string &other) {
            char* tmp = new char[len + other.length() + 1];
            for(int i = 0; i < len; i++)
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
            memcpy(tmp, data, sizeof(char) * (len + 1));
            tmp[len] = ch;
            len++;
            delete data;
            data = tmp;
            return *this;
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
    };

    std::ostream &operator<<(std::ostream &os, const string &obj) {
        os << obj.data;
        return os;
    }

    std::istream &operator>>(std::istream &is, string &obj) {
        is >> obj.data;
        return is;
    }

    class time {
    public:
        int hour, mini;
    public:

        time(): hour(-1), mini(-1) {}
        time(int h, int m): hour(h), mini(m) {}

        bool operator< (const time &other) {
            return hour < other.hour || (hour == other.hour && mini < other.mini);
        }
    };

    std::ostream &operator<<(std::ostream &os, const time &obj) {
        if(obj.hour == -1) {
            os << "xx:xx";
        }
        else {
            if(obj.hour < 10)
                os << '0';
            os << obj.hour << ":";
            if(obj.mini < 10)
                os << '0';
            os << obj.mini;
        }
        return os;
    }

    std::istream &operator>>(std::istream &is, time &obj) {
        string tmp(10);
        is >> tmp;
        if(tmp[0] == 'x')
            obj.hour = obj.mini = -1;
        else {
            obj.hour = (int)stringToDouble(tmp.get(0, 1));
            obj.mini= (int)stringToDouble(tmp.get(3, 4));
        }
        return is;
    }

    double stringToDouble(string str) {
        int len = str.length(), pos = 1;
        double res = 0, base = 1;
        while(str[pos] != '.' && pos < len)
            res = res * 10 + str[pos] - '0', ++pos;
        ++pos;
        while(pos < len) {
            base /= 10;
            res += (str[pos] - '0') * base;
            ++pos;
        }
        return res;
    }
}

#endif
