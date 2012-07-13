/*
 * Tokenizer.h
 *
 *  Created on: 09/10/2011
 *      Author: marrony
 */

#ifndef TOKENIZER_H_
#define TOKENIZER_H_

#include "Sentence.h"

namespace engine {

	class ParseInput {
	public:
		ParseInput(const std::string& input) :
				input(input), position(0), marked(0) {
		}

		void mark() {
			marked = position;
		}

		void unmark() {
			position = marked;
		}

		int peek() {
			if(position >= input.size())
				return -1;
			return input[position];
		}

		int get() {
			if(position >= input.size())
				return -1;
			return input[position++];
		}

		void unget() {
			if(position > 0)
				position--;
		}

		int getMarked() const {
			return marked;
		}

		int getPosition() const {
			return position;
		}

		std::string getString(int start, int end) const {
			return std::string(input.begin() + start, input.begin() + end);
		}
	private:
		std::string input;
		int position;
		int marked;
	};

	struct ParseOutput {
		std::vector<std::string> rules;
	};

	class DFA {
	public:
		DFA(const std::set<int>& alphabet) : alphabet(alphabet) {
			minElement = *std::min_element(alphabet.begin(), alphabet.end());
			maxElement = *std::max_element(alphabet.begin(), alphabet.end());
		}

		void createTransition(int from, int to, int symbol) {
			int maxState = std::max(from, to);

			for(int i = states.size(); i <= maxState; i++) {
				states.push_back(std::vector<int>(maxElement - minElement));
			}

			states[from][symbol - minElement] = to;
		}

		bool simulate(ParseInput& input, std::pair<int, int>& lexeme) {
			int state = start;

			while(input.peek() != -1) {
				int symbol = input.get();

				if(symbol < minElement || symbol > maxElement) {
					return false;
				}

				state = states[state][symbol - minElement];
			}

			return false;
		}
	private:
		std::vector<std::vector<int> > states;
		std::set<int> alphabet;
		int minElement;
		int maxElement;
		int start;
		std::set<int> final;
	};

	class NFA {
	public:
		DFA convertToDfa() {

		}

		bool simulate(ParseInput& input, std::vector<bool>& enabledStates, std::pair<int, int>& lexeme) {
			std::vector<bool> alreadyOn(nodeCount, false);

			std::vector<int> oldStates;
			std::vector<int> newStates;

			eClosure(alreadyOn, oldStates, start);

			std::fill(alreadyOn.begin(), alreadyOn.end(), false);

			lexeme.first = input.getPosition();
			input.mark();

			bool recognized = false;

			while(input.peek() != -1 && !oldStates.empty()) {
				int symbol = input.get();

				std::vector<int>::iterator stateS;

				while(!oldStates.empty()) {
					int state = oldStates.back();

					Transition transition(state, symbol);

					Range range = transitions.equal_range(transition);

					for(std::multimap<Transition, int>::const_iterator ite = range.first; ite != range.second; ite++) {
						if(!alreadyOn[ite->second])
							eClosure(alreadyOn, newStates, ite->second);
					}

					oldStates.pop_back();
				}

				if(recognized && !alreadyOn[final]) {
					input.unmark();
					break;
				}

				if(alreadyOn[final]) {
					recognized = true;
					enabledStates = alreadyOn;
					lexeme.second = input.getPosition();
					input.mark();
				}

				oldStates = newStates;
				newStates.clear();
				std::fill(alreadyOn.begin(), alreadyOn.end(), false);
			}

			return recognized;
		}

		void createUnion(const NFA& other, int& mergedStart, int& mergedFinal) {
			merge(other, mergedStart, mergedFinal);

			int oldStart = start;
			int oldFinal = final;
			int newStart = createStartNode();
			int newFinal = createFinalNode();

			createTransition(newStart, oldStart, -1);
			createTransition(newStart, mergedStart, -1);

			createTransition(oldFinal, newFinal, -1);
			createTransition(mergedFinal, newFinal, -1);
		}

