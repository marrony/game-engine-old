/*
 * Grammar.h
 *
 *  Created on: 09/10/2011
 *      Author: marrony
 */

#ifndef GRAMMAR_H_
#define GRAMMAR_H_

#include "Sentence.h"
#include "Tokenizer.h"

namespace engine {

	class Grammar;

	struct Terminal {
		Terminal(char ch) :
				sentence(new TerminalSentence(ch)) {
		}

		Terminal& action(Callback callback) {
			sentence->action(callback);
			return *this;
		}

		Sentence* sentence;
	};

	struct TokenFragment {
		TokenFragment(Sentence* sentence) :
				sentence(sentence) {
		}

		TokenFragment(const Terminal& terminal) :
				sentence(terminal.sentence) {
		}

		TokenFragment operator*() {
			return TokenFragment(new ClosureSentence(sentence));
		}

		TokenFragment& action(Callback callback) {
			sentence->action(callback);
			return *this;
		}

		Sentence* sentence;
	};

	struct RuleInter {
		RuleInter(Sentence* sentence, Grammar* grammar) :
				sentence(sentence), grammar(grammar) {
		}

		RuleInter operator*() {
			return RuleInter(new ClosureSentence(sentence), grammar);
		}

		RuleInter& action(Callback callback) {
			sentence->action(callback);
			return *this;
		}

		Sentence* sentence;
		Grammar* grammar;
	};

	struct Token {
		Token(TokenSentence* sentence, Grammar* grammar) :
				token(sentence), grammar(grammar) {
		}

		void operator=(const TokenFragment& tokenInter);
		void operator=(const Terminal& terminal);

		Token& ignoreToken() {
			token->ignoreToken();
			return *this;
		}

		RuleInter action(Callback callback) {
			RuleInter rule(token, grammar);

			rule.action(callback);

			return rule;
		}

		TokenSentence* token;
		Grammar* grammar;
	};

	struct Rule {
		Rule(RuleSentence* sentence, Grammar* grammar) :
				sentence(sentence), grammar(grammar) {
		}

		void operator=(const TokenFragment& token);
		void operator=(const RuleInter& rule);

		RuleSentence* sentence;
		Grammar* grammar;
	};

	struct Production {
		Production(RuleSentence* rule, Callback callback) :
				rule(rule), sentences(), callback(callback) {
		}

		Production(RuleSentence* rule, const std::vector<Sentence*>& sentences, Callback callback) :
				rule(rule), sentences(sentences), callback(callback) {
		}

		RuleSentence* rule;
		std::vector<Sentence*> sentences;
		Callback callback;
	};

	struct Item {
		Production* production;
		TokenSentence* terminal;
		int dot;

		bool operator==(const Item& other) const {
			return production == other.production && terminal == other.terminal && dot == other.dot;
		}

		bool operator<(const Item& other) const {
			if(production < other.production)
				return true;
			else if(production == other.production) {
				if(terminal < other.terminal)
					return true;
				else if(terminal == other.terminal)
					return dot < other.dot;
			}

			return false;
		}
	};

	class Grammar {
	public:
		Grammar();
		~Grammar();

		Rule createRule(const char* ruleName);
		Token createToken(const std::string& tokenName);

		void registerToDelete(Sentence* sentence);

		RuleSentence* findRule(const std::string& ruleName);
		TokenSentence* findToken(const std::string& tokenName);

		TokenSentence* getEpsilon();
		TokenSentence* getEndmarker();

		RuleSentence* getFirstRule();

		void computeItems();
		void computeParsingTable();

		void parse(const std::string& input);

		Tokenizer createTokenizer();
	private:
		bool isNonTerminal(Sentence* sentence);

		void augment();
		void computeFirst();
		std::set<Sentence*> computeFirst(const std::vector<Sentence*> sentences);

		void computeFollow();

		std::set<Item> closure(const std::set<Item>& I);
		std::set<Item> _goto(const std::set<Item>& I, Sentence* X);

		std::vector<Production*> findProduction(Sentence* sentence);

		void doOnTokens(SentenceVisitor* visitor);
		void doOnRules(SentenceVisitor* visitor);

		std::vector<Sentence*> allGrammarSymbols;
		std::set<Sentence*> allRules;
		std::map<std::string, RuleSentence*> rulesByName;
		std::map<std::string, TokenSentence*> tokensByName;
		RuleSentence* firstRule;
		RuleSentence* augmentedRule;

