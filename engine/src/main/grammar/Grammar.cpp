/*
 * Grammar.cpp
 *
 *  Created on: 09/10/2011
 *      Author: marrony
 */

#include "grammar/Grammar.h"

#include <string.h>
#include <stack>
#include <iostream>

namespace engine {

	void Token::operator=(const TokenFragment& tokenInter) {
		token->setLexeme(tokenInter.sentence);
		grammar->registerToDelete(tokenInter.sentence);
	}

	void Token::operator=(const Terminal& terminal) {
		token->setLexeme(terminal.sentence);
		grammar->registerToDelete(terminal.sentence);
	}

	void Rule::operator=(const TokenFragment& token) {
		sentence->setProduction(token.sentence);
		grammar->registerToDelete(token.sentence);
	}

	void Rule::operator=(const RuleInter& rule) {
		sentence->setProduction(rule.sentence);
		grammar->registerToDelete(rule.sentence);
	}

/////////////////////////////////////////////////////////////////
	class CollectAllChildrenVisitor : public AllSentencesVisitor {
	public:
		virtual void visit(TokenSentence* sentence) {
			children.insert(sentence);

			if(sentence->getLexeme())
				sentence->getLexeme()->accept(this);
		}

		virtual void visit(RuleSentence* sentence) {
			children.insert(sentence);

			if(sentence->getProduction())
				sentence->getProduction()->accept(this);
		}

		virtual void visit(OrSentence* sentence) {
			if(children.count(sentence) == 1)
				return;

			children.insert(sentence);
			sentence->getFirst()->accept(this);
			sentence->getSecond()->accept(this);
		}

		virtual void visit(AndSentence* sentence) {
			if(children.count(sentence) == 1)
				return;

			children.insert(sentence);
			sentence->getFirst()->accept(this);
			sentence->getSecond()->accept(this);
		}

		virtual void visit(NotSentence* sentence) {
		}

		virtual void visit(ClosureSentence* sentence) {
			if(children.count(sentence) == 1)
				return;

			children.insert(sentence);
			sentence->getSentence()->accept(this);
		}

		virtual void visit(TerminalSentence* sentence) {
			children.insert(sentence);
		}

		std::set<Sentence*> children;
	};

	class FirstVisitor : public AllSentencesVisitor {
	public:
		FirstVisitor(Sentence* epsilon) :
				epsilon(epsilon) {
		}

		virtual void visit(TokenSentence* sentence) {
			first.insert(sentence);
		}

		virtual void visit(RuleSentence* sentence) {
			sentence->getProduction()->accept(this);
		}

		virtual void visit(OrSentence* sentence) {
			sentence->getFirst()->accept(this);
			sentence->getSecond()->accept(this);
		}

		virtual void visit(AndSentence* sentence) {
			FirstVisitor visitor(epsilon);

			sentence->getFirst()->accept(&visitor);

			if(visitor.first.count(epsilon) != 0) {
				sentence->getSecond()->accept(&visitor);
			}

			first.insert(visitor.first.begin(), visitor.first.end());
		}

		virtual void visit(NotSentence* sentence) {
		}

		virtual void visit(ClosureSentence* sentence) {
			first.insert(epsilon);
			sentence->getSentence()->accept(this);
		}

		virtual void visit(TerminalSentence* sentence) {
			first.insert(sentence);
		}

		std::set<Sentence*> first;
		Sentence* epsilon;
	};

	class SequenceVisitor : public AllSentencesVisitor {
	public:
		virtual void visit(TokenSentence* sentence) {
			sentences.push_back(sentence);
		}

		virtual void visit(RuleSentence* sentence) {
			sentences.push_back(sentence);
		}

		virtual void visit(OrSentence* sentence) {
		}

		virtual void visit(AndSentence* sentence) {
			sentence->getFirst()->accept(this);
			sentence->getSecond()->accept(this);
		}

		virtual void visit(NotSentence* sentence) {
		}

		virtual void visit(ClosureSentence* sentence) {
		}

		virtual void visit(TerminalSentence* sentence) {
			sentences.push_back(sentence);
		}

		std::vector<Sentence*> sentences;
	};

	class ProductionVisitor : public AllSentencesVisitor {
	public:
		ProductionVisitor(std::vector<Production*>& productions) :
				rule(0), productions(productions) {
		}

		virtual void visit(TokenSentence* sentence) {
			visitSentence(sentence);
		}

