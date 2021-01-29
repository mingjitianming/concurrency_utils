#ifndef FUNCTION_WAPPER_H
#define FUNCTION_WAPPER_H

#include <functional>
#include <memory>
#include <type_traits>

namespace concurrency::pool
{
    class FuncWapper
    {
        struct func_base
        {
            virtual void call() = 0;
            virtual ~func_base() = default;
        };

        template <typename Func, typename... Args>
        struct func_impl : public func_base
        {
            using R = std::invoke_result_t<Func, Args...>;
            // std::function<R()> f;
            func_impl(Func &&f, Args... args)
                : f([func = std::move(f), args...]() mutable { return func(std::forward(args)...); })
            {
            }
            ~func_impl() = default;
            void call() override
            {
                f();
            }
        };

    public:
        FuncWapper() = default;

        template <typename Func, typename... Args>
        FuncWapper(Func &&f, Args... args) : func_(std::make_unique<func_impl<Func, Args...>>(std::move(f), args...)) {}

        FuncWapper(FuncWapper &&rhs) : func_(std::move(rhs.func_)) {}

        FuncWapper &operator=(FuncWapper &&rhs)
        {
            func_ = std::move(rhs.func_);
            return *this;
        }

        FuncWapper(FuncWapper &rhs) = delete;
        FuncWapper &operator=(FuncWapper &rhs) = delete;
        ~FuncWapper() = default;

        // FuncWapper& opera
        void call() { func_->call(); }

    private:
        std::unique_ptr<func_base> func_;
    };

} // namespace concurrency::pool

#endif