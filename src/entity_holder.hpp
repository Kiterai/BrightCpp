#pragma once

#include <brightcpp/handle_holder.hpp>
#include <unordered_map>

BRIGHTCPP_START

namespace internal {

template <class T> class entity_holder {
    size_t id_count = 0;
  protected:
    std::unordered_map<entity_handle_t, T> db;

  public:
    template <class... Args> entity_handle_t make(Args &&...args) {
        auto new_id = id_count;
        db.emplace(std::piecewise_construct, std::forward_as_tuple(new_id), std::forward_as_tuple(std::forward<Args>(args)...));
        id_count++;

        return new_id;
    }
    void destroy(entity_handle_t handle) { db.erase(handle); }
    T &get(entity_handle_t handle) { return db.at(handle); }
    const T &get(entity_handle_t handle) const { return db.at(handle); }
};

} // namespace internal

BRIGHTCPP_END