		virtual void visit(RuleSentence* sentence) {
			if(rule) {
				Production* production = new Production(rule, sentence->action());
				production->sentences.push_back(sentence);
				productions.push_back(production);
			} else {
				rule = sentence;
				rule->getProduction()->accept(this);
				rule = 0;
			}
		}

		virtual void visit(OrSentence* sentence) {
			sentence->getFirst()->accept(this);
			sentence->getSecond()->accept(this);
		}

		virtual void visit(AndSentence* sentence) {
			SequenceVisitor sequence;
			sentence->accept(&sequence);

			productions.push_back(new Production(rule, sequence.sentences, sentence->action()));
		}

		virtual void visit(NotSentence* sentence) {
		}

		virtual void visit(ClosureSentence* sentence) {
			visitSentence(sentence);
		}

		virtual void visit(TerminalSentence* sentence) {
			visitSentence(sentence);
		}

		void visitSentence(Sentence* sentence) {
			Production* production = new Production(rule, sentence->action());
			production->sentences.push_back(sentence);
			productions.push_back(production);
		}

		RuleSentence* rule;
		std::vector<Production*>& productions;
	};
/////////////////////////////////////////////////////////////////

	Grammar::Grammar() :
			rulesByName(), tokensByName(), firstRule(0), epsilon(0), endmarker(0) {
	}

	Grammar::~Grammar() {
		std::set<Sentence*>::iterator ite;

		for(ite = sentencesToDelete.begin(); ite != sentencesToDelete.end(); ite++) {
			delete *ite;
		}
	}

	Rule Grammar::createRule(const char* ruleName) {
		RuleSentence* rule = new RuleSentence(ruleName);

		rulesByName[ruleName] = rule;
		allRules.insert(rule);
		allGrammarSymbols.push_back(rule);

		registerToDelete(rule);

		if(!firstRule)
			firstRule = rule;

		return Rule(rule, this);
	}

	Token Grammar::createToken(const std::string& tokenName) {
		TokenSentence* token = new TokenSentence(tokenName);

		tokensByName[tokenName] = token;
		allGrammarSymbols.push_back(token);

		registerToDelete(token);

		return Token(token, this);
	}

	void Grammar::registerToDelete(Sentence* sentence) {
		CollectAllChildrenVisitor visitor;

		sentence->accept(&visitor);

		sentencesToDelete.insert(visitor.children.begin(), visitor.children.end());
	}

	RuleSentence* Grammar::findRule(const std::string& ruleName) {
		return rulesByName[ruleName];
	}

	TokenSentence* Grammar::findToken(const std::string& tokenName) {
		return tokensByName[tokenName];
	}

	TokenSentence* Grammar::getEpsilon() {
		if(!epsilon) {
			epsilon = new TokenSentence("epsilon");
			epsilon->setLexeme(new TerminalSentence(-1));

			registerToDelete(epsilon);
		}

		return epsilon;
	}

	TokenSentence* Grammar::getEndmarker() {
		if(!endmarker) {
			endmarker = new TokenSentence("endmarker");
			endmarker->setLexeme(new TerminalSentence(-2));

			registerToDelete(endmarker);
		}

		return endmarker;
	}

	void Grammar::augment() {
		Rule rule = createRule("AugmentedRule");

		rule = RuleInter(firstRule, this);

		augmentedRule = rule.sentence;
	}

	RuleSentence* Grammar::getFirstRule() {
		return firstRule;
	}

	void Grammar::computeFirst() {
		ProductionVisitor productionVisitor(productions);
		doOnRules(&productionVisitor);

		for(size_t i = 0; i < productions.size(); i++) {
			std::set<Sentence*> f = computeFirst(productions[i]->sentences);
			first[productions[i]->rule].insert(f.begin(), f.end());
		}
	}

	std::set<Sentence*> Grammar::computeFirst(const std::vector<Sentence*> sentences) {
		FirstVisitor visitor(getEpsilon());

		for(size_t j = 0; j < sentences.size(); j++) {
			sentences[j]->accept(&visitor);

			if(visitor.first.count(getEpsilon()) == 0)
				break;
		}

		return visitor.first;
	}

	bool Grammar::isNonTerminal(Sentence* sentence) {
		return allRules.count(sentence) == 1;
	}

