/*
 * Sentence.h
 *
 *  Created on: 09/10/2011
 *      Author: marrony
 */

#ifndef SENTENCE_H_
#define SENTENCE_H_

#include <string>
#include <vector>
#include <set>
#include <map>

#include "TypedEvent.h"

namespace engine {

	class Sentence;

	class SentenceVisitor {
	public:
		virtual ~SentenceVisitor() {
		}
	};

	struct GrammarEvent {
	};

	typedef std::function<void(void*, const GrammarEvent&)> Callback;

	class Sentence {
	public:
		Sentence(const std::string& sentenceName) :
				sentenceName(sentenceName), callback(0) {
		}
		virtual ~Sentence() {
		}

		virtual void accept(SentenceVisitor* visitor) = 0;

		void action(Callback cb) {
			callback = cb;
		}

		Callback action() {
			return callback;
		}

	private:
		std::string sentenceName;
		Callback callback;
	};

	class RuleSentence;
	class RuleSentenceVisitor {
	public:
		virtual ~RuleSentenceVisitor() { }
		virtual void visit(RuleSentence* sentence) = 0;
	};

	class RuleSentence : public Sentence {
	public:
		RuleSentence(const std::string& ruleName) :
				Sentence(ruleName), sentenceName(ruleName), production(), recursionCount(0) {
		}

		virtual void accept(SentenceVisitor* visitor) {
			RuleSentenceVisitor* ruleVisitor = dynamic_cast<RuleSentenceVisitor*>(visitor);

			if(ruleVisitor)
				ruleVisitor->visit(this);
		}

		void setProduction(Sentence* sentence) {
			production = sentence;
		}

		Sentence* getProduction() {
			return production;
		}

		std::string getName() const {
			return sentenceName;
		}
	private:
		std::string sentenceName;
		Sentence* production;
		int recursionCount;
	};

	class TokenSentence;
	class TokenSentenceVisitor {
	public:
		virtual ~TokenSentenceVisitor() { }
		virtual void visit(TokenSentence* sentence) = 0;
	};

	class TokenSentence : public Sentence {
	public:
		TokenSentence(const std::string& tokenName) :
				Sentence(tokenName), tokenName(tokenName), lexeme(), ignore(false) {
		}

		virtual void accept(SentenceVisitor* visitor) {
			TokenSentenceVisitor* tokenVisitor = dynamic_cast<TokenSentenceVisitor*>(visitor);

			if(tokenVisitor)
				tokenVisitor->visit(this);
		}

		void setLexeme(Sentence* lex) {
			lexeme = lex;
		}

		Sentence* getLexeme() {
			return lexeme;
		}

		std::string getTokenName() const {
			return tokenName;
		}

		void ignoreToken() {
			ignore = true;
		}

		bool shouldIgnore() const {
			return ignore;
		}
	private:
		std::string tokenName;
		Sentence* lexeme;
		bool ignore;
	};

	class OrSentence;
	class OrSentenceVisitor {
	public:
		virtual ~OrSentenceVisitor() { }
		virtual void visit(OrSentence* sentence) = 0;
	};

	class OrSentence : public Sentence {
	public:
		OrSentence(Sentence* first, Sentence* second) :
				Sentence("or"), first(first), second(second) {
		}

		virtual void accept(SentenceVisitor* visitor) {
			OrSentenceVisitor* orVisitor = dynamic_cast<OrSentenceVisitor*>(visitor);

			if(orVisitor)
				orVisitor->visit(this);
		}

		Sentence* getFirst() {
			return first;
		}

		Sentence* getSecond() {
			return second;
		}
	private:
		Sentence* first;
		Sentence* second;
	};

	class AndSentence;
	class AndSentenceVisitor {
	public:
		virtual ~AndSentenceVisitor() { }
		virtual void visit(AndSentence* sentence) = 0;
	};

	class AndSentence : public Sentence {
	public:
		AndSentence(Sentence* first, Sentence* second) :
				Sentence("and"), first(first), second(second) {
		}

		virtual void accept(SentenceVisitor* visitor) {
			AndSentenceVisitor* andVisitor = dynamic_cast<AndSentenceVisitor*>(visitor);

			if(andVisitor)
				andVisitor->visit(this);
		}

		Sentence* getFirst() {
			return first;
		}

		Sentence* getSecond() {
			return second;
		}
	private:
		Sentence* first;
		Sentence* second;
	};

	class NotSentence;
	class NotSentenceVisitor {
	public:
		virtual ~NotSentenceVisitor() { }
		virtual void visit(NotSentence* sentence) = 0;
	};

	class NotSentence : public Sentence {
	public:
		NotSentence(Sentence* sentence) :
			Sentence("not"), sentence(sentence) {
		}

		virtual void accept(SentenceVisitor* visitor) {
			NotSentenceVisitor* notVisitor = dynamic_cast<NotSentenceVisitor*>(visitor);

			if(notVisitor)
				notVisitor->visit(this);
		}

		Sentence* getSentence() {
			return sentence;
		}
	private:
		Sentence* sentence;
	};

	class ClosureSentence;
	class ClosureSentenceVisitor {
	public:
		virtual ~ClosureSentenceVisitor() { }
		virtual void visit(ClosureSentence* sentence) = 0;
	};

	class ClosureSentence : public Sentence {
	public:
		ClosureSentence(Sentence* sentence) :
				Sentence("closure"), sentence(sentence) {
		}

		virtual void accept(SentenceVisitor* visitor) {
			ClosureSentenceVisitor* closureVisitor = dynamic_cast<ClosureSentenceVisitor*>(visitor);

			if(closureVisitor)
				closureVisitor->visit(this);
		}

		Sentence* getSentence() {
			return sentence;
		}
	private:
		Sentence* sentence;
	};

	class TerminalSentence;
	class TerminalVisitor {
	public:
		virtual ~TerminalVisitor() { }
		virtual void visit(TerminalSentence* sentence) = 0;
	};

	class TerminalSentence : public Sentence {
		static std::string name(int terminal) {
			if(terminal == -1)
				return "epsilon";

			if(terminal == -2)
				return "$";

			std::string n;

			n.push_back((char) terminal);

			return n;
		}
	public:
		TerminalSentence(int terminal) :
				Sentence(name(terminal)), terminal(terminal) {
		}

		virtual void accept(SentenceVisitor* visitor) {
			TerminalVisitor* terminalVisitor = dynamic_cast<TerminalVisitor*>(visitor);

			if(terminalVisitor)
				terminalVisitor->visit(this);
		}

		int getTerminal() {
			return terminal;
		}
	private:
		int terminal;
	};

	class AllSentencesVisitor : public SentenceVisitor,
			public TokenSentenceVisitor,
			public RuleSentenceVisitor,
			public OrSentenceVisitor,
			public AndSentenceVisitor,
			public NotSentenceVisitor,
			public ClosureSentenceVisitor,
			public TerminalVisitor {
	};

} // namespace engine

#endif /* SENTENCE_H_ */
