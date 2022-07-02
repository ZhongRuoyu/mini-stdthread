#ifndef THREAD_H_
#define THREAD_H_

#include <errno.h>
#include <pthread.h>

#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <ostream>
#include <system_error>
#include <tuple>
#include <type_traits>

namespace minithread {

template <std::size_t...>
struct TupleIndices {};

template <class IdxType, IdxType... Values>
struct IntegerSequence {
    template <std::size_t Sp>
    using ToTupleIndices = TupleIndices<(Values + Sp)...>;
};

namespace detail {

template <typename Tp, std::size_t... _Extra>
struct Repeat;

template <typename Tp, Tp... Np, std::size_t... _Extra>
struct Repeat<IntegerSequence<Tp, Np...>, _Extra...> {
    using type =
        IntegerSequence<Tp, Np..., sizeof...(Np) + Np...,
                        2 * sizeof...(Np) + Np..., 3 * sizeof...(Np) + Np...,
                        4 * sizeof...(Np) + Np..., 5 * sizeof...(Np) + Np...,
                        6 * sizeof...(Np) + Np..., 7 * sizeof...(Np) + Np...,
                        _Extra...>;
};

template <std::size_t Np>
struct Parity;

template <std::size_t Np>
struct Make : Parity<Np % 8>::template PMake<Np> {};

template <>
struct Make<0> {
    using type = IntegerSequence<std::size_t>;
};
template <>
struct Make<1> {
    using type = IntegerSequence<std::size_t, 0>;
};
template <>
struct Make<2> {
    using type = IntegerSequence<std::size_t, 0, 1>;
};
template <>
struct Make<3> {
    using type = IntegerSequence<std::size_t, 0, 1, 2>;
};
template <>
struct Make<4> {
    using type = IntegerSequence<std::size_t, 0, 1, 2, 3>;
};
template <>
struct Make<5> {
    using type = IntegerSequence<std::size_t, 0, 1, 2, 3, 4>;
};
template <>
struct Make<6> {
    using type = IntegerSequence<std::size_t, 0, 1, 2, 3, 4, 5>;
};
template <>
struct Make<7> {
    using type = IntegerSequence<std::size_t, 0, 1, 2, 3, 4, 5, 6>;
};

template <>
struct Parity<0> {
    template <std::size_t Np>
    struct PMake : Repeat<typename Make<Np / 8>::type> {};
};
template <>
struct Parity<1> {
    template <std::size_t Np>
    struct PMake : Repeat<typename Make<Np / 8>::type, Np - 1> {};
};
template <>
struct Parity<2> {
    template <std::size_t Np>
    struct PMake : Repeat<typename Make<Np / 8>::type, Np - 2, Np - 1> {};
};
template <>
struct Parity<3> {
    template <std::size_t Np>
    struct PMake : Repeat<typename Make<Np / 8>::type, Np - 3, Np - 2, Np - 1> {
    };
};
template <>
struct Parity<4> {
    template <std::size_t Np>
    struct PMake
        : Repeat<typename Make<Np / 8>::type, Np - 4, Np - 3, Np - 2, Np - 1> {
    };
};
template <>
struct Parity<5> {
    template <std::size_t Np>
    struct PMake : Repeat<typename Make<Np / 8>::type, Np - 5, Np - 4, Np - 3,
                          Np - 2, Np - 1> {};
};
template <>
struct Parity<6> {
    template <std::size_t Np>
    struct PMake : Repeat<typename Make<Np / 8>::type, Np - 6, Np - 5, Np - 4,
                          Np - 3, Np - 2, Np - 1> {};
};
template <>
struct Parity<7> {
    template <std::size_t Np>
    struct PMake : Repeat<typename Make<Np / 8>::type, Np - 7, Np - 6, Np - 5,
                          Np - 4, Np - 3, Np - 2, Np - 1> {};
};

template <std::size_t Ep, std::size_t Sp>
using MakeIndicesImp =
    typename Make<Ep - Sp>::type::template ToTupleIndices<Sp>;

}  // namespace detail

template <std::size_t Ep, std::size_t Sp = 0>
struct MakeTupleIndices {
    static_assert(Sp <= Ep, "MakeTupleIndices input error");
    using type = detail::MakeIndicesImp<Ep, Sp>;
};

namespace detail {

template <class Function, class... Args, size_t... Indices>
inline void ThreadExecute(std::tuple<Function, Args...> &t,
                          TupleIndices<Indices...>) {
    std::invoke(std::move(std::get<0>(t)), std::move(std::get<Indices>(t))...);
}

template <class Fp>
void *ThreadProxy(void *vp) {
    std::unique_ptr<Fp> p(static_cast<Fp *>(vp));
    using Index =
        typename MakeTupleIndices<std::tuple_size<Fp>::value, 1>::type;
    ThreadExecute(*p, Index());
    return nullptr;
}

template <class T>
std::decay_t<T> DecayCopy(T &&v) {
    return std::forward<T>(v);
}

}  // namespace detail

class Thread {
   public:
    using native_handle_type = pthread_t;

