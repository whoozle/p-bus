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
		virtual std::exception_ptr GetException() const = 0;
	};
	TOOLKIT_DECLARE_PTR(IResponseParser);

	struct BaseResponseParser : public IResponseParser
	{
		std::exception_ptr _exception;

		void SetException(std::exception_ptr exception) override
		{ _exception = exception; }
		std::exception_ptr GetException() const override
		{ return _exception; }
	};

	template<typename ResultType>
	class ResponseParser : public BaseResponseParser
	{
		ResultType _value;

	public:
		ResponseParser(): _value()
		{ }

		void Parse(ConstBuffer data) override
		{ }

		ResultType GetValue() const
		{ return _value; }
	};

	template<>
	class ResponseParser<void> : public BaseResponseParser
	{
	public:
		void Parse(ConstBuffer data) override
		{ }

		void GetValue() const
		{ }
	};

}

#endif
