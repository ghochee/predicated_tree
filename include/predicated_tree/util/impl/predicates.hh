#include <random>
#include <type_traits>

namespace detangled {

template <typename T>
std::hash<T> stable_random<T>::hash_computer_;

template <typename T>
size_t stable_random<T>::seed_ = (std::random_device())();

template <typename T>
bool stable_random<T>::operator()(const T &left, const T &right) const {
    return (hash_computer_(left) ^ seed_) < (hash_computer_(right) ^ seed_);
}

template <typename T>
bool more_even(const T &a, const T &b) {
    static_assert(std::is_integral<T>::value,
                  "Calling more even on non integral type");
    return (a ^ (a - 1)) > (b ^ (b - 1));
}

}  // namespace detangled
