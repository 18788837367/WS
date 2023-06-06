//
//  TVecHelpers.h
//  WS
//
//  Created by zrf on 2023/3/8.
//

#ifndef TVecHelpers_h
#define TVecHelpers_h

#include <stdint.h>
#include <sys/types.h>
#include <cmath>
#include <type_traits>

namespace ME {
    
template<typename U>
inline constexpr U min(U a, U b) noexcept {
    return a < b ? a : b;
}

template<typename U>
inline constexpr U max(U a, U b) noexcept {
    return a > b ? a : b;
}

//std::declval是返回该类型的右值引用
//decltype自动推导类型
template<typename T, typename U>
struct arithmetic_result {
    using type = decltype(std::declval<T>() + std::declval<U>());
};

template<typename T, typename U>
using arithmetic_result_t = typename arithmetic_result<T, U>::type;


template<typename T>
struct is_arithmetic : std::integral_constant<bool, std::is_integral<T>::value || std::is_floating_point<T>::value> {
};

template<typename A, typename B = int, typename C = int, typename D = int>
using enable_if_arithmetic_t = std::enable_if_t<
        is_arithmetic<A>::value &&
        is_arithmetic<B>::value &&
        is_arithmetic<C>::value &&
        is_arithmetic<D>::value>;



template< template<typename T> class VECTOR, typename T>
class TVecAddOperators {
public:
    
    template<typename U>
    constexpr VECTOR<T>& operator+=(const VECTOR<U>& v) {
        VECTOR<T>& lhs = static_cast<VECTOR<T>&>(*this);
        for(size_t i = 0; i < lhs.size(); ++i) {
            lhs[i] += v[i];
        }
        return lhs;
    }
    
    template<typename U, typename = enable_if_arithmetic_t<U>>
    constexpr VECTOR<T>& operator+=(U v) {
        return operator+=(VECTOR<T>(v));
    }
    
    template<typename U>
    constexpr VECTOR<T>& operator-=(const VECTOR<U>& v) {
        VECTOR<T>& lhs = static_cast<VECTOR<T>&>(*this);
        for(size_t i = 0; i < lhs.size(); ++i) {
            lhs[i] -= v[i];
        }
        return lhs;
    }
    
    template<typename U, typename = enable_if_arithmetic_t<U>>
    constexpr VECTOR<T>& operator-=(U v) {
        return operator-=(VECTOR<T>(v));
    }
private:
    template<typename U>
    friend inline constexpr
    VECTOR<arithmetic_result_t<T,U>> operator+(const VECTOR<T>& lv, const VECTOR<U>& rv) {
        VECTOR<arithmetic_result_t<T,U>> res(lv);
        res+=rv;
        return res;
    }
    
    template<typename U, typename = enable_if_arithmetic_t<U>>
    friend inline constexpr
    VECTOR<arithmetic_result_t<T, U>> operator+(const VECTOR<T>& lv, U rv) {
        return lv + VECTOR<U>(rv);
    }
    
    template<typename U, typename = enable_if_arithmetic_t<U>>
    friend inline constexpr
    VECTOR<arithmetic_result_t<T, U>> operator+(U lv, const VECTOR<T>& rv) {
        return VECTOR<U>(lv) + rv;
    }
    
    template<typename U>
    friend inline constexpr
    VECTOR<arithmetic_result_t<T, U>> operator-(const VECTOR<T>& lv, const VECTOR<U>& rv) {
        VECTOR<arithmetic_result_t<T, U>> res(lv);
        res-=rv;
        return res;
    }
    
    template<typename U, typename = enable_if_arithmetic_t<U>>
    friend inline constexpr
    VECTOR<arithmetic_result_t<T, U>> operator-(const VECTOR<T>& lv, U rv) {
        return lv-VECTOR<U>(rv);
    }
    
    template<typename U, typename = enable_if_arithmetic_t<U>>
    friend inline constexpr
    VECTOR<arithmetic_result_t<T, U>> operator-(U lv, const VECTOR<T>& rv) {
        return VECTOR<U>(lv) - rv;
    }
};


template< template<typename T> class VECTOR, typename T>
class TVecProductOperators {
public:
    template<typename U>
    constexpr VECTOR<T>& operator*=(const VECTOR<U>& v) {
        VECTOR<T>& lhs = static_cast<VECTOR<T>&>(*this);
        for(int i=0;i<lhs.size();++i) {
            lhs[i] *= v[i];
        }
        return lhs;
    }
    
    template<typename U, typename = enable_if_arithmetic_t<U>>
    constexpr VECTOR<T>& operator*=(U v) {
        return operator*=(VECTOR<U>(v));
    }
    
    template<typename U>
    constexpr VECTOR<T>& operator/=(const VECTOR<U>& v) {
        VECTOR<T>& lhs = static_cast<VECTOR<T>&>(*this);
        for(int i=0;i < lhs.size(); ++i) {
            lhs[i] /= v[i];
        }
        return lhs;
    }
    
