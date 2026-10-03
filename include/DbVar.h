#ifndef DBVAR_H
#define DBVAR_H
 
#include <utility>
#include <string>

class IDbVar 
{
 public:
    virtual ~IDbVar() = default;
    virtual std::string getString() const = 0;
    virtual int getInt() const = 0;
    virtual bool isDirty() const = 0;
    virtual void clean() = 0;
};

template<typename T>
class DbVar : public IDbVar
{
 public:
    DbVar() = default;
    explicit DbVar(T value)
        : value_(std::move(value)) {}

    DbVar& operator=(T value) 
    {
        if (value_ != value) 
        {
            value_ = std::move(value);
            dirty_ = true;
        }
        return *this;
    }
    // All operators below are so DbVar can behave exactly like T without the need of getters for value_
    operator const T&() const
    {
        return value_;
    }

    bool operator==(const T& other) const
    {
        return value_ == other;
    }

    bool operator!=(const T& other) const
    {
        return value_ != other;
    }

    bool operator<(const T& other) const
    {
        return value_ < other;
    }

    bool operator>(const T& other) const
    {
        return value_ > other;
    }

    bool operator<=(const T& other) const
    {
        return value_ <= other;
    }

    bool operator>=(const T& other) const
    {
        return value_ >= other;
    }
    //

    std::string getString() const override 
    {
        if constexpr (std::is_same_v<T, std::string>)
        {
            return value_;
        }
        else
        {
            return std::to_string(value_);
        }
    }

    int getInt() const override 
    {
        if constexpr (std::is_same_v<T, int>)
        {
            return value_;
        }
        else
        {
            return std::stoi(getString());
        }
    }
    
    bool isDirty() const override { return dirty_;}
    void clean() override { dirty_ = false; }

 private:
    T value_{};
    bool dirty_ = false;
};

#endif