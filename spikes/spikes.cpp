#include <stdio.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <functional>
#include <string>

#include "definitions.h"
#include "Stream.h"
#include "StringTokenizer.h"
#include "grammar/Grammar.h"
#include "math/Matrix.h"
#include "math/Vector.h"
#include "math/Quaternion.h"
#include "Visitor.h"
#include "Model.h"

#define STRINGFY(x) #x

class A {
public:
	void constF() const {
	}

	void nonConstF() {
	}
};

class B {
public:

	void constF() const {
		a->constF();
		a->nonConstF();

		x.constF();
//		x.nonConstF();
	}

	void nonConstF() {
		a->constF();
		a->nonConstF();

		x.constF();
		x.nonConstF();
	}
private:
	A* a;
	A x;
};

template<typename T>
struct is_pointer {
	static const bool value = false;
};

template<typename T>
struct is_pointer<T*> {
	static const bool value = true;
};

template<typename T>
struct is_const {
	static const bool value = false;
};

template<typename T>
struct is_const<const T> {
	static const bool value = true;
};

template<typename T>
struct is_const<const T&> {
	static const bool value = true;
};

template<typename T>
struct is_const<const T*> {
	static const bool value = true;
};

template<typename T = float>
struct exp2c {
	static T value(int scale) {
		const int mantissa = 23;
		int s = (scale - mantissa + 127) << mantissa;
		return *(float*)&s;
	}
};

template<>
struct exp2c<double> {
	static double value(int scale) {
//		__int64 mantissa = 52;
//		__int64 s = (scale - mantissa + 1023) << mantissa;
//		return *(double*)&s;
		return 0;
	}
};

class Listener {
public:
	virtual ~Listener ( ) { }
};

template<typename Return = void, typename Event = void>
struct Function;

template<typename Return, typename Event>
struct Function<Return(Event)> {

};

template<typename Event>
class Distributor {
public:
	void addListener(Listener* listener, Function<void(Event)> function) {
	}

	void listen(Event e) {
		for(size_t i = 0; i < _listeners.size(); i++) {
			_listeners[i].listen(e);
		}
	}

private:
	struct ListenNode {
		Listener* listener;

		Function<void(Event)> callback;

		void listen(Event e) {
			callback(e);
		}
	};

	std::vector<ListenNode> _listeners;
};

/**
 * pag 244 First(), Follow()
 * pag 266 Closure()
 * pag 259 Goto()
 * pag 284
 */

template<typename T>
class Handle {
public:
	class HandleManager {
	public:
		Handle<T> addObject() {
			int index = -1;

			for(size_t i = 0; i < objectsUsed.size(); i++) {
				if(!objectsUsed[i]) {
					index = i;
					break;
				}
			}

			Handle<T> handle;

			handle.manager = this;

			if(index != -1) {
				handle.index = index;
				objectsUsed[index] = true;
			} else {
				handle.index = objects.size();
				objects.push_back(T());
				objectsUsed.push_back(true);
			}

			return handle;
		}

		void deleteObject(const Handle<T>& handle) {
			objectsUsed[handle.index] = false;
		}

		T* getObject(const Handle<T>& handle) const {
			return &objects[handle.index];
		}
	private:
		std::vector<T> objects;
		std::vector<bool> objectsUsed;
	};

	T* getObject() const {
		if(manager)
			return manager->getObject(*this);
		return 0;
	}

	void deleteObject() {
		if(manager)
			manager->deleteObject(*this);
		manager = 0;
	}
private:
	HandleManager* manager;
	int index;
};

//Header file
class Tokenizer {
public:
	static const std::string DELIMITERS;
	Tokenizer(const std::string& str);
	Tokenizer(const std::string& str, const std::string& delimiters);
	bool NextToken();
	bool NextToken(const std::string& delimiters);
	const std::string GetToken() const;
	void Reset();
protected:
	size_t m_offset;
	const std::string m_string;
	std::string m_token;
	std::string m_delimiters;
};

//CPP file
const std::string Tokenizer::DELIMITERS(" \t\n\r");

Tokenizer::Tokenizer(const std::string& s) :
		m_string(s), m_offset(0), m_delimiters(DELIMITERS) {
}

Tokenizer::Tokenizer(const std::string& s, const std::string& delimiters) :
		m_string(s), m_offset(0), m_delimiters(delimiters) {
}

bool Tokenizer::NextToken() {
	return NextToken(m_delimiters);
}

