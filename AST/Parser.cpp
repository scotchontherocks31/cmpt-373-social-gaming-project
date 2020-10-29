#include "Parser.h"
#include <assert.h>

namespace AST {

AST JSONToASTParser::parseHelper() {

  return AST{parseRules(this->json["rules"])};
}

std::unique_ptr<Rules> JSONToASTParser::parseRules(const Json &json) {
  auto list = std::vector<std::unique_ptr<ASTNode>>{};
  for (const auto &rule : json) {
    std::cout << rule.dump(4) << std::endl;
    list.push_back(parseRule(rule));
  }
  return std::make_unique<Rules>(list);
}

std::unique_ptr<ASTNode> JSONToASTParser::parseRule(const Json &json) {
  if (json["rule"] == "global-message") {
    return parseGlobalMessage(json);
  } else if (json["rule"] == "parallelfor") {
    return parseParallelFor(json);
  } else {
      assert(false);
  }
}

std::unique_ptr<FormatNode> JSONToASTParser::parseFormatNode(const Json &json) {

  return std::make_unique<FormatNode>(json["value"]);
}

std::unique_ptr<GlobalMessage>
JSONToASTParser::parseGlobalMessage(const Json &json) {

  auto &&format = parseFormatNode(json);
  return std::make_unique<GlobalMessage>(std::move(format));
}

std::unique_ptr<ParallelFor>
JSONToASTParser::parseParallelFor(const Json &json) {

  auto &&var = parseVariable(json);
  auto &&varDec = parseVarDeclaration(json);
  auto &&rules = parseRules(json["rules"]);

  return std::make_unique<ParallelFor>(std::move(var), std::move(varDec),
                                       std::move(rules));
}

std::unique_ptr<VarDeclaration>
JSONToASTParser::parseVarDeclaration(const Json &json) {

  return std::make_unique<VarDeclaration>(json["element"]);
}

std::unique_ptr<Variable> JSONToASTParser::parseVariable(const Json &json) {

  return std::make_unique<Variable>(json["list"]);
}
} // namespace AST
