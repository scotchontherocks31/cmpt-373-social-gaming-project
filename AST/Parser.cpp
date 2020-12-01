#include "Parser.h"
#include "ASTVisitor.h"
#include "CFGParser.h"
#include "ExpressionASTParser.h"
#include <algorithm>
#include <assert.h>
#include <exception>
#include <iostream>
#include <map>
#include <regex>

namespace AST {

AST JSONToASTParser::parseHelper() { return AST{parseRules(json[0]["rules"])}; }

std::unique_ptr<Rules> JSONToASTParser::parseRules(const Json &json) {

  auto rulePtr = std::make_unique<Rules>();
  for (const auto &rule : json) {
    auto &&x = parseRule(rule);
    rulePtr->appendChild(std::move(x));
  }
  return rulePtr;
}

std::unique_ptr<ASTNode> JSONToASTParser::parseRule(const Json &json) {
  if (json["rule"] == "global-message") {
    return parseGlobalMessage(json);
  } else if (json["rule"] == "parallelfor") {
    return parseParallelFor(json);
  } else {
    assert("rule not implemented yet");
    return std::unique_ptr<ASTNode>{};
  }
}

std::regex PLACEHOLDER("\\{([^}]*)\\}");

class myexception : public std::exception {
  virtual const char *what() const throw() {
    return "Expression is invalid in Format Parsing";
  }
};

std::optional<std::string> getFirstMatch(std::string str) {
  std::smatch match;
  if (std::regex_search(str, match, PLACEHOLDER)) {
    return match.str(0);
  }
  return {};
}

std::vector<std::unique_ptr<ExpressionNode>>
extractExpressionsHelper(std::string str) {

  std::vector<std::unique_ptr<ExpressionNode>> expressions;

  std::optional<std::string> matchOpt = getFirstMatch(str);
  while (matchOpt) {
    try {

      auto match = (matchOpt.value()).substr(1, (matchOpt.value()).size() - 2);
      auto start_position_to_erase = str.find(match);
      str.erase(start_position_to_erase - 1, match.size() + 2);

      ExpressionASTParser rdp(match);
    
      expressions.push_back(rdp.parse_S());
      matchOpt = getFirstMatch(str);

    } catch (const std::bad_optional_access &e) {
      throw myexception();
    }
  }

  return expressions;
}

std::unique_ptr<FormatNode> JSONToASTParser::parseFormatNode(const Json &json) {
  std::vector<std::unique_ptr<ExpressionNode>> expressions =
      extractExpressionsHelper(json["value"]);
  return std::make_unique<FormatNode>(json["value"], std::move(expressions));
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

std::unique_ptr<ASTNode>
JSONToASTParser::parseExpression(const std::string &str) {

  ExpressionASTParser expressionParse(str);
  return expressionParse.parse_S();
}

} // namespace AST
