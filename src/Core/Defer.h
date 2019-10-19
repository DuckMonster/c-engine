template<typename TFunctor>
struct _Defer
{
	_Defer() {}
	_Defer(TFunctor functor) : functor(functor) {}
	~_Defer()
	{
		functor();
	}

	TFunctor functor;
};

struct DeferHelper
{
	template<typename TFunctor>
	_Defer<TFunctor> operator+(TFunctor functor)
	{
		return _Defer<TFunctor>(functor);
	}
};

#define CONCAT_INTERNAL(a, b) a##b
#define CONCAT(a, b) CONCAT_INTERNAL(a, b)
#define defer const auto& CONCAT(defer__, __LINE__) = DeferHelper() + [&]()