	void Grammar::computeFollow() {
		follow[firstRule].insert(getEndmarker());

		for(size_t i = 0; i < productions.size(); i++) {
			for(size_t j = 0; j < productions[i]->sentences.size(); j++) {
				Sentence* B = productions[i]->sentences[j];

				if(!isNonTerminal(B))
					continue;

				if(j + 1 < productions[i]->sentences.size()) {
					Sentence* beta = productions[i]->sentences[j + 1];

					std::set<Sentence*> firstBeta = first[beta];

					firstBeta.erase(getEpsilon());
					follow[B].insert(firstBeta.begin(), firstBeta.end());
				}
			}
		}

		for(size_t i = 0; i < productions.size(); i++) {
			Sentence* A = productions[i]->rule;

			for(size_t j = 0; j < productions[i]->sentences.size(); j++) {
				Sentence* B = productions[i]->sentences[j];

				if(!isNonTerminal(B))
					continue;

				if(j + 1 < productions[i]->sentences.size()) {
					Sentence* beta = productions[i]->sentences[j + 1];

					const std::set<Sentence*>& firstBeta = first[beta];

					if(firstBeta.count(getEpsilon()) != 0) {
						follow[B].insert(follow[A].begin(), follow[A].end());
					}
				} else {
					follow[B].insert(follow[A].begin(), follow[A].end());
				}
			}
		}
	}

	std::set<Item> Grammar::closure(const std::set<Item>& I) {
		std::vector<Item> items(I.begin(), I.end());
		std::set<Item> result(I.begin(), I.end());

		for(size_t i = 0; i < items.size(); i++) {
			Item item = items[i];

			Production* A = item.production;

			Sentence* B = A->sentences[item.dot];

			if(!isNonTerminal(B))
				continue;

			std::vector<Production*> BtoGamma = findProduction(B);

			std::vector<Sentence*> beta;

			if(item.dot + 1 < A->sentences.size())
				beta.push_back(A->sentences[item.dot + 1]);

			beta.push_back(item.terminal);

			std::set<Sentence*> firstBeta = computeFirst(beta);

			for(size_t k = 0; k < BtoGamma.size(); k++) {
				for(std::set<Sentence*>::iterator b = firstBeta.begin(); b != firstBeta.end(); b++) {
					TokenSentence* terminal = (TokenSentence*) *b;
					Item newItem = { BtoGamma[k], terminal, 0 };

					if(result.count(newItem) == 0) {
						result.insert(newItem);
						items.push_back(newItem);
					}
				}
			}
		}

		return result;

//	std::set<Item> result2(result.begin(), result.end());
//	return result2;
	}

	std::vector<Production*> Grammar::findProduction(Sentence* sentence) {
		std::vector<Production*> result;

		for(size_t i = 0; i < productions.size(); i++) {
			if(productions[i]->rule == sentence)
				result.push_back(productions[i]);
		}

		return result;
	}

	std::set<Item> Grammar::_goto(const std::set<Item>& I, Sentence* X) {
		std::set<Item> J;

		for(std::set<Item>::const_iterator ite = I.begin(); ite != I.end(); ite++) {
			Item item = *ite;

			if(item.dot < item.production->sentences.size() && item.production->sentences[item.dot] == X) {
				Item newItem = { item.production, item.terminal, item.dot + 1 };
				J.insert(newItem);
			}
		}

		return closure(J);
	}

	void Grammar::computeItems() {
		augment();
		computeFirst();
		computeFollow();

		std::set<std::set<Item> > C;

		Production* prod = findProduction(augmentedRule)[0];
		TokenSentence* endmark = getEndmarker();
		int dot = 0;

		Item item = { prod, endmark, dot };
		std::set<Item> temp;
		temp.insert(item);

		temp = closure(temp);

		C.insert(temp);
		items.push_back(temp);

		for(size_t i = 0; i < items.size(); i++) {
			for(size_t j = 0; j < allGrammarSymbols.size(); j++) {
				std::set<Item> go = _goto(items[i], allGrammarSymbols[j]);

				if(!go.empty() && C.count(go) == 0) {
					C.insert(go);
					items.push_back(go);
				}
			}
		}
	}

