#pragma once

template <int argv>     // 计算 argv 二进制最高位 1 在第几位
struct num2;

template <int A>
struct num2 {
    static constexpr int value = num2<A / 2>::value + 1;
};
template<>
struct num2<0> {
    static constexpr int value = 0;
};


template <int argv>     // 判断 argv 是否为完全平方数
struct check2;

template <int A>
struct check2 {
    static constexpr int value = ((A & (A - 1)) == 0);
};

template <int ...args>  // 定义 f(x) 是大于等于 x 且是完全平方数的数字 这里是求 \max{f(x_i)} 
struct upperBound2;

template <int A>
struct upperBound2<A> { // 求 f(A)
    static constexpr int value = check2<A>::value ? A : (1 << (num2<A>::value));
};

template <int A, int ...args> 
struct upperBound2<A, args...> {
    static constexpr int value = upperBound2<A>::value > upperBound2<args...>::value ? upperBound2<A>::value : upperBound2<args...>::value;
};