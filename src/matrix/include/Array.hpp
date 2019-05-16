#pragma once

// TODO: disable this before uploading to the drone!
#define SAFE_ARRAY_INDICES

#include <algorithm>  // copy
#include <cmath>      // isfinite
#include <cstddef>    // size_t
#include <numeric>    // accumulate
#include <type_traits>
#ifdef SAFE_ARRAY_INDICES
#include <stdexcept>
#endif

#if __cplusplus >= 201500
#define MATRIX_CONSTEXPR constexpr
#else
#define MATRIX_CONSTEXPR
#endif

template <class T, size_t N>
struct Array;

template <class T>
struct getArrayTypeBase {
    using type = void;
};
template <class T, size_t N>
struct getArrayTypeBase<Array<T, N>> {
    using type = T;
};
template <typename T>
struct getArrayType : getArrayTypeBase<T> {};

template <class T>
struct getArrayLengthBase {
    MATRIX_CONSTEXPR const static size_t value = 0;
};
template <class T, size_t N>
struct getArrayLengthBase<Array<T, N>> {
    MATRIX_CONSTEXPR const static size_t value = N;
};
template <typename T>
struct getArrayLength : getArrayLengthBase<T> {};

template <class T, size_t N>
struct Array {
    T data[N];

    MATRIX_CONSTEXPR T &operator[](size_t index) {
#ifdef SAFE_ARRAY_INDICES
        if (index >= N)
            throw std::out_of_range(std::string("Array index out of bounds (") +
                                    std::to_string(index) + ") " +
                                    __PRETTY_FUNCTION__);
#endif
        return data[index];
    }
    MATRIX_CONSTEXPR const T &operator[](size_t index) const {
#ifdef SAFE_ARRAY_INDICES
        if (index >= N)
            throw std::out_of_range(std::string("Array index out of bounds (") +
                                    std::to_string(index) + ") " +
                                    __PRETTY_FUNCTION__);
#endif
        return data[index];
    }
    MATRIX_CONSTEXPR T *begin() { return &data[0]; }
    MATRIX_CONSTEXPR const T *begin() const { return &data[0]; }
    MATRIX_CONSTEXPR T *end() { return &data[N]; }
    MATRIX_CONSTEXPR const T *end() const { return &data[N]; }

    MATRIX_CONSTEXPR bool operator==(const Array<T, N> &rhs) const {
        if (this == &rhs)
            return true;
        for (size_t i = 0; i < N; i++)
            if ((*this)[i] != rhs[i])
                return false;
        return true;
    }

    MATRIX_CONSTEXPR Array<T, N> operator*(double rhs) const {
        Array<T, N> result = *this;
        result *= rhs;
        return result;
    }

    MATRIX_CONSTEXPR Array<T, N> &operator*=(double rhs) {
        for (auto &el : *this)
            el *= rhs;
        return *this;
    }

    MATRIX_CONSTEXPR Array<T, N> operator/(double rhs) const {
        Array<T, N> result = *this;
        result /= rhs;
        return result;
    }

    MATRIX_CONSTEXPR Array<T, N> &operator/=(double rhs) {
        for (auto &el : *this)
            el /= rhs;
        return *this;
    }

    MATRIX_CONSTEXPR Array<T, N> operator+(const Array<T, N> &rhs) const {
        Array<T, N> result = *this;
        result += rhs;
        return result;
    }

    MATRIX_CONSTEXPR Array<T, N> &operator+=(const Array<T, N> &rhs) {
        for (size_t i = 0; i < N; ++i)
            (*this)[i] += rhs[i];
        return *this;
    }

    MATRIX_CONSTEXPR Array<T, N> operator-(const Array<T, N> &rhs) const {
        Array<T, N> result = *this;
        result -= rhs;
        return result;
    }

    MATRIX_CONSTEXPR Array<T, N> &operator-=(const Array<T, N> &rhs) {
        for (size_t i = 0; i < N; ++i)
            (*this)[i] -= rhs[i];
        return *this;
    }

    MATRIX_CONSTEXPR Array<T, N> operator-() const {
        Array<T, N> result = *this;
        for (size_t i = 0; i < N; ++i)
            (result)[i] = -(result)[i];
        return result;
    }

    MATRIX_CONSTEXPR bool operator!=(const Array<T, N> &rhs) const {
        return !(*this == rhs);
    }

    /**
     * @brief   Assignment operator for Array<T, 1>.
     */
    template <bool EnableBool = true>
    MATRIX_CONSTEXPR
        typename std::enable_if<N == 1 && EnableBool, Array<T, 1> &>::type
        operator=(const T &t) {
        data[0] = t;
        return *this;
    }

    /**
     * @brief   Implicit conversion from Array<T, 1> to T&.
     */
    template <bool EnableBool = true>
    MATRIX_CONSTEXPR operator typename std::add_lvalue_reference<
        typename std::enable_if<N == 1 && EnableBool, T>::type>::type() {
        return data[0];
    }

    /**
     * @brief   Implicit conversion from const Array<T, 1> to const T&.
     */
    template <bool EnableBool = true>
    MATRIX_CONSTEXPR
    operator typename std::add_lvalue_reference<typename std::add_const<
        typename std::enable_if<N == 1 && EnableBool, T>::type>::type>::type()
        const {
        return data[0];
    }