	void Grammar::computeParsingTable() {
		actionsTable.resize(items.size());
		gotoTable.resize(items.size());

		for(size_t i = 0; i < items.size(); i++) {
			for(std::set<Item>::iterator j = items[i].begin(); j != items[i].end(); j++) {
				Item item = *j;

				if(item.dot < item.production->sentences.size()) {
					Sentence* a = item.production->sentences[item.dot];

					if(!isNonTerminal(a)) {
						std::set<Item> go = _goto(items[i], a);

						for(size_t k = 0; k < items.size(); k++) {
							if(go == items[k]) {
								//actions[i, a] = shift k
								actionsTable[i][a].action = SHIFT;
								actionsTable[i][a].state = k;
								break;
							}
						}
					}
				}

				if(item.dot >= item.production->sentences.size() && item.production->rule != augmentedRule) {
					//actions[i, a] = reduce A -> alpha
					int prodIndex = 0;

					for(size_t k = 0; productions.size(); k++) {
						if(productions[k] == item.production) {
							prodIndex = k;
							break;
						}
					}

					Sentence* a = item.terminal;
					actionsTable[i][a].action = REDUCE;
					actionsTable[i][a].state = prodIndex;
				}

				if(item.production->rule == augmentedRule && item.production->sentences[0] == firstRule && item.dot == 1
						&& item.terminal == getEndmarker()) {
					//actions[i, $] = accept
					actionsTable[i][item.terminal].action = ACCEPT;
					acceptState = i;
				}
			}
		}

		for(size_t i = 0; i < items.size(); i++) {
			for(std::set<Sentence*>::iterator A = allRules.begin(); A != allRules.end(); A++) {
				std::set<Item> go = _goto(items[i], *A);

				for(size_t j = 0; j < items.size(); j++) {
					if(go == items[j]) {
						//goto[i, A] = j
						gotoTable[i][*A] = j;
					}
				}
			}
		}

		for(size_t i = 0; i < items.size(); i++) {
			for(std::set<Item>::iterator j = items[i].begin(); j != items[i].end(); j++) {
				Item item = *j;

				if(item.production->rule == augmentedRule && item.production->sentences[0] == firstRule
						&& item.terminal == getEndmarker() && item.dot == 0) {
					startState = i;
				}
			}
		}
	}

	/**
	 * while(true) {
	 * 	let s be the state on top of the stack;
	 *
	 * 	if ( ACTION[s, a] == shift t ) {
	 * 		push t onto the stack;
	 * 		let a be the next input symbol;
	 * 	} else if ( ACTION[s, a] == reduce A -> beta ) {
	 * 		pop |beta| symbols off the stack;
	 * 		let state t now be on top of the stack;
	 * 		push GOTO[t, A] onto the stack;
	 * 		output the production A -> beta;
	 * 	} else if ( ACTION[S, a] == accept ) break; //parsing is done
	 * 	else call error-recovery routine;
	 * }
	 */
	void Grammar::parse(const std::string& input) {
		Tokenizer tokenizer = createTokenizer();

		ParseInput parseInput(input);
		std::vector<Tokenizer::Token> tokens = tokenizer.tokenize(parseInput);

		Tokenizer::Token endmark;
		endmark.token = getEndmarker();

		tokens.push_back(endmark);

		std::stack<int> states;

		states.push(startState);

		size_t i = 0;
		while(i < tokens.size()) {
			int state = states.top();

			State action = actionsTable[state][tokens[i].token];
			if(action.action == SHIFT) {
				tokens[i].token->getLexeme()->action()(this, GrammarEvent());

				states.push(action.state);
				i++;
			} else if(action.action == REDUCE) {
				for(size_t x = 0; x < productions[action.state]->sentences.size(); x++)
					states.pop();

				Sentence* A = productions[action.state]->rule;

				productions[action.state]->callback(this, GrammarEvent());

				state = states.top();

				states.push(gotoTable[state][A]);
			} else if(action.action == ACCEPT) {
				break;
			} else {
				std::cout << "error" << std::endl;
			}
		}
	}

	Tokenizer Grammar::createTokenizer() {
		TokenCompiler compiler;

		doOnTokens(&compiler);

		return compiler.createTokenizer();
	}

	void Grammar::doOnTokens(SentenceVisitor* visitor) {
		std::map<std::string, TokenSentence*>::iterator ite;

		for(ite = tokensByName.begin(); ite != tokensByName.end(); ite++) {
			ite->second->accept(visitor);
		}
	}

	void Grammar::doOnRules(SentenceVisitor* visitor) {
		std::map<std::string, RuleSentence*>::iterator ite;

		for(ite = rulesByName.begin(); ite != rulesByName.end(); ite++) {
			ite->second->accept(visitor);
		}
	}

	RuleInter operator&(const Token& token, const Rule& rule) {
		return RuleInter(new AndSentence(token.token, rule.sentence), rule.grammar);
	}

	RuleInter operator|(const Token& token, const Rule& rule) {
		return RuleInter(new OrSentence(token.token, rule.sentence), rule.grammar);
	}

