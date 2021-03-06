#ifndef PBUS_RESPONSE_PARSER_H
#define PBUS_RESPONSE_PARSER_H

#include <toolkit/serialization/bson/InputStream.h>
#include <toolkit/core/Buffer.h>
#include <memory>
#include <mutex>
#include <future>

namespace pbus
{
	struct IResponseParser
	{
		virtual ~IResponseParser() = default;
		virtual void Parse(ConstBuffer data, size_t & offset) = 0;
		virtual void SetException(std::exception_ptr ex) = 0;
		virtual std::exception_ptr GetException() const = 0;
		virtual bool Finished() const = 0;
	};
	TOOLKIT_DECLARE_PTR(IResponseParser);

	class BaseResponseParser : public IResponseParser
	{
	protected:
		std::exception_ptr 	_exception;
		bool				_finished = false;

	public:
		bool Finished() const override
		{ return _finished; }

		void SetException(std::exception_ptr exception) override
		{ _exception = exception; _finished = true; }

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

		void Parse(ConstBuffer data, size_t & offset) override
		{
			_value = serialization::bson::ReadSingleValue<ResultType>(data, offset);
			_finished = true;
		}

		ResultType GetValue() const
		{ return _value; }
	};

	template<>
	class ResponseParser<void> : public BaseResponseParser
	{
	public:
		void Parse(ConstBuffer data, size_t & offset) override
		{
			serialization::bson::ReadSingleValue<serialization::Undefined>(data, offset);
			_finished = true;
		}

		void GetValue() const
		{ }
	};

}

#endif