bool Tokenizer::NextToken(const std::string& delimiters) {
	size_t i = m_string.find_first_not_of(delimiters, m_offset);
	if(std::string::npos == i) {
		m_offset = m_string.length();
		return false;
	}

	size_t j = m_string.find_first_of(delimiters, i);
	if(std::string::npos == j) {
		m_token = m_string.substr(i);
		m_offset = m_string.length();
		return true;
	}

	m_token = m_string.substr(i, j - i);
	m_offset = j;
	return true;
}

const std::string Tokenizer::GetToken() const {
	return m_token;
}

void Tokenizer::Reset() {
}

int teste(std::function<int (int, int)> func) {
	return func(10, 10);
}

int xx(int x, int y) {
	return x + y;
}

struct Teste {
	int z;

	Teste(int z) : z(z) {
	}

	std::function<int (int, int)> func() {
		return [&](int x, int y) -> int {
			return x + y + z;
		};
	}
};

void my_printf(const char *s) {
	while(*s) {
		if(*s == '%' && *(++s) != '%')
			throw ("invalid format string: missing arguments");
		std::cout << *s++;
	}
}

template<typename T, typename ... Args>
void my_printf(const char *s, T firstValue, Args ... restValues) {
	while(*s) {
		if(*s == '%' && *(++s) != '%') {
			std::cout << firstValue;
			++s;
			my_printf(s, restValues...); // call even when *s == 0 to detect extra arguments
			return;
		}
		std::cout << *s++;
	}

	throw ("extra arguments provided to printf");
}

std::string getName() {
	return "marrony";
}

void printReference(const std::string& str) {
	std::cout << str;
}

void printReference(std::string&& str) {
	std::cout << str;
}

template<int C>
struct Vec {
	float v[C];
};

template<int C, int V>
struct st_vec {
	INLINE static float dot(const Vec<C>& v0, const Vec<C>& v1) {
		return v0.v[V - 1] * v1.v[V - 1] + st_vec<C, V - 1>::dot(v0, v1);
	}

	INLINE static void add(const Vec<C>& v0, const Vec<C>& v1, Vec<C>& r) {
		st_vec<C, V - 1>::add(v0, v1, r);

		r.v[V - 1] = v0.v[V - 1] + v1.v[V - 1];
	}
};

template<int C>
struct st_vec<C, 1> {
	INLINE static float dot(const Vec<C>& v0, const Vec<C>& v1) {
		return v0.v[0] * v1.v[0];
	}

	INLINE static void add(const Vec<C>& v0, const Vec<C>& v1, Vec<C>& r) {
		r.v[0] = v0.v[0] + v1.v[0];
	}
};

template<int C>
INLINE float dot(const Vec<C>& v0, const Vec<C>& v1) {
	return st_vec<C, C>::dot(v0, v1);
}

template<int C>
INLINE Vec<C> add(const Vec<C>& v0, const Vec<C>& v1) {
	Vec<C> r;

	st_vec<C, C>::add(v0, v1, r);

	return r;
}

template<typename T>
struct Producer {
	typedef std::function<T()> Type;
};

template<typename T>
struct Consumer {
	typedef std::function<void(T)> Type;
};

template<typename O, typename I>
struct Functor {
	typedef std::function<O(I)> Type;
};

using namespace engine;

struct Input {
	virtual void update(int slot, float value) = 0;
};

class Wire {
	float value;
	std::vector<std::pair<int, Input*>> connections;

public:
	Wire() :
			value(0), connections() {
	}

	float setValue(float newValue) {
		float oldValue = value;

		value = newValue;

		if(oldValue != newValue)
			update();
	}

	float getValue() {
		return value;
	}

	void addConnection(int slot, Input* input) {
		connections.push_back({slot, input});
		update();
	}

	void removeConnection(int slot, Input* input) {
	}

	void update() {
		for(auto c : connections)
			c.second->update(c.first, value);
	}
};

struct Gate {
	Wire output;

	ACCEPT_METHOD() = 0;
};

enum Semmantic {
	Position,
	Normal
};

enum ShaderType {
	Deferred,
	Forward,
};

class NewEffect : public Input {
	std::map<Semmantic, Gate*> inputs;
	std::vector<std::string> samplers;
	std::map<std::string, std::string> shaders;
public:
	void setInput(Semmantic semmantic, Gate* gate) {
		int slot = semmantic;

		if(inputs[semmantic])
			inputs[semmantic]->output.removeConnection(slot, this);

		inputs[semmantic] = gate;

		if(gate)
			gate->output.addConnection(slot, this);
	}

	void addSampler(const std::string& sampler) {
		samplers.push_back(sampler);
	}