	RuleInter operator&(const Rule& rule, const Token& token) {
		return RuleInter(new AndSentence(rule.sentence, token.token), rule.grammar);
	}

	RuleInter operator|(const Rule& rule, const Token& token) {
		return RuleInter(new OrSentence(rule.sentence, token.token), rule.grammar);
	}

	RuleInter operator&(const Token& token, const RuleInter& rule) {
		return RuleInter(new AndSentence(token.token, rule.sentence), rule.grammar);
	}

	RuleInter operator|(const Token& token, const RuleInter& rule) {
		return RuleInter(new OrSentence(token.token, rule.sentence), rule.grammar);
	}

	RuleInter operator&(const RuleInter& rule, const Token& token) {
		return RuleInter(new AndSentence(rule.sentence, token.token), rule.grammar);
	}

	RuleInter operator|(const RuleInter& rule, const Token& token) {
		return RuleInter(new OrSentence(rule.sentence, token.token), rule.grammar);
	}

	RuleInter operator&(const TokenFragment& token, const Rule& rule) {
		return RuleInter(new AndSentence(token.sentence, rule.sentence), rule.grammar);
	}

	RuleInter operator|(const TokenFragment& token, const Rule& rule) {
		return RuleInter(new OrSentence(token.sentence, rule.sentence), rule.grammar);
	}

	RuleInter operator&(const Rule& rule, const TokenFragment& token) {
		return RuleInter(new AndSentence(rule.sentence, token.sentence), rule.grammar);
	}

	RuleInter operator|(const Rule& rule, const TokenFragment& token) {
		return RuleInter(new OrSentence(rule.sentence, token.sentence), rule.grammar);
	}

//////////////////////

	RuleInter operator&(const Rule& rule0, const Rule& rule1) {
		return RuleInter(new AndSentence(rule0.sentence, rule1.sentence), rule0.grammar);
	}

	RuleInter operator|(const Rule& rule0, const Rule& rule1) {
		return RuleInter(new OrSentence(rule0.sentence, rule1.sentence), rule0.grammar);
	}

	RuleInter operator&(const RuleInter& rule0, const RuleInter& rule1) {
		return RuleInter(new AndSentence(rule0.sentence, rule1.sentence), rule0.grammar);
	}

	RuleInter operator|(const RuleInter& rule0, const RuleInter& rule1) {
		return RuleInter(new OrSentence(rule0.sentence, rule1.sentence), rule0.grammar);
	}

	RuleInter operator&(const RuleInter& rule0, const Rule& rule1) {
		return RuleInter(new AndSentence(rule0.sentence, rule1.sentence), rule0.grammar);
	}

	RuleInter operator|(const RuleInter& rule0, const Rule& rule1) {
		return RuleInter(new OrSentence(rule0.sentence, rule1.sentence), rule0.grammar);
	}

	RuleInter operator|(const Rule& rule0, const RuleInter& rule1) {
		return RuleInter(new OrSentence(rule0.sentence, rule1.sentence), rule0.grammar);
	}

	RuleInter operator&(const Rule& rule0, const RuleInter& rule1) {
		return RuleInter(new AndSentence(rule0.sentence, rule1.sentence), rule0.grammar);
	}

	RuleInter operator|(const Rule& rule, int) {
		return RuleInter(new OrSentence(rule.sentence, rule.grammar->getEpsilon()), rule.grammar);
	}

	RuleInter operator|(const RuleInter& rule, int) {
		return RuleInter(new OrSentence(rule.sentence, rule.grammar->getEpsilon()), rule.grammar);
	}

	RuleInter operator&(char ch, const Rule& rule) {
		std::string name("anonymous_");
		name.push_back(ch);
		Token token = rule.grammar->createToken(name);
		token = Terminal(ch);
		return token & rule;
	}

	RuleInter operator|(char ch, const Rule& rule) {
		std::string name("anonymous_");
		name.push_back(ch);
		Token token = rule.grammar->createToken(name);
		token = Terminal(ch);
		return token | rule;
	}

	RuleInter operator&(const Rule& rule, char ch) {
		std::string name("anonymous_");
		name.push_back(ch);
		Token token = rule.grammar->createToken(name);
		token = Terminal(ch);
		return rule & token;
	}

	RuleInter operator|(const Rule& rule, char ch) {
		std::string name("anonymous_");
		name.push_back(ch);
		Token token = rule.grammar->createToken(name);
		token = Terminal(ch);
		return rule | token;
	}