    /**
     * @brief   Implicit conversion from Array<Array<T, 1>, 1> to T&.
     */
    template <bool EnableBool = true>
    operator typename std::add_lvalue_reference<typename std::enable_if<
        N == 1 && getArrayLength<T>::value == 1 && EnableBool,
        typename getArrayType<T>::type>::type>::type() {
        return data[0][0];
    }

    /**
     * @brief   Implicit conversion from const Array<Array<T, 1>, 1> to const T&.
     */
    template <bool EnableBool = true>
    operator typename std::add_lvalue_reference<
        typename std::add_const<typename std::enable_if<
            N == 1 && getArrayLength<T>::value == 1 && EnableBool,
            typename getArrayType<T>::type>::type>::type>::type() const {
        return data[0][0];
    }

    /** 
     * @brief   Comparison of arrays: less than
     */
    template <class U>
    MATRIX_CONSTEXPR auto operator<(const Array<U, N> &rhs) const {
        Array<decltype(this->data[0] < rhs.data[0]), N> result = {};
        for (size_t i = 0; i < N; ++i)
            result[i] = this->data[i] < rhs.data[i];
        return result;
    }

    /** 
     * @brief   Comparison of arrays: less than or equal
     */
    template <class U>
    MATRIX_CONSTEXPR auto operator<=(const Array<U, N> &rhs) const {
        Array<decltype(this->data[0] <= rhs.data[0]), N> result = {};
        for (size_t i = 0; i < N; ++i)
            result[i] = this->data[i] <= rhs.data[i];
        return result;
    }

    /** 
     * @brief   Comparison of arrays: greater than
     */
    template <class U>
    MATRIX_CONSTEXPR auto operator>(const Array<U, N> &rhs) const {
        Array<decltype(this->data[0] > rhs.data[0]), N> result = {};
        for (size_t i = 0; i < N; ++i)
            result[i] = this->data[i] > rhs.data[i];
        return result;
    }

    /** 
     * @brief   Comparison of arrays: greater than or equal
     */
    template <class U>
    MATRIX_CONSTEXPR auto operator>=(const Array<U, N> &rhs) const {
        Array<decltype(this->data[0] >= rhs.data[0]), N> result = {};
        for (size_t i = 0; i < N; ++i)
            result[i] = this->data[i] >= rhs.data[i];
        return result;
    }

    MATRIX_CONSTEXPR static Array<T, N> fromCppArray(const T (&a)[N]) {
        Array<T, N> result = {};
        std::copy(a, a + N, result.begin());
        return result;
    }

    MATRIX_CONSTEXPR static Array<T, N> fromCArray(const T *a) {
        Array<T, N> result = {};
        std::copy(a, a + N, result.begin());
        return result;
    }

    static MATRIX_CONSTEXPR size_t length() { return N; }
    using type = T;
};

// Rounding
template <class T>
MATRIX_CONSTEXPR T round(T t, size_t digits) {
    return std::round(t * std::pow(10, digits)) / std::pow(10, digits);
}
template <class T, size_t N>
MATRIX_CONSTEXPR Array<T, N> round(Array<T, N> a, size_t digits) {
    for (auto &e : a) {
        e = round(e, digits);
        if (e == T{})  // fix -0.0
            e = T{};
    }
    return a;
}

template <class T, size_t N>
MATRIX_CONSTEXPR Array<T, N> round(Array<T, N> a) {
    using namespace std;
    for (auto &e : a) {
        e = round(e);
        if (e == T{})  // fix -0.0
            e = T{};
    }
    return a;
}

// Absolute value
template <class T, size_t N>
MATRIX_CONSTEXPR Array<T, N> abs(Array<T, N> a) {
    using namespace std;
    for (auto &e : a)
        e = abs(e);
    return a;
}

template <class T, size_t N>
MATRIX_CONSTEXPR bool isfinite(const Array<T, N> &a) {
    using std::isfinite;
    return std::all_of(a.begin(), a.end(),
                       [](const T &e) { return isfinite(e); });
}

template <class T, size_t N>
MATRIX_CONSTEXPR Array<T, N> operator*(double lhs, const Array<T, N> &rhs) {
    Array<T, N> result = rhs;
    result *= lhs;
    return result;
}

template <class T>
MATRIX_CONSTEXPR Array<T, 1> operator*(Array<T, 1> lhs, Array<T, 1> rhs) {
    return {T{lhs} * T{rhs}};
}

template <class T>
MATRIX_CONSTEXPR Array<T, 1> operator/(Array<T, 1> lhs, Array<T, 1> rhs) {
    return {T{lhs} / T{rhs}};
}

template <class T, class U,
          typename = std::enable_if<getArrayLength<U>::value == 0>>
MATRIX_CONSTEXPR auto operator+(const Array<T, 1> &a, const U &u)
    -> decltype(static_cast<T>(a) + u) {
    return static_cast<T>(a) + u;
}

template <class T, class U,
          typename = std::enable_if<getArrayLength<U>::value == 0>>
