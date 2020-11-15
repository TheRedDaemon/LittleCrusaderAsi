#ifndef ENUMBASE
#define ENUMBASE

#include <string>
#include <unordered_map>
#include <functional>

// template for EnumType structure
namespace modcore
{
  template<typename T>
  class EnumContainer
  {
  private:

    const std::string* namePtr{ nullptr };
    const T value;

  public:

    EnumContainer(T &&valueIn) : value(std::forward<T>(valueIn))
    {
    };

    // returns const reference to name
    const std::string& getName() const
    {
      if (namePtr)
      {
        return *(namePtr);
      }
      else
      {
        abort(); // -> should burst in this case
      }
    }

    // returns const reference to value
    const T& getValue() const
    {
      return value;
    }

  private:

    // needs to be called
    void setNamePtr(const std::string* nameRef)
    {
      namePtr = nameRef;
    }

    // TODO: maybe delete other stuff?
    // prevent copy and assign (not sure how necessary)
    EnumContainer(const EnumContainer &base) = delete;
    EnumContainer& operator=(const EnumContainer &base) = delete;

    // needs to be friend:
    template<const char desc[], typename U, bool enumClassLike, bool uniqueValues>
    friend struct EnumBase;
  };


  // uses no namespace -> is template to derived from anyway
  // NOTE: desc pointer just guarantees the uniqueness of the template
  // can however be used to store some description
  // these strings can be created using: static constexpr char ...[]{ ... };
  // typename       -> type of value
  // enumClassLike  -> if true, uses pointer to EnumContainer as values, instead of the values itself
  // uniqueValues   -> fails if value repeats, adds additional map structure that uses values as keys, allows getEnum (if classlike and getName by value)
  //                -> however, needs more space and stores another copy of values -> should not be complex, need to be compareable
  template<const char desc[], typename T, bool enumClassLike, bool uniqueValues = false>
  struct EnumBase
  {

  private:
    // for intern use
    using _EnumTypeObj = typename std::conditional<enumClassLike, EnumContainer<T>, T>::type;

    // was reading stuff again -> elements (pairs of keys, values) have stable references, so
    // the map can be and unordered map
    //inline static std::map<std::string, std::unique_ptr<_EnumTypeObj>> enumMap;
    inline static std::unordered_map<std::string, _EnumTypeObj> enumMap;

    // adds funcs and support for unique value restriction and functions
    // turns map into little bool, if this structure is not needed
    inline static typename std::conditional<uniqueValues,
      std::unordered_map<T, std::pair<const std::string*, const _EnumTypeObj*>>,
      bool>::type uniqueValueMap;

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
    // in case of not class like, this is also the enum
    static const T* const GetValue(const std::string& name)
    {
      auto it{ enumMap.find(name) };
      if (it != enumMap.end())
      {
        if constexpr (enumClassLike)
        {
          return &(it->second.getValue());
        }
        else
        {
          return &(it->second);
        }
      }

      return nullptr;
    }


    // executes a function for every pseudo enum pair (in order of the names (stored via map))
    // the given paramters are a const reference to the name and a version of the pseudo enum
    // NOTE: for non class like, this will copy the value, so be careful with complex objects
    static void WithEachEnum(const std::function<void(const std::string&, EnumType)> &func)
    {
      for (const auto&[name, value] : enumMap)
      {
        if constexpr (enumClassLike)
        {
          func(name, *value);
        }
        else
        {
          func(name, value);
        }
      }
    }


    // executes a function for every pseudo enum name/value pair (in order of the names (stored via map))
    // the given paramters are a const reference to the name and a const reference to the value
    // in case of a class like, the enum value itself is missing (no pointer)
    // for a non class like, this behaves almost the same as WithEachEnum, but uses a reference to the value
    // this might be preferred for complex objects, however, they were already copied for the value creation
    // so this only prevents unnecessary copies
    static void WithEachValue(const std::function<void(const std::string&, const T&)> &func)
    {
      for (const auto&[name, value] : enumMap)
      {
        if constexpr (enumClassLike)
        {
          func(name, value.getValue());
        }
        else
        {
          func(name, value);
        }
      }
    }


    // additional func for enumClassLike
    // receives an identifier string and returns EnumType
    // only supported by enumClassLike, since there is no reliable way to return an invalid only type Enum
    // if no value is found, the EnumType will be nullptr
    template<typename U = EnumType>
    static typename std::enable_if_t<enumClassLike, U>
      GetEnumByName(const std::string& name)
    {
      auto it{ enumMap.find(name) };
      if (it != enumMap.end())
      {
        return &(it->second);
      }

      return nullptr;
    }


    // adds funcs and support for unique value restriction and functions
    // receives a value and returns a pointer to the name
    // if no value is found, the pointer will be empty
    template<typename U = std::string>
    static typename std::enable_if_t<uniqueValues, const U* const>
      GetName(const T& value)
    {
      auto it{ uniqueValueMap.find(value) };
      if (it != uniqueValueMap.end())
      {
        return it->second.first;
      }

      return nullptr;
    }


    // adds getEnumByValue if enumClasslike and uniqueValue
    // receives a value and returns a EnumType
    // if no value is found, the EnumType will be empty
    template<typename U = EnumType>
    static typename std::enable_if_t<enumClassLike && uniqueValues, U>
      GetEnumByValue(const T& value)
    {
      auto it{ uniqueValueMap.find(value) };
      if (it != uniqueValueMap.end())
      {
        return it->second.second;
      }

      return nullptr;
    }

    virtual ~EnumBase()
    {
    }

  protected:

    // takes (and moves) the given values into the control map to create an enum
    // returns either a pointer to be used as enum (with acess to name and value)
    // or a !copy! of the value (so should complex objects be used -> use class behaviour)
    // NOTE: if a key already exists -> abort()
    // (I sadly do not know any way to validate this during compile everything would need to be constexpr
    static EnumType CreateEnum(std::string &&name, T &&value)
    {
      // NOTE:
      // uses abort if name is added a second time -> want valid enum, everything else should crash
      // this should never be reached by normal run and is more for debug, and if it happens, it should burst

      auto res{ enumMap.try_emplace(std::forward<std::string>(name), std::move(value)) };
      if (!res.second)
      {
        abort();
      }

      if constexpr (enumClassLike)
      {
        res.first->second.setNamePtr(&(res.first->first));
      }

      if constexpr (uniqueValues)
      {
        auto vRes{ uniqueValueMap.try_emplace(*GetValue(res.first->first), &(res.first->first), &(res.first->second)) };
        if (!vRes.second)
        {
          abort();
        }
      }

      if constexpr (enumClassLike)
      {
        return &(res.first->second);
      }
      else
      {
        return res.first->second;
      }
    }

  private:
    EnumBase() = delete;
  };

  /*
  static constexpr char testName[]{ "Test" };
  struct Test : EnumBase<testName, int32_t, true, true>
  {
  public:

    inline static EnumType TEST{ CreateEnum("Test", 32) };

    inline static EnumType TEST2{ CreateEnum("Test2", 43) };

  };
  */
}

template<const char desc[], typename T, bool enumClassLike, bool uniqueValues = false>
using PseudoEnum = modcore::EnumBase<desc, T, enumClassLike, uniqueValues>;

#endif // ENUMBASE