constexpr side operator!(const side wing) {
    if (wing == side::left) {
        return side::right;
    } else {
        return side::left;
    }
}

constexpr traversal_order operator~(const traversal_order order) {
    if (order == traversal_order::pre) {
        return traversal_order::post;
    } else if (order == traversal_order::in) {
        return traversal_order::in;
    } else {
        return traversal_order::pre;
    }
}

// There are a large number of wrapper functions of the following type
//     returnType functionName(side wing, args...);
//
// which have analogous
//     template <side wing>
//     returnType function(args...);
//
// templated functions. The definition of such wrapper functions is as seen for
// the SWITCH_ON_SIDE macro. Basically it involves looking at the wing argument
// and choosing to call a specific template version (side == left or right).
// This macro does this and makes is more reliable to define the wrapper
// functions with much lower probability of errors creeping in.
//
// TODO(ghochee): Try to template instantiate the entire definition or MACRO
// define the whole function instead of just the body.
#define SWITCH_ON_SIDE(function_name, ...)              \
    if (wing == side::left) {                           \
        return function_name<side::left>(__VA_ARGS__);  \
    } else {                                            \
        return function_name<side::right>(__VA_ARGS__); \
    }

// The following code for compaction when accessing array indices (children_).
// NOTE: https://stackoverflow.com/questions/8357240
//
// TODO(ghochee): Make this private to a class to avoid polluting global
// namespace.
template <typename E>
constexpr auto as_int(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
}

// TODO(ghochee): Make these private to a class to avoid polluting global
// namespace.
constexpr std::underlying_type_t<side> _left = as_int(side::left);
constexpr std::underlying_type_t<side> _right = as_int(side::right);

template <typename T>
raw_tree<T>::raw_tree(raw_tree &&other)
    : value_(std::move(other.value_)),
      parent_(other.parent_),
      children_(std::move(other.children_)) {
    if (children_[_left]) { children_[_left]->parent_ = this; }
    if (children_[_right]) { children_[_right]->parent_ = this; }

    if (parent_ == &other) {
        parent_ = this;
    } else if (other.is_side<side::left>()) {
        parent_->children_[_left].reset();
    } else {
        parent_->children_[_right].reset();
    }
}

template <typename T>
raw_tree<T> &raw_tree<T>::operator=(raw_tree<T> &&other) {
    value_ = std::move(other.value_);
    parent_ = other.parent_;
    children_ = std::move(other.children_);
    if (children_[_left]) { children_[_left]->parent_ = this; }
    if (children_[_right]) { children_[_right]->parent_ = this; }

    if (parent_ == &other) {
        parent_ = this;
    } else if (other.is_side<side::left>()) {
        parent_->children_[_left].reset();
    } else {
        parent_->children_[_right].reset();
    }

    return *this;
}

template <typename T>
void raw_tree<T>::swap(raw_tree<T> &other) {
    std::swap(value_, other.value_);
    std::swap(children_, other.children_);
    if (children_[_left]) { children_[_left]->parent_ = this; }
    if (children_[_right]) { children_[_right]->parent_ = this; }

    if (other.children_[_left]) { other.children_[_left]->parent_ = &other; }
    if (other.children_[_right]) { other.children_[_right]->parent_ = &other; }
}

template <typename T>
void swap(raw_tree<T> &left, raw_tree<T> &right) {
    left.swap(right);
}

template <typename T>
const T &raw_tree<T>::operator*() const {
    return value_;
}

template <typename T>
T &raw_tree<T>::operator*() {
    return const_cast<T &>(
        const_cast<const raw_tree<T> *>(this)->raw_tree<T>::operator*());
}

template <typename T>
bool raw_tree<T>::has_parent() const {
    return parent_ != this;
}

template <typename T>
const raw_tree<T> &raw_tree<T>::parent() const {
    return *parent_;
}

template <typename T>
raw_tree<T> &raw_tree<T>::parent() {
    return *parent_;
}

template <typename T>
template <side wing>
bool raw_tree<T>::is_side() const {
    return parent_->children_[as_int(wing)].get() == this;
}

template <typename T>
bool raw_tree<T>::is_side(side wing) const {
    SWITCH_ON_SIDE(is_side);
}

template <typename T>
template <side wing>
bool raw_tree<T>::has_child() const {
    return (bool)children_[as_int(wing)];
}

template <typename T>
bool raw_tree<T>::has_child(side wing) const {
    SWITCH_ON_SIDE(has_child);
}

template <typename T>
template <side wing>
const raw_tree<T> &raw_tree<T>::child() const {
    return *children_[as_int(wing)];
}

template <typename T>
const raw_tree<T> &raw_tree<T>::child(side wing) const {
    SWITCH_ON_SIDE(child);
}

template <typename T>
template <side wing>
raw_tree<T> &raw_tree<T>::child() {
    return const_cast<raw_tree<T> &>(
        const_cast<const raw_tree<T> *>(this)->raw_tree<T>::child<wing>());
}

