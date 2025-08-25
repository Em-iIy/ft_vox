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

		Expected(T val): _hasVal(true), _val(val) {}

		Expected(E error): _hasVal(false), _error(error) {}

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