    template<typename U , typename = enable_if_arithmetic_t<U>>
    constexpr VECTOR<T>& operator/=(U v) {
        return operator/=(VECTOR<U>(v));
    }
    
private:
    
    template<typename U>
    friend inline constexpr
    VECTOR<arithmetic_result_t<T, U>> operator*(const VECTOR<T>& lv, const VECTOR<U>& rv) {
        VECTOR<arithmetic_result_t<T, U>> res(lv);
        res *= rv;
        return res;
    }
    
    template<typename U, typename = enable_if_arithmetic_t<U>>
    friend inline constexpr
    VECTOR<arithmetic_result_t<T, U>> operator*(const VECTOR<T>& lv, U rv) {
        return lv * VECTOR<U>(rv);
    }
    
    template<typename U, typename = enable_if_arithmetic_t<U>>
    friend inline constexpr
    VECTOR<arithmetic_result_t<T, U>> operator*(T lv, const VECTOR<U>& rv) {
        return VECTOR<T>(lv) * rv;
    }
    
    template<typename U>
    friend inline constexpr
    VECTOR<arithmetic_result_t<T, U>> operator/(const VECTOR<T>& lv, const VECTOR<U>& rv) {
        VECTOR<arithmetic_result_t<T, U>> res(lv);
        res /= rv;
        return res;
    }
    
    template<typename U, typename = enable_if_arithmetic_t<U>>
    friend inline constexpr
    VECTOR<arithmetic_result_t<T, U>> operator/(const VECTOR<T>& lv, U rv) {
        return lv / VECTOR<U>(rv);
    }
    
    template<typename U, typename = enable_if_arithmetic_t<U>>
    friend inline constexpr
    VECTOR<arithmetic_result_t<T, U>> operator/=(T lv, const VECTOR<U>& rv) {
        return VECTOR<T>(lv) / rv;
    }
};

template< template<typename T> class VECTOR, typename T>
class TVecUnaryOperators {
public:
    const VECTOR<T> operator-() const {
        VECTOR<T> r{};
        VECTOR<T> const& rv(static_cast<VECTOR<T> const&>(*this));
        for(size_t i=0; i< r.size(); ++i) {
            r[i] = -rv[i];
        }
        return r;
    }
};

template< template<typename T> class VECTOR, typename T>
class TVecComparisonOperators {
private:
    template<typename U>
    friend inline constexpr
    bool operator==(const VECTOR<T>& lv, const VECTOR<U>& rv) {
        for(size_t i=0; i< lv.size(); ++i) {
            if(lv[i] != rv[i]) {
                return false;
            }
        }
        return true;
    }
    
    template<typename U>
    friend inline constexpr
    bool operator!=(const VECTOR<T>& lv, const VECTOR<U>& rv) {
        return !operator==(lv, rv);
    }
    
    template<typename U>
    friend inline constexpr
    VECTOR<bool> equal(const VECTOR<T>& lv, const VECTOR<U>& rv) {
        VECTOR<bool> r{};
        for(size_t i=0; i< lv.size(); ++i) {
            r[i] = (lv[i] == rv[i]);
        }
        return r;
    }
    
    template<typename U>
    friend inline constexpr
    VECTOR<bool> notEqual(const VECTOR<T>& lv, const VECTOR<U>& rv) {
        VECTOR<bool> r{};
        for(size_t i=0; i< lv.size(); ++i) {
            r[i] = (lv[i] != rv[i]);
        }
        return r;
    }
    
    template<typename U>
    friend inline  constexpr
    VECTOR<bool> lessThan(const VECTOR<T>& lv, const VECTOR<U>& rv) {
        VECTOR<bool> r{};
        for(size_t i=0; i< lv.size(); ++i) {
            r[i] = (lv[i] < rv[i]);
        }
        return r;
    }
    
    template<typename U>
    friend inline  constexpr
    VECTOR<bool> lessThanEqual(const VECTOR<T>& lv, const VECTOR<U>& rv) {
        VECTOR<bool> r{};
        for(size_t i=0; i< lv.size(); ++i) {
            r[i] = (lv[i] <= rv[i]);
        }
        return r;
    }
    
    template<typename U>
    friend inline  constexpr
    VECTOR<bool> greaterThan(const VECTOR<T>& lv, const VECTOR<U>& rv) {
        VECTOR<bool> r{};
        for(size_t i=0; i< lv.size(); ++i) {
            r[i] = (lv[i] > rv[i]);
        }
        return r;
    }
    
    template<typename U>
    friend inline  constexpr
    VECTOR<bool> greaterThanEqual(const VECTOR<T>& lv, const VECTOR<U>& rv) {
        VECTOR<bool> r{};
        for(size_t i=0; i< lv.size(); ++i) {
            r[i] = (lv[i] >= rv[i]);
        }
        return r;
    }
};



}

#endif /* TVecHelpers_h */