template <typename T>
raw_tree<T> &raw_tree<T>::child(side wing) {
    return const_cast<raw_tree<T> &>(
        const_cast<const raw_tree<T> *>(this)->raw_tree<T>::child(wing));
}

template <typename T>
template <side C, side GC>
void raw_tree<T>::reshape() {
    // NOTE: Variable names based on pre-rotation positions.
    std::swap(**children_[as_int(!C)], value_);

    std::unique_ptr<raw_tree<T>> child_node =
        std::exchange(children_[as_int(!C)],
                      std::move(children_[as_int(!C)]->children_[as_int(!C)]));
    if (children_[as_int(!C)]) { children_[as_int(!C)]->parent_ = this; }

    child_node->children_[as_int(!C)] =
        std::move(child_node->children_[as_int(C)]);

    if constexpr (GC == C) {
        if (children_[as_int(C)]) {
            children_[as_int(C)]->parent_ = child_node.get();
            child_node->children_[as_int(C)] = std::move(children_[as_int(C)]);
        }
        children_[as_int(C)] = std::move(child_node);
    } else {
        if (children_[as_int(C)]) {
            if (children_[as_int(C)]->children_[as_int(GC)]) {
                children_[as_int(C)]->children_[as_int(GC)]->parent_ =
                    child_node.get();
                child_node->children_[as_int(C)] =
                    std::move(children_[as_int(C)]->children_[as_int(GC)]);
            }
            child_node->parent_ = children_[as_int(C)].get();
            children_[as_int(C)]->children_[as_int(GC)] = std::move(child_node);
        } else {
            child_node->parent_ = this;
            children_[as_int(C)] = std::move(child_node);
        }
    }
}

template <typename T>
template <side wing>
void raw_tree<T>::rotate() {
    return reshape<wing, wing>();
}

template <typename T>
void raw_tree<T>::rotate(side wing) {
    SWITCH_ON_SIDE(rotate);
}

template <typename T>
void raw_tree<T>::flip() {
    using std::swap;
    swap(children_[_left], children_[_right]);
}

template <typename T>
template <side wing>
void raw_tree<T>::replace(raw_tree<T> &&child) {
    this->children_[as_int(wing)] =
        std::make_unique<raw_tree<T>>(std::move(child));
    this->children_[as_int(wing)]->parent_ = this;
}

template <typename T>
void raw_tree<T>::replace(side wing, raw_tree<T> &&child) {
    SWITCH_ON_SIDE(replace, std::move(child));
}

template <typename T>
template <side wing, typename... Args>
void raw_tree<T>::emplace(Args &&... args) {
    this->children_[as_int(wing)] = std::make_unique<raw_tree<T>>(args...);
    this->children_[as_int(wing)]->parent_ = this;
}

template <typename T>
template <typename... Args>
void raw_tree<T>::emplace(side wing, Args &&... args) {
    SWITCH_ON_SIDE(emplace, args...);
}

template <typename T>
template <side wing, side gc_wing, typename... Args>
void raw_tree<T>::splice(Args &&... args) {
    auto inserted = ::std::make_unique<raw_tree<T>>(args...);
    inserted->children_[as_int(gc_wing)] =
        ::std::move(this->children_[as_int(wing)]);
    inserted->children_[as_int(gc_wing)]->parent_ = inserted.get();

    this->children_[as_int(wing)] = ::std::move(inserted);
    this->children_[as_int(wing)]->parent_ = this;
}

template <typename T>
template <side wing>
raw_tree<T> raw_tree<T>::detach() {
    auto detached = std::move(children_[as_int(wing)]);
    detached->parent_ = detached.get();
    return raw_tree<T>(std::move(*detached));
}

template <typename T>
raw_tree<T> raw_tree<T>::detach(side wing) {
    SWITCH_ON_SIDE(detach);
}

template <typename T>
template <traversal_order order, side wing>
iterator<raw_tree<T>, order, wing> raw_tree<T>::begin() {
    return iterator<raw_tree<T>, order, wing>(*this);
}

template <typename T>
template <traversal_order order, side wing>
iterator<const raw_tree<T>, order, wing> raw_tree<T>::begin() const {
    return iterator<const raw_tree<T>, order, wing>(*this);
}

template <typename T>
template <traversal_order order, side wing>
iterator<raw_tree<T>, order, wing> raw_tree<T>::end() {
    return iterator<raw_tree<T>, order, wing>(*this, -1);
}

template <typename T>
template <traversal_order order, side wing>
iterator<const raw_tree<T>, order, wing> raw_tree<T>::end() const {
    return iterator<const raw_tree<T>, order, wing>(*this, -1);
}

template <typename T>
size_t raw_tree<T>::size() const {
    return 1 + (has_child<side::left>() ? children_[_left]->size() : 0) +
           (has_child<side::right>() ? children_[_right]->size() : 0);
}
