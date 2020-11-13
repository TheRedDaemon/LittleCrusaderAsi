#ifndef ENUMBASE
#define ENUMBASE

#include <string>
#include <map>

// template for EnumType structure
template<typename T>
class EnumContainer
{
  using ConIter = typename std::map<std::string, std::unique_ptr<EnumContainer<T>>>::iterator;

private:

  const std::pair<ConIter, T> enumPair;

public:

  EnumContainer(ConIter it, T &&value) : enumPair(std::move(it), std::forward<T>(value)){};

  // returns const reference to name
  // should be stable -> all positions created after start
  const std::string& getName() const
  {
    return enumPair.first->first;
  }

  // returns const reference to value
  const T& getValue() const
  {
    return enumPair.second;
  }

private:

  // TODO: maybe delete other stuff?
  // prevent copy and assign (not sure how necessary)
  EnumContainer(const EnumContainer &base) = delete;
  virtual EnumContainer& operator=(const EnumContainer &base) final = delete;
};


// uses no namespace -> is template to derived from anyway
// NOTE: desc pointer just guarantees the uniqueness of the template
// can however be used to store some description
// these strings can be created using: static constexpr char ...[]{ ... };
template<const char desc[], typename T, bool enumClassLike>
class EnumBase
{

private:
  // for intern use
  using _EnumTypeObj = typename std::conditional<enumClassLike, EnumContainer<T>, T>::type;

  // now pointers are stable (map iterator stable + T obj pointer)
  inline static std::map<std::string, std::unique_ptr<_EnumTypeObj>> enumMap;

public:
  // will be used as type to create static variables
  using EnumType = typename std::conditional<enumClassLike, const _EnumTypeObj* const, const _EnumTypeObj>::type;

  // returns description of class
  static const std::string GetDescription()
  {
    return desc;
  }

  // receives an identifier string and returns a pointer to the value
  // if no value is found, the pointer will be empty
  static const T* const GetValue(const std::string& name)
  {
    auto it{ enumMap.find(name) };
    if (it != enumMap.end())
    {
      if constexpr (enumClassLike)
      {
        return &(it->second->getValue());
      }
      else
      {
        return it->second.get();
      }
    }
    
    return nullptr;
  }

protected:

  // takes (and moves) the given values into the control map to create an enum
  // returns either a pointer to be used as enum (with acess to name and value)
  // or a !copy! of the value (so should complex objects be used -> use class behaviour)
  // NOTE: if a key already exists -> abort()
  // (I sadly do not know any way to validate this during compile everything would need to be constexpr
  static EnumType CreateEnum(std::string &&name, T &&value)
  {
    auto it{ enumMap.find(name) };
    if (it != enumMap.end())
    {
      // crash it -> want valid enum, everything else should crash
      // exit(1);
      abort(); // this should never be reached by normal run and is more for debug, and i f it happens, it should burst
    }

    // create iterator
    auto res{ enumMap.emplace(std::make_pair(std::move(name), nullptr)) };
    if constexpr (enumClassLike)
    {
      // creates string and then swaps container in place
      res.first->second = std::make_unique<_EnumTypeObj>(res.first, std::forward<T>(value));
      return res.first->second.get();
    }
    else
    {
      res.first->second = std::make_unique<_EnumTypeObj>(std::forward<T>(value));
      return *(res.first->second);
    }
  }

  virtual ~EnumBase(){ }

private:
  EnumBase() = delete;
};

/*
static constexpr char testName[]{ "Test" };
class Test : public EnumBase<testName, int32_t, false>
{
public:

  inline static EnumType TEST{ CreateEnum("Test", 32) };

  inline static EnumType TEST2{ CreateEnum("Test", 43) };

};
*/

#endif // ENUMBASE