		std::set<Sentence*> sentencesToDelete;

		TokenSentence* epsilon;
		TokenSentence* endmarker;

		std::vector<Production*> productions;
		std::map<Sentence*, std::set<Sentence*> > first;
		std::map<Sentence*, std::set<Sentence*> > follow;
		std::vector<std::set<Item> > items;

		enum Action {
			_ERROR, SHIFT, REDUCE, ACCEPT
		};

		struct State {
			Action action;
			int state;
		};

		std::vector<std::map<Sentence*, State> > actionsTable;
		std::vector<std::map<Sentence*, int> > gotoTable;
		int acceptState;
		int startState;
	};

//////////////////////////////////////////////////////////

	RuleInter operator&(const Rule& rule0, const Rule& rule1);
	RuleInter operator|(const Rule& rule0, const Rule& rule1);

	RuleInter operator&(const Rule& rule0, const RuleInter& rule1);
	RuleInter operator|(const Rule& rule0, const RuleInter& rule1);

	RuleInter operator&(const Rule& rule, const Token& token);
	RuleInter operator|(const Rule& rule, const Token& token);

	RuleInter operator&(const Rule& rule, const TokenFragment& token);
	RuleInter operator|(const Rule& rule, const TokenFragment& token);

	RuleInter operator&(const Rule& rule, char ch);
	RuleInter operator|(const Rule& rule, char ch);

	RuleInter operator|(const Rule& rule, int);

//////////////////////////////////////////////////////////

	RuleInter operator&(const Token& token, const Rule& rule);
	RuleInter operator|(const Token& token, const Rule& rule);

	RuleInter operator&(const Token& token, const RuleInter& rule);
	RuleInter operator|(const Token& token, const RuleInter& rule);

	TokenFragment operator&(const Token& token, char ch);
	TokenFragment operator|(const Token& token, char ch);

//////////////////////////////////////////////////////////

	RuleInter operator&(const RuleInter& rule0, const RuleInter& rule1);
	RuleInter operator|(const RuleInter& rule0, const RuleInter& rule1);

	RuleInter operator&(const RuleInter& rule, const Token& token);
	RuleInter operator|(const RuleInter& rule, const Token& token);

	RuleInter operator&(const RuleInter& rule0, const Rule& rule1);
	RuleInter operator|(const RuleInter& rule0, const Rule& rule1);

	RuleInter operator&(const RuleInter& rule, char ch);
	RuleInter operator|(const RuleInter& rule, char ch);

	RuleInter operator|(const RuleInter& rule, int);

//////////////////////////////////////////////////////////

	RuleInter operator&(const TokenFragment& token, const Rule& rule);
	RuleInter operator|(const TokenFragment& token, const Rule& rule);

	TokenFragment operator|(const TokenFragment& token, const Terminal& terminal);
	TokenFragment operator&(const TokenFragment& token, const Terminal& terminal);

	TokenFragment operator|(const TokenFragment& fragment, const Token& token);
	TokenFragment operator&(const TokenFragment& fragment, const Token& token);

	TokenFragment operator&(const TokenFragment& token0, const TokenFragment& token1);
	TokenFragment operator|(const TokenFragment& token0, const TokenFragment& token1);

//////////////////////////////////////////////////////////

	TokenFragment operator|(const Terminal& terminal0, const Terminal& terminal1);
	TokenFragment operator&(const Terminal& terminal0, const Terminal& terminal1);

	TokenFragment operator|(const Terminal& terminal, const TokenFragment& token);
	TokenFragment operator&(const Terminal& terminal, const TokenFragment& token);

//////////////////////////////////////////////////////////

	TokenFragment operator&(char ch, const Token& token);
	TokenFragment operator|(char ch, const Token& token);

	RuleInter operator&(char ch, const Rule& rule);
	RuleInter operator|(char ch, const Rule& rule);

	RuleInter operator&(char ch, const RuleInter& rule);
	RuleInter operator|(char ch, const RuleInter& rule);

	TokenFragment set(int start, int end, std::initializer_list<int> excludes);
	TokenFragment sequence(const char* seq);

} // namespace engine

#endif /* GRAMMAR_H_ */
