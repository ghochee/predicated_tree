template <typename T>
tree<T>::tree(std::function<bool(const T&, const T&)> isTall,
              std::function<bool(const T&, const T&)> isLeft)
    : isTall_(isTall), isLeft_(isLeft) {}
