#include "Parser.h"
#include "ASTVisitor.h"
#include <assert.h>

namespace AST {

AST JSONToASTParser::parseHelper() { return AST{parseRules(json[0]["rules"])}; }

std::unique_ptr<Rules> JSONToASTParser::parseRules(const Json &json) {

  auto rulePtr = std::make_unique<Rules>();

  if(json.empty())
    return rulePtr;
  
  for (const auto &rule : json) {
    auto &&x = parseRule(rule);
    rulePtr->appendChild(std::move(x));
  }
  return rulePtr;
}

std::unique_ptr<ASTNode> JSONToASTParser::parseRule(const Json &json) {

  auto itr = strToRules.find(json["rule"]);
  if(itr == strToRules.end())
    return std::unique_ptr<ASTNode>{};

  switch(itr->second){
    case RuleID::MESSAGE:
      return parseMessage(json);
      break;
    case RuleID::GLOBAL_MESSAGE:
      return parseGlobalMessage(json);
      break;
    case RuleID::SCORES:
      return parseScores(json);
      break;
    case RuleID::PARALLEL_FOR:
      return parseParallelFor(json);
      break;
    case RuleID::FOR_EACH:
      return parseForEach(json);
      break;
    case RuleID::LOOP:
      return parseLoop(json);
      break;
    case RuleID::IN_PARALLEL:
      return parseInParallel(json);
      break;
    case RuleID::SWITCH:
      return parseSwitch(json);
      break;
    case RuleID::WHEN:
      return parseWhen(json);
      break;
    case RuleID::REVERSE:
      return parseReverse(json);
      break;
    case RuleID::EXTEND:
      return parseExtend(json);
      break;
    case RuleID::SHUFFLE:
      return parseShuffle(json);
      break;
    case RuleID::SORT:
      return parseSort(json);
      break;
    case RuleID::DEAL:
      return parseDeal(json);
      break;
    case RuleID::DISCARD:
      return parseDiscard(json);
      break;
    case RuleID::ADD:
      return parseAdd(json);
      break;
    case RuleID::TIMER:
      return parseTimer(json);
      break;
    case RuleID::INPUT_CHOICE:
      return parseInputChoice(json);
      break;
    case RuleID::INPUT_TEXT:
      return parseInputText(json);
      break;
    case RuleID::INPUT_VOTE:
      return parseInputVote(json);
      break;
    }
}

std::unique_ptr<FormatNode> JSONToASTParser::parseFormatNode(const std::string &str) {

  return std::make_unique<FormatNode>(std::move(str));
}

std::unique_ptr<Message> JSONToASTParser::parseMessage(const Json &json){

  auto &&to = parseVariable(json["to"]);
  auto &&value = parseFormatNode(json["value"]);
  return std::make_unique<Message>(std::move(to), std::move(value));
}

std::unique_ptr<GlobalMessage>
JSONToASTParser::parseGlobalMessage(const Json &json) {

  auto &&format = parseFormatNode(json["value"]);
  return std::make_unique<GlobalMessage>(std::move(format));
}

std::unique_ptr<Scores> JSONToASTParser::parseScores(const Json &json){

  auto &&score = parseVariable(json["score"]);
  auto ascending = json["ascending"];
  return std::make_unique<Scores>(std::move(score), ascending);
}

std::unique_ptr<ParallelFor>
JSONToASTParser::parseParallelFor(const Json &json) {

  auto &&var = parseVariable(json["list"]);
  auto &&varDec = parseVariable(json["element"]);
  auto &&rules = parseRules(json["rules"]);

  return std::make_unique<ParallelFor>(std::move(var), std::move(varDec),
                                       std::move(rules));
}

std::unique_ptr<Variable> JSONToASTParser::parseVariable(const std::string &str) {

  return std::make_unique<Variable>(std::move(str));
}

std::unique_ptr<Condition> JSONToASTParser::parseCondition(const std::string &str){

  return std::make_unique<Condition>(std::move(str));
}

std::unique_ptr<AllSwitchCases> JSONToASTParser::parseSwitchCases(const Json &json){

  auto casePtr = std::make_unique<AllSwitchCases>();
  if(json.empty())
    return casePtr;

  for (const auto &node : json) {
    auto &&value = parseFormatNode(node["case"]);
    auto &&rules = parseRules(node["rules"]);
    auto &&x = std::make_unique<SwitchCase>(std::move(value), std::move(rules));
    casePtr->appendChild(std::move(x));
  }
  return casePtr;
}

std::unique_ptr<AllWhenCases> JSONToASTParser::parseWhenCases(const Json &json){

  auto casePtr = std::make_unique<AllWhenCases>();
  if(json.empty())
    return casePtr;

  for (const auto &node : json) {
    auto &&cond = parseCondition(node["condition"]);
    auto &&rules = parseRules(node["rules"]);
    auto &&x = std::make_unique<WhenCase>(std::move(cond), std::move(rules));
    casePtr->appendChild(std::move(x));
  }
  return casePtr;
}

std::unique_ptr<ForEach> JSONToASTParser::parseForEach(const Json &json){

  auto &&var = parseVariable(json["list"]);
  auto &&varDec = parseVariable(json["element"]);
  auto &&rules = parseRules(json["rules"]);

  return std::make_unique<ForEach>(std::move(var), std::move(varDec),
                                       std::move(rules));
}

std::unique_ptr<Loop> JSONToASTParser::parseLoop(const Json &json){

  if(json.contains("until")){
    auto &&cond = parseCondition(json["until"]);
    auto &&rules = parseRules(json["rules"]);

    return std::make_unique<Loop>(std::move(cond), std::move(rules));
  }else{
    auto &&cond = parseCondition(json["while"]);
    auto &&rules = parseRules(json["rules"]);

    return std::make_unique<Loop>(std::move(cond), std::move(rules));
  }
}

std::unique_ptr<InParallel> JSONToASTParser::parseInParallel(const Json &json){
  //may be subject to change to allow coroutines to facilitate the array of rules within
  auto &&rules = parseRules(json["rules"]);

  return std::make_unique<InParallel>(std::move(rules));
}

std::unique_ptr<Switch> JSONToASTParser::parseSwitch(const Json &json){

  auto &&value = parseFormatNode(json["value"]);
  auto &&list = parseVariable(json["list"]);
  auto &&cases = parseSwitchCases(json["cases"]);

  return std::make_unique<Switch>(std::move(value), std::move(list), std::move(cases));
}

std::unique_ptr<When> JSONToASTParser::parseWhen(const Json &json){

  auto &&cases = parseWhenCases(json["cases"]);

  return std::make_unique<When>(std::move(cases));
}

std::unique_ptr<Reverse> JSONToASTParser::parseReverse(const Json &json){

  auto &&list = parseVariable(json["list"]);

  return std::make_unique<Reverse>(std::move(list));
}
std::unique_ptr<Extend> JSONToASTParser::parseExtend(const Json &json){

  auto &&target = parseVariable(json["target"]);
  auto &&list = parseVariable(json["list"]);

  return std::make_unique<Extend>(std::move(target), std::move(list));
}
std::unique_ptr<Shuffle> JSONToASTParser::parseShuffle(const Json &json){

  auto &&list = parseVariable(json["list"]);

  return std::make_unique<Shuffle>(std::move(list));
}
std::unique_ptr<Sort> JSONToASTParser::parseSort(const Json &json){

  auto &&list = parseVariable(json["list"]);
  auto sortPointer = std::make_unique<Sort>(std::move(list));
  if(json.contains("key")){
    auto&& key = parseVariable(json["key"]);
    sortPointer->addAttribute(std::move(key));
  }

  return sortPointer;
}
std::unique_ptr<Deal> JSONToASTParser::parseDeal(const Json &json){

  auto &&from = parseVariable(json["from"]);
  auto &&to = parseVariable(json["to"]);

  return std::make_unique<Deal>(std::move(from), std::move(to), json["count"]);
}
std::unique_ptr<Discard> JSONToASTParser::parseDiscard(const Json &json){

  auto &&from = parseVariable(json["from"]);

  return std::make_unique<Discard>(std::move(from), json["count"]);
}

std::unique_ptr<Add> JSONToASTParser::parseAdd(const Json &json){

  auto &&var = parseVariable(json["to"]);

  return std::make_unique<Add>(std::move(var), json["value"]);
}
std::unique_ptr<Timer> JSONToASTParser::parseTimer(const Json &json){

  auto &&mode = parseVariable(json["mode"]);
  auto &&rules = parseRules(json["rules"]);
  auto timerPointer = std::make_unique<Timer>(json["duration"], std::move(mode), std::move(rules));

  if(json.contains("flag")){
    auto&& flag = parseCondition(json["flag"]);
    timerPointer->addFlag(std::move(flag));
  }

  return timerPointer;
}

std::unique_ptr<InputChoice> JSONToASTParser::parseInputChoice(const Json &json){

  auto &&prompt = parseFormatNode(json["prompt"]);
  auto &&to = parseVariable(json["to"]);
  auto &&choices = parseVariable(json["choices"]);
  auto &&result = parseVariable(json["result"]);

  auto inputChoicePtr = std::make_unique<InputChoice>(std::move(prompt), std::move(to), std::move(choices), std::move(result));

  if(json.contains("timeout")){
    inputChoicePtr->setTimeout(json["timeout"]);
  }
  return inputChoicePtr;
}
std::unique_ptr<InputText> JSONToASTParser::parseInputText(const Json &json){

  auto &&prompt = parseFormatNode(json["prompt"]);
  auto &&to = parseVariable(json["to"]);
  auto &&result = parseVariable(json["result"]);

  auto inputTextPtr = std::make_unique<InputText>(std::move(prompt), std::move(to), std::move(result));

  if(json.contains("timeout")){
    inputTextPtr->setTimeout(json["timeout"]);
  }
  return inputTextPtr;
}
std::unique_ptr<InputVote> JSONToASTParser::parseInputVote(const Json &json){

  auto &&prompt = parseFormatNode(json["prompt"]);
  auto &&to = parseVariable(json["to"]);
  auto &&choices = parseVariable(json["choices"]);
  auto &&result = parseVariable(json["result"]);

  auto inputVotePtr = std::make_unique<InputVote>(std::move(prompt), std::move(to), std::move(choices), std::move(result));

  if(json.contains("timeout")){
    inputVotePtr->setTimeout(json["timeout"]);
  }
  return inputVotePtr;
}
} // namespace AST