    class id {
       public:
        id() noexcept : id_(0) {}

        friend bool operator==(id x, id y) noexcept {
            if (x.id_ == 0) {
                return y.id_ == 0;
            }
            if (y.id_ == 0) {
                return false;
            }
            return pthread_equal(x.id_, y.id_) != 0;
        }
        friend bool operator!=(id x, id y) noexcept { return !(x == y); }
        friend bool operator<(id x, id y) noexcept {
            if (x.id_ == 0) {
                return y.id_ != 0;
            }
            if (y.id_ == 0) {
                return false;
            }
            return x.id_ < y.id_;
        }
        friend bool operator<=(id x, id y) noexcept { return !(y < x); }
        friend bool operator>(id x, id y) noexcept { return y < x; }
        friend bool operator>=(id x, id y) noexcept { return !(x < y); }

        template <class CharT, class Traits>
        friend std::basic_ostream<CharT, Traits> &operator<<(
            std::basic_ostream<CharT, Traits> &os, id id);

       private:
        friend class Thread;

        id(native_handle_type id) : id_(id) {}

        native_handle_type id_;
    };

    Thread() noexcept : t_(0U) {}

    Thread(Thread &&other) noexcept : t_(other.t_) { other.t_ = 0U; }

    template <
        class Fp, class... Args,
        class = std::enable_if_t<!std::is_same<
            std::remove_cv_t<std::remove_reference_t<Fp>>, Thread>::value>>
    explicit Thread(Fp &&f, Args &&...args) {
        using Gp = std::tuple<std::decay_t<Fp>, std::decay_t<Args>...>;
        std::unique_ptr<Gp> p(
            new Gp(detail::DecayCopy(std::forward<Fp>(f)),
                   detail::DecayCopy(std::forward<Args>(args))...));
        int ec = pthread_create(&t_, 0, &detail::ThreadProxy<Gp>, p.get());
        if (ec == 0) {
            p.release();
        } else {
            throw std::system_error(std::error_code(ec, std::system_category()),
                                    "Thread constructor failed");
        }
    }

    Thread(const Thread &) = delete;

    Thread &operator=(Thread &&other) noexcept {
        if (t_ != 0) {
            std::terminate();
        }
        t_ = other.t_;
        other.t_ = 0U;
        return *this;
    }

    ~Thread() {
        if (t_ != 0) {
            std::terminate();
        }
    }

    bool joinable() const noexcept { return t_ != 0; }

    id get_id() const noexcept { return t_; }

    native_handle_type native_handle() { return t_; }

    void join() {
        int ec = EINVAL;
        if (t_ != 0) {
            ec = pthread_join(t_, 0);
            if (ec == 0) {
                t_ = 0U;
            }
        }
        if (ec != 0) {
            throw std::system_error(std::error_code(ec, std::system_category()),
                                    "Thread::join failed");
        }
    }

    void detach() {
        int ec = EINVAL;
        if (t_ != 0) {
            ec = pthread_detach(t_);
            if (ec == 0) {
                t_ = 0U;
            }
        }
        if (ec != 0) {
            throw std::system_error(std::error_code(ec, std::system_category()),
                                    "Thread::detach failed");
        }
    }

    void swap(Thread &other) noexcept { std::swap(t_, other.t_); }

   private:
    pthread_t t_;
};

template <class CharT, class Traits>
std::basic_ostream<CharT, Traits> &operator<<(
    std::basic_ostream<CharT, Traits> &os, Thread::id id) {
    return os << id.id_;
}

}  // namespace minithread

#endif  // THREAD_H_
