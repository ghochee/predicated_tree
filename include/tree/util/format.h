#ifndef TREE_FORMAT_H
#define TREE_FORMAT_H

// 'Format' tree objects for printability.
// TODO(ghochee): Formatting options, indent level coloring?
class format {
  public:
    // Prints (using non-ascii characters) 'tree'. Left-child nodes are
    // connected via thick lines, right children are thin lines.
    template <typename Tree>
    std::string print(Tree &tree) const;

  private:
    template <typename Tree>
    void print_internal(Tree &tree, std::string &output) const;
};

template <typename T>
std::ostream &operator<<(std::ostream &out, raw_tree<T> &t);

#include "tree/format.hh"

#endif  // TREE_FORMAT_H
