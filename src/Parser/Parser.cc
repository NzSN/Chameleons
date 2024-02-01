#include <memory>
#include "Parser-inl.h"

namespace Parser {

std::optional<Base::ParseTreeT_DYNAMIC>
ParserRuntimeSelect(unsigned lang, std::istringstream& sentences) {
  switch (lang) {
  case Base::ParseTree<Base::Antlr4Node>::TESTLANG: {

    std::unique_ptr<Base::ParseTree<Base::Antlr4Node>>
      t = ParserSelect<Base::ParseTree<Base::Antlr4Node>::TESTLANG>
      ::parser::parse<
        Base::ParseTree<Base::Antlr4Node>,
        Base::DYNAMIC>(&sentences);

    return std::move(t);
  }
  default:
    return std::nullopt;
  };
}

} // Parser
