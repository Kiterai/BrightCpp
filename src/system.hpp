#pragma once

#include <brightcpp/common.hpp>
#include <unordered_map>

namespace BRIGHTCPP_NAMESPACE {

namespace internal {

class system_container {
    std::unordered_map<size_t, void*> objs;
  public:
    template<class T>
    T& get() { return *static_cast<T*>(objs.at(typeid(T).hash_code())); }
    template<class T>
    void _reg(T& obj) { objs.at(typeid(T).hash_code()) = &obj; }
    template<class T>
    void _unreg() { objs.erase(typeid(T).hash_code()); }
};

class system_initializer {
  public:
    system_initializer();
    ~system_initializer();
    
    static system_container& global_container();
};

} // namespace internal

} // namespace BRIGHTCPP_NAMESPACE
