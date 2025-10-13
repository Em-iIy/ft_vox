/*
Created by: Emily (Em_iIy) Winnink
Created on: 24/08/2025
*/

#pragma once

#include <exception>

template<typename T, typename E>
class	Expected {
	public:
		Expected(): _hasVal(true), _val({}) {}

		Expected(Expected &src): _hasVal(src._hasVal)
		{
			if (_hasVal)
				new (&_val) T(src._val);
			else
				new (&_error) E(src._error);
		}

		Expected(Expected &&src): _hasVal(src._hasVal)
		{
			if (_hasVal)
				new (&_val) T(std::move(src._val));
			else
				new (&_error) E(std::move(src._error));
		}

		Expected(T val): _hasVal(true), _val(val) {}

		Expected(E error): _hasVal(false), _error(error) {}

		Expected	&operator=(const Expected &src)
		{
			_hasVal = src.hasValue;
			if (_hasVal)
				_val = src._val;
			else
				_error = src._error;
			return (*this);
		}

		~Expected() {}


		bool	hasValue() const
		{
			return (_hasVal);
		}

		constexpr T	&value()
		{
			if (!_hasVal)
				throw std::exception();
			return (_val);
		}

		constexpr E	&error()
		{
			if (_hasVal)
				throw std::exception();
			return (_error);
		}


		constexpr T	*operator->() const
		{
			if (!_hasVal)
				throw std::exception();
			return (&_val);

		}

		constexpr T	&operator*() const
		{
			if (!_hasVal)
				throw std::exception();
			return (_val);
		}


	private:
		bool	_hasVal;
		union {
			T	_val;
			E	_error;
		};
};