	std::vector<std::string> getSamplers() {
		return samplers;
	}

	virtual void update(int slot, float value) {
		std::cout << "slot: " << slot << ", value: " << value << std::endl;
	}

	std::string getShader(const std::string& shader) {
		return shaders[shader];
	}

	void compileShader(ShaderType shaderType);
};

DECLARE_VISITOR(SinGate);

class SinGate : public Gate, public Input {
	Gate* input;
public:
	SinGate(Gate* gate) : input(0) {
		setInput(gate);
	}

	void setInput(Gate* gate) {
		if(input)
			input->output.removeConnection(0, this);

		input = gate;

		if(input)
			input->output.addConnection(0, this);
	}

	Gate* getInput() {
		return input;
	}

	virtual void update(int slot, float value) {
		output.setValue(std::sin(value));
	}

	ACCEPT_METHOD();
};

DEFINE_VISITOR(SinGate);

DECLARE_VISITOR(SumGate);

class SumGate : public Gate, public Input {
	Gate* input0;
	Gate* input1;

	float v0;
	float v1;

	void updateOutput() {
		output.setValue(v0 + v1);
	}
public:
	SumGate(Gate* i0, Gate* i1) : v0(0), v1(0), input0(0), input1(0) {
		setInput0(i0);
		setInput1(i1);
	}

	void setInput0(Gate* gate) {
		if(input0)
			input0->output.removeConnection(0, this);

		input0 = gate;

		if(input0)
			input0->output.addConnection(0, this);
	}

	void setInput1(Gate* gate) {
		if(input1)
			input1->output.removeConnection(1, this);

		input1 = gate;

		if(input1)
			input1->output.addConnection(1, this);
	}

	Gate* getInput0() {
		return input0;
	}

	Gate* getInput1() {
		return input1;
	}

	virtual void update(int slot, float value) {
		switch(slot) {
		case 0:
			v0 = value;
			updateOutput();
			break;

		case 1:
			v1 = value;
			updateOutput();
			break;
		}
	}

	ACCEPT_METHOD();
};

DEFINE_VISITOR(SumGate);

DECLARE_VISITOR(ConstantGate);

class ConstantGate : public Gate {
public:
	ConstantGate(float value) {
		setValue(value);
	}

	void setValue(float value) {
		output.setValue(value);
	}

	float getValue() {
		return output.getValue();
	}

	ACCEPT_METHOD();
};

DEFINE_VISITOR(ConstantGate);

DECLARE_VISITOR(Sampler2DGate);

class Sampler2DGate : public Gate {
	Gate* input;
public:
	Sampler2DGate(Gate* gate) :
			input(0) {
		setInput(gate);
	}

	void setInput(Gate* gate) {
		input = gate;
	}

	Gate* getInput() {
		return input;
	}

	ACCEPT_METHOD();
};

DEFINE_VISITOR(Sampler2DGate);