	RuleInter operator&(char ch, const RuleInter& rule) {
		std::string name("anonymous_");
		name.push_back(ch);
		Token token = rule.grammar->createToken(name);
		token = Terminal(ch);
		return token & rule;
	}

	RuleInter operator|(char ch, const RuleInter& rule) {
		std::string name("anonymous_");
		name.push_back(ch);
		Token token = rule.grammar->createToken(name);
		token = Terminal(ch);
		return token | rule;
	}

	RuleInter operator&(const RuleInter& rule, char ch) {
		std::string name("anonymous_");
		name.push_back(ch);
		Token token = rule.grammar->createToken(name);
		token = Terminal(ch);
		return rule & token;
	}

	RuleInter operator|(const RuleInter& rule, char ch) {
		std::string name("anonymous_");
		name.push_back(ch);
		Token token = rule.grammar->createToken(name);
		token = Terminal(ch);
		return rule | token;
	}

///////////////////////////

	TokenFragment operator|(const Terminal& terminal0, const Terminal& terminal1) {
		return TokenFragment(new OrSentence(terminal0.sentence, terminal1.sentence));
	}

	TokenFragment operator&(const Terminal& terminal0, const Terminal& terminal1) {
		return TokenFragment(new AndSentence(terminal0.sentence, terminal1.sentence));
	}

	TokenFragment operator|(const TokenFragment& token, const Terminal& terminal) {
		return TokenFragment(new OrSentence(token.sentence, terminal.sentence));
	}

	TokenFragment operator|(const Terminal& terminal, const TokenFragment& token) {
		return TokenFragment(new OrSentence(terminal.sentence, token.sentence));
	}

	TokenFragment operator&(const TokenFragment& token, const Terminal& terminal) {
		return TokenFragment(new AndSentence(token.sentence, terminal.sentence));
	}

	TokenFragment operator|(const TokenFragment& fragment, const Token& token) {
		return TokenFragment(new OrSentence(fragment.sentence, token.token));
	}

	TokenFragment operator&(const TokenFragment& fragment, const Token& token) {
		return TokenFragment(new AndSentence(fragment.sentence, token.token));
	}

	TokenFragment operator&(const Terminal& terminal, const TokenFragment& token) {
		return TokenFragment(new AndSentence(terminal.sentence, token.sentence));
	}

	TokenFragment operator&(const TokenFragment& token0, const TokenFragment& token1) {
		return TokenFragment(new AndSentence(token0.sentence, token1.sentence));
	}

	TokenFragment operator|(const TokenFragment& token0, const TokenFragment& token1) {
		return TokenFragment(new OrSentence(token0.sentence, token1.sentence));
	}

///////////////////////////

	TokenFragment operator&(char ch, const Token& token) {
		std::string name("anonymous_");
		name.push_back(ch);
		Token newToken = token.grammar->createToken(name);
		newToken = Terminal(ch);
		return TokenFragment(new AndSentence(newToken.token, token.token));
	}

	TokenFragment operator&(const Token& token, char ch) {
		std::string name("anonymous_");
		name.push_back(ch);
		Token newToken = token.grammar->createToken(name);
		newToken = Terminal(ch);
		return TokenFragment(new AndSentence(token.token, newToken.token));
	}

	TokenFragment operator|(char ch, const Token& token) {
		std::string name("anonymous_");
		name.push_back(ch);
		Token newToken = token.grammar->createToken(name);
		newToken = Terminal(ch);
		return TokenFragment(new OrSentence(newToken.token, token.token));
	}

	TokenFragment operator|(const Token& token, char ch) {
		std::string name("anonymous_");
		name.push_back(ch);
		Token newToken = token.grammar->createToken(name);
		newToken = Terminal(ch);
		return TokenFragment(new OrSentence(token.token, newToken.token));
	}

	TokenFragment set(int start, int end, std::initializer_list<int> excludes) {
		TokenFragment token(Terminal(start++));

		std::set<int> ex(excludes.begin(), excludes.end());

		while(start <= end) {
			if(ex.count(start) == 0)
				token = token | Terminal(start);
			start++;
		}

		return token;
	}

	TokenFragment sequence(const char* seq) {
		const char* end = seq + strlen(seq);

		TokenFragment token(Terminal(*seq++));

		while(seq < end) {
			token = token & Terminal(*seq++);
		}

		return token;
	}

///////////////////////////
}// namespace engine