		void createConcatenation(const NFA& other, int& mergedStart, int& mergedFinal) {
			merge(other, mergedStart, mergedFinal);

			int oldFinal = final;

			createTransition(oldFinal, mergedStart, -1);

			final = mergedFinal;
		}

		void createClosure() {
			int oldStart = start;
			int oldFinal = final;
			int newStart = createStartNode();
			int newFinal = createFinalNode();

			createTransition(newStart, oldStart, -1);
			createTransition(newStart, newFinal, -1);

			createTransition(oldFinal, newFinal, -1);
			createTransition(oldFinal, oldStart, -1);
		}

		void createNegation() {
			int oldFinal = final;
			int newFinal = createFinalNode();

			std::set<int> alphabet;

			for(int i = 0; i < 255; i++) {
				if(std::isprint(i))
					alphabet.insert(i);
			}

			std::set<Transition> oldTransition;

			for(TransitionMap::iterator ite = transitions.begin(); ite != transitions.end(); ite++)
				oldTransition.insert(ite->first);

			for(int state = start; state <= final; state++) {
				if(state == oldFinal)
					continue;

				for(std::set<Transition>::iterator ite = oldTransition.begin(); ite != oldTransition.end(); ite++) {
					if(ite->state != state)
						continue;

					std::set<int> alphabet2 = alphabet;

					alphabet2.erase(ite->symbol);

					for(std::set<int>::iterator symbol = alphabet2.begin(); symbol != alphabet2.end(); symbol++)
						createTransition(state, newFinal, *symbol);
				}
			}
		}

		int finalState() const {
			return final;
		}

		int startState() const {
			return start;
		}

		static NFA createEmpty() {
			return NFA();
		}

		static NFA createTransition(int symbol) {
			NFA nfa;

			int start = nfa.createStartNode();
			int final = nfa.createFinalNode();

			nfa.createTransition(start, final, symbol);

			return nfa;
		}

		static NFA createUnion(const NFA& one, const NFA& other) {
			NFA nfa = one;

			int mergedStart, mergedFinal;
			nfa.createUnion(other, mergedStart, mergedFinal);

			return nfa;
		}

		static NFA createConcatenation(const NFA& one, const NFA& other) {
			NFA nfa = one;

			int mergedStart, mergedFinal;
			nfa.createConcatenation(other, mergedStart, mergedFinal);

			return nfa;
		}

		static NFA createClosure(const NFA& one) {
			NFA nfa = one;

			nfa.createClosure();

			return nfa;
		}

		static NFA createNegation(const NFA& one) {
			NFA nfa = one;

			nfa.createNegation();

			return nfa;
		}
	private:
		NFA() :
				nodeCount(0), start(0), final(0), transitions() {
		}

		void eClosure(std::vector<bool>& alreadyOn, std::vector<int>& newStates, int state) {
			newStates.push_back(state);
			alreadyOn[state] = true;

			Transition transition(state, -1);

			Range range = transitions.equal_range(transition);

			for(TransitionMap::const_iterator ite = range.first; ite != range.second; ite++) {
				if(!alreadyOn[ite->second])
					eClosure(alreadyOn, newStates, ite->second);
			}
		}

		int createStartNode() {
			start = nodeCount++;
			return start;
		}

		int createFinalNode() {
			final = nodeCount++;
			return final;
		}

		void createTransition(int from, int to, int symbol) {
			transitions.insert(std::make_pair(Transition(from, symbol), to));
		}

		void merge(const NFA& other, int& mergedStart, int& mergedFinal) {
			TransitionMap::const_iterator ite;

			for(ite = other.transitions.begin(); ite != other.transitions.end(); ite++) {
				int from = ite->first.state + nodeCount;
				int to = ite->second + nodeCount;
				createTransition(from, to, ite->first.symbol);
			}

			mergedStart = other.start + nodeCount;
			mergedFinal = other.final + nodeCount;
			nodeCount += other.nodeCount;
		}

		struct Transition {
			int state;
			int symbol;

			Transition(int state, int symbol) :
					state(state), symbol(symbol) {
			}

			bool operator<(const Transition& other) const {
				if(state < other.state)
					return true;
				else if(state == other.state)
					return symbol < other.symbol;

				return false;
			}
		};