class ShaderGenerator : public engine::Visitor,
								public ConstantGateVisitor,
								public SumGateVisitor,
								public SinGateVisitor,
								public Sampler2DGateVisitor {
	NewEffect* effect;
	std::vector<std::string> globals;
	std::vector<std::string> code;
	std::map<Gate*, std::string> localNames;
	std::map<Gate*, std::string> globalNames;
	int localCount;
	int globalCount;

	std::string getLocalName(Gate* gate) {
		auto ite = localNames.find(gate);

		if(ite != localNames.end())
			return ite->second;

		char name[32];
		snprintf(name, sizeof(name), "var%d", localCount++);

		localNames[gate] = name;

		return std::string(name);
	}

	std::string getGlobalName(Gate* gate) {
		auto ite = globalNames.find(gate);

		if(ite != globalNames.end())
			return ite->second;

		char name[32];
		snprintf(name, sizeof(name), "global%d", globalCount++);

		globalNames[gate] = name;

		return std::string(name);
	}

	bool isVisited(Gate* gate) {
		return localNames.find(gate) != localNames.end();
	}
public:
	ShaderGenerator(NewEffect* effect) :
			effect(effect), localCount(0), globalCount(0) {
	}

	void addCode(const std::string& c) {
		code.push_back(c);
	}

	void addGlobal(const std::string& c) {
		globals.push_back(c);
	}

	virtual void visit(ConstantGate* gate) {
		if(isVisited(gate)) return;

		std::string name = getLocalName(gate);

		std::stringstream ss;
		ss << name << " = " << gate->output.getValue() << ";";
		addCode(ss.str());
	}

	virtual void visit(SumGate* gate) {
		if(isVisited(gate)) return;

		gate->getInput0()->accept(this);
		gate->getInput1()->accept(this);

		std::string name = getLocalName(gate);
		std::string name0 = getLocalName(gate->getInput0());
		std::string name1 = getLocalName(gate->getInput1());

		std::stringstream ss;
		ss << name << " = " << name0 << " + " << name1 << ";";
		addCode(ss.str());
	}

	virtual void visit(SinGate* gate) {
		if(isVisited(gate)) return;

		gate->getInput()->accept(this);

		std::string name = getLocalName(gate);
		std::string name0 = getLocalName(gate->getInput());

		std::stringstream ss;
		ss << name << " = sin(" << name0 << ");";
		addCode(ss.str());
	}

	virtual void visit(Sampler2DGate* gate) {
		if(isVisited(gate)) return;

		gate->getInput()->accept(this);


		std::string globalName = getGlobalName(gate);
		std::string localName = getLocalName(gate);
		std::string name0 = getLocalName(gate->getInput());

		effect->addSampler(globalName);
		addGlobal("sampler2D " + globalName + ";");

		std::stringstream ss;
		ss << localName << " = texture2D(" << globalName << ", " << name0 << ");";
		addCode(ss.str());
	}

	std::string generateSource() {
		std::stringstream ss;

		for(auto s : globals)
			ss << s << std::endl;

		ss << std::endl;

		ss << "void main() {" << std::endl;

		for(auto s : code)
			ss << '\t' << s << std::endl;

		ss << "}" << std::endl;

		return ss.str();
	}

	void generateFor(const std::string& name, Gate* gate) {
		gate->accept(this);

		std::stringstream ss;
		ss << name << " = " << getLocalName(gate) << ";";
		addCode(ss.str());
	}
};

void NewEffect::compileShader(ShaderType shaderType) {
	if(shaderType == ShaderType::Forward) {
		ShaderGenerator generator(this);

		if(inputs[Semmantic::Normal]) {
			generator.addGlobal("varying vec3 normal;");
			generator.generateFor("normal", inputs[Semmantic::Normal]);
		}

		if(inputs[Semmantic::Position])
			generator.generateFor("gl_Position", inputs[Semmantic::Position]);

		shaders["forward"] = generator.generateSource();
	}

	if(shaderType == ShaderType::Deferred) {
		{
			ShaderGenerator zpass(this);

			if(inputs[Semmantic::Position])
				zpass.generateFor("gl_Position", inputs[Semmantic::Position]);

			shaders["zpass"] = zpass.generateSource();
		}

		{
			ShaderGenerator lightPass(this);

			if(inputs[Semmantic::Normal]) {
				lightPass.addGlobal("varying vec3 normal;");
				lightPass.generateFor("normal", inputs[Semmantic::Normal]);
			}

			if(inputs[Semmantic::Position])
				lightPass.generateFor("gl_Position", inputs[Semmantic::Position]);

			shaders["lightPass"] = lightPass.generateSource();
		}
	}
}

class NewMaterial {
	NewEffect* effect;
	std::map<std::string, std::string> samplers;
public:
	NewMaterial(NewEffect* effect) {
		setEffect(effect);
	}

	void setEffect(NewEffect* effect) {
		this->effect = effect;

		for(const std::string& s : effect->getSamplers())
			samplers[s] = "sampler";
	}

	std::map<std::string, std::string> getSamplers() {
		return samplers;
	}
};

/*
 * todo: criar visitors para gerar codigo de acordo
 * com o contexto de renderizacao, ex:
 * foward rendering
 * deferred rendering
 */

int main(int argc, char* argv[]) {
	NewEffect effect;

	ConstantGate* s = new ConstantGate(0);
	ConstantGate* c10 = new ConstantGate(10);
	ConstantGate* c20 = new ConstantGate(20);
	SinGate* sinValue = new SinGate(s);
	SumGate* sumValue = new SumGate(sinValue, c10);
	Sampler2DGate* sampler = new Sampler2DGate(sumValue);
	SumGate* sumValue2 = new SumGate(sampler, c20);

	effect.setInput(Semmantic::Normal, sumValue2);
	effect.setInput(Semmantic::Position, sumValue);

	effect.compileShader(ShaderType::Deferred);
	std::cout << effect.getShader("zpass") << std::endl;
	std::cout << effect.getShader("lightPass") << std::endl;

	NewMaterial material(&effect);

	for(auto s : material.getSamplers())
		std::cout << s.first << " " << s.second << std::endl;

	return 0;
}

