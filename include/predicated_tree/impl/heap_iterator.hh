#include "detangled/tree/accessor.h"
#include "detangled/tree/raw_tree.h"

namespace detangled {

template <class PTree>
heap_iterator<PTree>::heap_iterator() {}

template <class PTree>
heap_iterator<PTree>::heap_iterator(PTree &ptree) : ptree_(&ptree) {}

template <class PTree>
heap_iterator<PTree> &heap_iterator<PTree>::operator++() {
    ptree_->erase(accessor<const raw_tree<value_type>>(ptree_->node()));
    bool value_present = (bool)(*ptree_);
    if (!value_present) { ptree_ = nullptr; }
    return *this;
}

template <class PTree>
void heap_iterator<PTree>::operator++(int) {
    ++(*this);
}

template <class PTree>
bool heap_iterator<PTree>::operator==(const heap_iterator<PTree> &other) const {
    return ptree_ == other.ptree_;
}

template <class PTree>
bool heap_iterator<PTree>::operator!=(const heap_iterator<PTree> &other) const {
    return !(*this == other);
}

template <class PTree>
const typename heap_iterator<PTree>::value_type &heap_iterator<PTree>::operator
    *() const {
    return **ptree_;
}

template <class PTree>
const typename heap_iterator<PTree>::value_type
    *heap_iterator<PTree>::operator->() const {
    return &**ptree_;
}

}  // namespace detangled
