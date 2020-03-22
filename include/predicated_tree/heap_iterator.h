#ifndef PREDICATED_TREE_HEAP_ITERATOR_H
#define PREDICATED_TREE_HEAP_ITERATOR_H

#include <iterator>

namespace detangled {

/// @addtogroup predicated
/// @{

/// A *destructive* iterator which pops `predicated_tree` elements.
///
/// This iterator allows sequencing the elements of a `predicated_tree` in a
/// heap-like fashion. The iterations operation involves popping elements off
/// the `predicated_tree`. If client code runs through this iterator till `end`
/// then the tree would be empty.
///
/// Due to it's very nature, this is a single pass (`InputIterator`).
///
/// Usage Example:
///
/// ```cpp
/// predicated_tree<uint32_t, ...> p;
/// insert_values_in_tree(p);
///
/// std::copy(heap_iterator(p), heap_iterator_end(p),
///           ::std::ostream_iterator<uint32_t>(::std::cout, ", "));
/// ```
///
/// NOTE: Due to it's nature it makes sense to only have one copy of the
/// iterator incrementing.
/// NOTE: Due to it's nature, after incrementing, previously held value is
/// destroyed and hence holding onto references of values from the iterator is
/// not valid.
///
/// @tparam PTree is a `predicated_tree` which objects of this class will
///     heap-iterate over.
template <class PTree>
class heap_iterator : public ::std::iterator<std::input_iterator_tag,
                                             typename PTree::value_type> {
  public:
    using value_type = typename PTree::value_type;

    heap_iterator(PTree &ptree);
    heap_iterator();

    heap_iterator<PTree> &operator++();
    void operator++(int);

    bool operator==(const heap_iterator &other) const;
    bool operator!=(const heap_iterator &other) const;

    const value_type &operator*() const;
    const value_type *operator->() const;

  private:
    PTree *ptree_ = nullptr;
};

/// A helper function to create the heap end iterator.
///
/// NOTE: We don't really need or use the function argument except for type
/// deduction.
template <class PTree>
heap_iterator<PTree> heap_iterator_end(PTree &) {
    return heap_iterator<PTree>();
}

/// @}

}  // namespace detangled

#include "impl/heap_iterator.hh"

#endif  // PREDICATED_TREE_HEAP_ITERATOR_H
