#include <memory>

// This class is private+internal to tree.
template <typename T>
class tree<T>::Node {
  private:
    class I {
      public:
        constexpr bool operator()(const T &, const T &) const { return false; };
    };

  public:
    Node() = delete;
    Node(T &&value) : value_(std::move(value)) {}
    Node(const T &value) : value_(value) {}

    template <typename HeightComparator = I, typename LeftComparator = I>
    static std::unique_ptr<tree<T>::Node> merge(
        std::unique_ptr<tree<T>::Node> &&first,
        std::unique_ptr<tree<T>::Node> &&second,
        const HeightComparator &heightComparator = HeightComparator(),
        const LeftComparator &leftComparator = LeftComparator());

    T &operator*();

    template <typename Comparator>
    static std::tuple<std::unique_ptr<tree<T>::Node>,
                      std::unique_ptr<tree<T>::Node>>
    myminmax(std::unique_ptr<tree<T>::Node> &&first,
             std::unique_ptr<tree<T>::Node> &&second,
             const Comparator &comparator);

    T value_;
    tree<T>::Node *parent_ = nullptr;
    std::unique_ptr<tree<T>::Node> left_ = nullptr, right_ = nullptr;
};

#include "tree/node.hh"
