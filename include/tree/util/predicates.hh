#include <random>

template <typename T>
std::hash<T> stable_random<T>::hash_computer_;

template <typename T>
size_t stable_random<T>::seed_ = (std::random_device())();

template <typename T>
bool stable_random<T>::operator()(const T &left, const T &right) const {
    return (hash_computer_(left) ^ seed_) < (hash_computer_(right) ^ seed_);
}