MATRIX_CONSTEXPR auto operator+(const U &u, const Array<T, 1> &a)
    -> decltype(u + static_cast<T>(a)) {
    return u + static_cast<T>(a);
}

template <class T, class U,
          typename = std::enable_if<getArrayLength<U>::value == 0>>
MATRIX_CONSTEXPR auto operator-(const Array<T, 1> &a, const U &u)
    -> decltype(static_cast<T>(a) - u) {
    return static_cast<T>(a) - u;
}

template <class T, class U,
          typename = std::enable_if<getArrayLength<U>::value == 0>>
MATRIX_CONSTEXPR auto operator-(const U &u, const Array<T, 1> &a)
    -> decltype(u - static_cast<T>(a)) {
    return u - static_cast<T>(a);
}

template <class T, class U,
          typename = std::enable_if<getArrayLength<U>::value == 0>>
MATRIX_CONSTEXPR auto operator/(const U &u, const Array<T, 1> &a)
    -> decltype(u / static_cast<T>(a)) {
    return u / static_cast<T>(a);
}

template <class T, size_t N>
MATRIX_CONSTEXPR T sum(const Array<T, N> &a) {
    return std::accumulate(a.begin(), a.end(), T{});
}

template <class T, class U>
MATRIX_CONSTEXPR
    typename std::enable_if<getArrayLength<U>::value == 0, bool>::type
    operator<(const U &u, const Array<T, 1> &a) {
    return u < static_cast<T>(a);
}

template <class T, class U>
MATRIX_CONSTEXPR
    typename std::enable_if<getArrayLength<U>::value == 0, bool>::type
    operator<(const Array<T, 1> &a, const U &u) {
    return static_cast<T>(a) < u;
}

template <class T, class U>
MATRIX_CONSTEXPR
    typename std::enable_if<getArrayLength<U>::value == 0, bool>::type
    operator<=(const U &u, const Array<T, 1> &a) {
    return u <= static_cast<T>(a);
}

template <class T, class U>
MATRIX_CONSTEXPR
    typename std::enable_if<getArrayLength<U>::value == 0, bool>::type
    operator<=(const Array<T, 1> &a, const U &u) {
    return static_cast<T>(a) <= u;
}

template <class T, class U>
MATRIX_CONSTEXPR
    typename std::enable_if<getArrayLength<U>::value == 0, bool>::type
    operator>(const U &u, const Array<T, 1> &a) {
    return u > static_cast<T>(a);
}

template <class T, class U>
MATRIX_CONSTEXPR
    typename std::enable_if<getArrayLength<U>::value == 0, bool>::type
    operator>(const Array<T, 1> &a, const U &u) {
    return static_cast<T>(a) > u;
}

template <class T, class U>
MATRIX_CONSTEXPR
    typename std::enable_if<getArrayLength<U>::value == 0, bool>::type
    operator>=(const U &u, const Array<T, 1> &a) {
    return u >= static_cast<T>(a);
}

template <class T, class U>
MATRIX_CONSTEXPR
    typename std::enable_if<getArrayLength<U>::value == 0, bool>::type
    operator>=(const Array<T, 1> &a, const U &u) {
    return static_cast<T>(a) >= u;
}

template <class T, class U>
MATRIX_CONSTEXPR
    typename std::enable_if<getArrayLength<U>::value == 0, bool>::type
    operator==(const U &u, const Array<T, 1> &a) {
    return u == static_cast<T>(a);
}

template <class T, class U>
MATRIX_CONSTEXPR
    typename std::enable_if<getArrayLength<U>::value == 0, bool>::type
    operator==(const Array<T, 1> &a, const U &u) {
    return static_cast<T>(a) == u;
}

template <class T, class U>
MATRIX_CONSTEXPR
    typename std::enable_if<getArrayLength<U>::value == 0, bool>::type
    operator!=(const U &u, const Array<T, 1> &a) {
    return u != static_cast<T>(a);
}

template <class T, class U>
MATRIX_CONSTEXPR
    typename std::enable_if<getArrayLength<U>::value == 0, bool>::type
    operator!=(const Array<T, 1> &a, const U &u) {
    return static_cast<T>(a) != u;
}

template <class T>
MATRIX_CONSTEXPR T &operator++(Array<T, 1> &a) {
    T &e = static_cast<T &>(a);
    ++e;
    return e;
}

//

template <class T, class U>
MATRIX_CONSTEXPR U &operator+=(U &u, const Array<T, 1> &a) {
    return u += static_cast<T>(a);
}

template <class T, class U>
MATRIX_CONSTEXPR Array<T, 1> &operator+=(Array<T, 1> &a, const U &u) {
    Array<T, 1> b = {};
    b[0] = u;
    return a += b;
}

//

template <class T, class F>
MATRIX_CONSTEXPR auto map(const T &t, F &&f) {
    return f(t);
}

template <class T, size_t N, class F>
MATRIX_CONSTEXPR auto map(const Array<T, N> &a, F &&f) {
    Array<decltype(map(a[0], f)), N> result = {};
    for (size_t i = 0; i < N; ++i)
        result[i] = map(a[i], f);
    return result;
}