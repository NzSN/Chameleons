
#ifndef GENERIC_PARSETREE_H
#define GENERIC_PARSETREE_H

#include <type_traits>
#include <memory>
#include <ranges>
#include <tuple>
#include <vector>
#include <concepts>
#include <functional>

#include "Concepts/n_ary_tree.h"
#include "TreeLayer.h"

namespace Base {

/////////////////////////////////////////////////////////////////////////////
//                                 Concepts                                //
/////////////////////////////////////////////////////////////////////////////
template<typename T>
concept Layer = std::equality_comparable<T>;

template<typename T>
concept GPTMappable =
  Layer<T> &&
  Concepts::NAryTree::NAryTree<T> &&
  requires(T t) {
    // Source information
    { t.getText() } -> std::same_as<std::string>;
    { t.setNode(t) } -> std::same_as<bool>;
};


/////////////////////////////////////////////////////////////////////////////
//                             ParseTree                            //
/////////////////////////////////////////////////////////////////////////////
template<Layer T>
class ParseTree: public TreeLayer<ParseTree<T>> {
public:
  enum SUPPORTED_LANGUAGE {
    MINIMUM_LANG_INDEX = 0,
    TYPESCRIPT = 0,
    CPP = 1,
    // Testing purposes
    TESTLANG = 2,
    WGSL = 3,
    NUM_OF_LANG_SUPPORTED,
  };

  using Childs = std::vector<std::unique_ptr<ParseTree>>;
  using GPTIterator = Childs::iterator;

  // Terminal
  ParseTree(const T& meta):
    parent{nullptr}, metaRef{meta} {}
  ParseTree(const ParseTree& other):
    parent{other.parent}, metaRef(other.metaRef) {

    for (auto& c: other.childs_) {
      this->addChild(
        std::make_unique<ParseTree>(*c));
    }
  }

  ParseTree& addChild(
    std::unique_ptr<ParseTree>&& child) {
    child->parent = this;
    child->setDepth(this->getDepth() + 1);
    childs_.push_back(std::move(child));

    return *childs_.back();
  }

  ParseTree* parent;

  Childs& getChildren() {
    return childs_;
  };

  std::optional<ParseTree*> getChild(int i) {
    if (childs_.size() < i) {
      return std::nullopt;
    } else {
      return childs_[i].get();
    }
  }

  bool operator==(const ParseTree& other) const;

  ParseTree* select(
    std::function<bool(ParseTree&)> predicate);

  void traverse(
    std::function<bool(ParseTree&)> proc);

  // This procedure require modification to
  // underlaying tree.
  // Only when T is NAryTree with setNode().
  bool setNode(const ParseTree& other)
    requires Concepts::NAryTree::NAryTree<T> &&
             requires(T t, const T arg) {
               { t.setNode(arg) } -> std::convertible_to<bool>;
             }
  {
    return const_cast<T&>(metaRef).setNode(other.getMeta());
  }

  std::unique_ptr<ParseTree> clone() const
    requires requires(T t) {
      { t.clone() } -> std::same_as<std::unique_ptr<T>>;
    }
  {
    std::unique_ptr<T> metaCopy = metaRef.clone();

    std::unique_ptr<ParseTree> copy =
      this->template mapping<T, Base::DYNAMIC>(
        *metaCopy);
    copy->metaUnique_ = std::move(metaCopy);

    return copy;
  }


  // For convenience, ParseTree will require that
  std::string getText() const {
    return const_cast<T&>(metaRef).getText();
  }

  const T& getMeta() const {
    return metaRef;
  }

  T& getMetaMutable() {
    return const_cast<T&>(metaRef);
  }

private:
  friend struct ParseTreeTest;

  const T& metaRef;

  Childs childs_;

  // For clone purposes
  std::unique_ptr<T> metaUnique_;
};

} // Base

#endif /* GENERIC_PARSETREE_H */
