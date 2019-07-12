template <typename A, side wing>
class traverser<A, traversal_order::pre, wing> {
  public:
    static void next(A &a);
};

template <typename A, side wing>
void traverser<A, traversal_order::pre, wing>::next(A &a) {
    if (!a) {
        a.root();
        return;
    }

    if (a.template down<wing>() || a.template down<!wing>()) { return; }

    for (; !a.is_root() && (a->template is_side<!wing>() ||
                            !a->parent().template has_child<!wing>());
         a.up()) {}
    a.up();

    if (a) { a.template down<!wing>(); }
}

template <typename A, side wing>
class traverser<A, traversal_order::in, wing> {
  public:
    static void next(A &a);
};

template <typename A, side wing>
void traverser<A, traversal_order::in, wing>::next(A &a) {
    if (!a) {
        for (; a.template down<wing>();) {};
        return;
    }

    if (a->template has_child<!wing>()) {
        for (a.template down<!wing>(); a.template down<wing>();) {}
        return;
    }

    for (; !a.is_root() && a->template is_side<!wing>(); a.up()) {}
    a.up();
}

template <typename A, side wing>
class traverser<A, traversal_order::post, wing> {
  public:
    static void next(A &a);
};

template <typename A, side wing>
void traverser<A, traversal_order::post, wing>::next(A &a) {
    if (!a) {
        for (; a.template down<wing>() || a.template down<!wing>();) {}
        return;
    }

    if (a.is_root()) { return a.up(); }

    if (a->template is_side<!wing>() ||
        !a->parent().template has_child<!wing>()) {
        return a.up();
    }

    for (a.up(), a.template down<!wing>();
         a.template down<wing>() || a.template down<!wing>();) {}
}
