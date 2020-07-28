#ifndef ENUMSTRINGS_H
#define ENUMSTRINGS_H

#include "src/core/Global.h"

template<typename E>
class EnumStrings {

private:
    static const EnumStrings SINGLETON;

private:
    std::vector<std::pair<E, QString> > m_to_string, m_from_string;

private:
    inline static bool CompareFirst(const std::pair<E, QString>& a, const std::pair<E, QString>& b) { return (a.first < b.first); }
    inline static bool CompareSecond(const std::pair<E, QString>& a, const std::pair<E, QString>& b) { return (a.second < b.second); }

public:
    EnumStrings(std::initializer_list<std::pair<E, QString> > list) {
        m_to_string = m_from_string = list;
        std::sort(m_to_string.begin(), m_to_string.end(), CompareFirst);
        std::sort(m_from_string.begin(), m_from_string.end(), CompareSecond);
    }
    inline static QString ToString(E value) {
        auto it = std::lower_bound(SINGLETON.m_to_string.begin(), SINGLETON.m_to_string.end(), std::make_pair(value, QString()), CompareFirst);
        if(it == SINGLETON.m_to_string.end() || it->first != value) {
            assert(false);
            return QString();
        }
        return it->second;
    }
    inline static E FromString(const QString& string, E fallback) {
        auto it = std::lower_bound(SINGLETON.m_from_string.begin(), SINGLETON.m_from_string.end(), std::make_pair((E) 0, string), CompareSecond);
        if(it == SINGLETON.m_from_string.end() || it->second != string)
            return fallback;
        return it->first;
    }

};

template<typename E>
inline QString EnumToString(E value) { return EnumStrings<E>::ToString(value); }
template<typename E>
inline E StringToEnum(const QString& string, E fallback) { return EnumStrings<E>::FromString(string, fallback); }

#define ENUMSTRINGS(E) template<> const EnumStrings<E> EnumStrings<E>::SINGLETON


#endif // ENUMSTRINGS_H
