#include <iterator>

namespace detangled {

template <typename AccessorType, typename HeightPredicate>
bool is_heap(AccessorType pos, const HeightPredicate &h) {
    for (auto it = ::std::next(pos->prelbegin()); it != pos->prelend(); ++it) {
        if (h(*it, *it->parent())) { return false; }
    }
    return true;
}

}  // namespace detangled
