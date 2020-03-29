#ifndef PBUS_RESPONSE_PARSER_H
#define PBUS_RESPONSE_PARSER_H

#include <toolkit/core/Buffer.h>
#include <memory>
#include <mutex>
#include <future>

namespace pbus
{
	struct IResponseParser
	{
		virtual ~IResponseParser() = default;
		virtual void Parse(ConstBuffer data) = 0;
		virtual void SetException(std::exception_ptr ex) = 0;
	};
	TOOLKIT_DECLARE_PTR(IResponseParser);

	template<typename ResultType>
	class ResponseParser : public IResponseParser
	{
		using Promise = std::promise<ResultType>;
		Promise 		_promise;

	public:
		ResponseParser(Promise && promise): _promise(std::move(promise))
		{ }

		void Parse(ConstBuffer data) override
		{ }

		void SetException(std::exception_ptr ex)
		{ _promise.set_exception(ex); }

		std::future<ResultType> GetFuture()
		{ return _promise.get_future(); }
	};
}

#endif
