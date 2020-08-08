#pragma once

#include <string>
#include <any>
#include <functional>
#include <optional>

#include "String.h"
#include "Log.h"

class IArgument
{
public:
	virtual ~IArgument() = default;
	virtual void Set(const std::string& value) = 0;
	virtual operator std::string() const = 0;
	virtual std::any Get() = 0;
	virtual std::string GetName() = 0;
	virtual std::string GetDesc() = 0;
};

template <typename T = std::string>
class Argument : public IArgument
{
public:
	using ValueType = T;
	using ValueTypeOpt = std::optional<ValueType>;
	using ConstraintFuncMsg = std::optional<std::string>;
	using ConstraintFunc = std::function<ConstraintFuncMsg(const ValueType&)>;
	using ConvertFunc = std::function<ValueType(const std::string&)>;

	Argument(
		std::string name,
		std::string desc = "",
		ValueTypeOpt defaultValue = std::nullopt,
		ConstraintFunc constraint = [](const auto&) { return std::nullopt; },
		ConvertFunc convert = [](const auto& val) { return val; }) :
		val(std::move(defaultValue)),
		constraint(std::move(constraint)),
		convert(std::move(convert)),
		name(std::move(name)),
		desc(std::move(desc)) {}

	void Set(const std::string& value) override
	{
		auto conv = convert(value);
		auto msg = constraint(conv);
		if (!msg)
		{
			val = ValueTypeOpt(conv);
		}
		else
		{
			ThrowEx(name, ": ", msg.value().c_str());
		}
	}

	[[nodiscard]] std::any Get() override { return val; }

	[[nodiscard]] std::string GetName() override { return name; }

	[[nodiscard]] std::string GetDesc() override { return desc; }

	[[nodiscard]] operator std::string() const override { return name; }

private:
	std::any val;
	ConstraintFunc constraint;
	ConvertFunc convert;
	std::string name;
	std::string desc;
};

class Arguments
{
public:
	void Parse(int argc, char** argv);

	template <typename T>
	void Add(Argument<T>& arg)
	{
		if (args.find(arg.GetName()) != args.end())
		{
			ThrowEx(PrefixString<'-'>(args.at(arg)->GetName()), ": exists");
		}
		args[arg.GetName()] = &arg;
	}

	std::string GetDesc();

	template <typename T = std::string>
	typename Argument<T>::ValueType Value(const std::string& arg)
	{
		try
		{
			return Get<T>(arg).value();
		}
		catch (const std::exception& e)
		{
			ThrowEx(PrefixString<'-'>(args.at(arg)->GetName()), ": ", e.what());
		}
	}

	template <typename T = std::string>
	std::optional<T> Get(const std::string& arg)
	{
		return std::any_cast<typename Argument<T>::ValueTypeOpt>(args.at(arg)->Get());
	}

	IArgument* operator[](const std::string& arg);

private:
	std::unordered_map<std::string, IArgument*> args;
};