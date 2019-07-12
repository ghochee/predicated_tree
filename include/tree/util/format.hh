#include <iostream>
#include <sstream>
#include <vector>

template <typename Tree>
std::string format::print(Tree &tree) const {
    std::string output;
    print_internal(tree, output);
    return output;
}

template <typename Tree>
void format::print_internal(Tree &tree, std::string &output) const {
    std::vector<uint32_t> right_childed_ancestors;
    for (auto it = tree.prelbegin(); it != tree.prelend(); ++it) {
        uint32_t depth = it.depth();
        uint32_t cumulative = 0;
        for (auto jt = right_childed_ancestors.begin();
             jt != right_childed_ancestors.end(); ++jt) {
            for (; cumulative < 3 * *jt; ++cumulative) { output += " "; }

            if (*jt != depth - 1) { output += "│"; }
        }

        for (; (cumulative + 3) < 3 * depth; ++cumulative) { output += " "; }

        if (depth) {
            if (it->template is_side<side::right>()) {
                output += "└";
                right_childed_ancestors.pop_back();
            } else if (right_childed_ancestors.size() &&
                       right_childed_ancestors.back() == depth - 1) {
                output += "┝";
            } else {
                output += "┕";
            }

            if (it->template is_side<side::right>()) {
                output += "──";
            } else {
                output += "━━";
            }
        }
        std::stringstream ss;
        ss << *it << "\n";
        output += ss.str();

        if (it->template has_child<side::right>()) {
            right_childed_ancestors.push_back(depth);
        }
    }
}

template <typename T>
std::ostream &operator<<(std::ostream &out, raw_tree<T> &t) {
    format f;
    out << f.print(t);
    return out;
}
