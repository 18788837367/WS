//
//  vec2.h
//  WS
//
//  Created by zrf on 2023/3/8.
//

#ifndef vec2_h
#define vec2_h

#include "TVecHelpers.h"

namespace ME {

template<typename T>
class TVec2 :
    public TVecAddOperators<TVec2, T>,
    public TVecProductOperators<TVec2, T>,
    public TVecUnaryOperators<TVec2, T>,
    public TVecComparisonOperators<TVec2, T> {
public:
    typedef T value_type;
    typedef T& reference;
    typedef T const& const_reference;
    typedef size_t size_type;
    static constexpr size_t SIZE = 2;
    
    inline constexpr size_type size() const { return SIZE; }
    
    inline constexpr T const& operator[](size_t i) const noexcept {
        assert(i<SIZE);
        return v[i];
    }
    
    inline constexpr T& operator[](size_t i) noexcept {
        assert(i<SIZE);
        return v[i];
    }
    
    TVec2() = default;
    
    template<typename A, typename = enable_if_arithmetic_t<A>>
    constexpr TVec2(A v) noexcept : v{T(v), T(v)} {}
    
    template<typename A, typename B, typename = enable_if_arithmetic_t<A,B>>
    constexpr TVec2(A x, B y) noexcept : v{T(x), T(y)} {}
    
    template<typename A, typename = enable_if_arithmetic_t<A>>
    constexpr TVec2(const TVec2<A>& v) noexcept : v{T(v[0]), T(v[1])} {}
    
    union {
        T v[SIZE];
        struct { T x, y;};
        struct { T s, t;};
        struct { T r, g;};
    };
};




}

#endif /* vec2_h */