		typedef std::multimap<Transition, int> TransitionMap;
		typedef std::pair<TransitionMap::iterator, TransitionMap::iterator> Range;

		int nodeCount;
		int start;
		int final;
		TransitionMap transitions;
	};

	class Tokenizer {
	public:
		struct Token {
			TokenSentence* token;
			int lexemeStart;
			int lexemeEnd;
		};

		static Tokenizer createFromNFAS(const std::vector<NFA>& nfas, const std::vector<TokenSentence*>& tokens) {
			Tokenizer tokenizer;

			tokenizer.tokens = tokens;
			tokenizer.finals.resize(nfas.size());

			tokenizer.nfa = nfas.front();
			tokenizer.finals[0] = tokenizer.nfa.finalState();

			for(size_t i = 1; i < nfas.size(); i++) {
				int mergedStart, mergedFinal;

				tokenizer.nfa.createUnion(nfas[i], mergedStart, mergedFinal);

				tokenizer.finals[i] = mergedFinal;
			}

			return tokenizer;
		}

		std::vector<Token> tokenize(ParseInput& input) {
			std::vector<Token> tokenizeResult;

			std::vector<bool> enabledStates;
			std::pair<int, int> lexeme;
			Token token;

			while(input.peek() != -1) {
				if(tokenize(input, token))
					tokenizeResult.push_back(token);
			}

			return tokenizeResult;
		}

		bool tokenize(ParseInput& input, Token& token) {
			std::vector<bool> enabledStates;
			std::pair<int, int> lexeme;

			while(true) {
				if(input.peek() == -1 || !nfa.simulate(input, enabledStates, lexeme))
					return false;

				for(size_t i = 0; i < finals.size(); i++) {
					if(enabledStates[finals[i]] && !tokens[i]->shouldIgnore()) {
						token.token = tokens[i];
						token.lexemeStart = lexeme.first;
						token.lexemeEnd = lexeme.second;

						return true;
					}
				}
			}

			return false;
		}
	private:
		Tokenizer() :
				nfa(NFA::createEmpty()) {
		}

		std::vector<TokenSentence*> tokens;
		std::vector<int> finals;
		NFA nfa;
	};

	class TokenCompiler : public SentenceVisitor,
			public TokenSentenceVisitor,
			public OrSentenceVisitor,
			public AndSentenceVisitor,
			public NotSentenceVisitor,
			public ClosureSentenceVisitor,
			public TerminalVisitor {
	public:
		TokenCompiler() :
				nfas(), tokens() {
		}

		virtual void visit(TokenSentence* sentence) {
			sentence->getLexeme()->accept(this);
			tokens.push_back(sentence);
		}

		virtual void visit(OrSentence* sentence) {
			sentence->getFirst()->accept(this);
			sentence->getSecond()->accept(this);

			NFA second = pop();
			NFA first = pop();

			push(NFA::createUnion(first, second));
		}

		virtual void visit(AndSentence* sentence) {
			sentence->getFirst()->accept(this);
			sentence->getSecond()->accept(this);

			NFA second = pop();
			NFA first = pop();

			push(NFA::createConcatenation(first, second));
		}

		virtual void visit(NotSentence* sentence) {
		}

		virtual void visit(ClosureSentence* sentence) {
			sentence->getSentence()->accept(this);

			NFA nfa = pop();

			push(NFA::createClosure(nfa));
		}

		virtual void visit(TerminalSentence* sentence) {
			NFA nfa = NFA::createTransition(sentence->getTerminal());

			push(nfa);
		}

		Tokenizer createTokenizer() {
			return Tokenizer::createFromNFAS(nfas, tokens);
		}
	private:
		void push(const NFA& nfa) {
			nfas.push_back(nfa);
		}

		NFA pop() {
			NFA nfa = nfas.back();
			nfas.pop_back();
			return nfa;
		}

		std::vector<NFA> nfas;
		std::vector<TokenSentence*> tokens;
	};

} // namespace engine

#endif /* TOKENIZER_H_ */
