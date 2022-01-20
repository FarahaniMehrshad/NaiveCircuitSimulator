#ifndef MF_CIRCUIT_DEF
#define MF_CIRCUIT_DEF

#include <string>
#include "mfLinkedList.h"

class Node;
class Element;
class CircuitCore;

class Node
{
	friend class CircuitCore;
private:
	Node();
	Node(std::string name);
	std::string getName() const;

private:
	std::string _name;
	mf::LinkedList<Element*> _elements;
};

class Element
{
	friend class CircuitCore;
public:
	std::string getName() const;
	double getVoltage() const;
	double getResistance() const;
	double getCurrent() const;

private:
	Element();
	Element(std::string name, double voltage, double current, double resistance	, Node* node1, Node* node2);

private:
	std::string _name;
	double _voltage = 0.0;
	double _current = 0.0;
	double _resistance = 0.0;
	Node* _node1 = nullptr;
	Node* _node2 = nullptr;
	Element* _left = nullptr;
	Element* _right = nullptr;
	int _childrenConnections = 0;
};

class CircuitCore
{
public:

	enum ConnectionsType
	{
		NONE,
		SERIES,
		PARALLEL,
	};

	enum Errors
	{
		DIRTY_CIRCUIT,
		ELEMENT_ALREADY_EXIST,
		NO_ELEMENT_TO_REMOVE,
		NO_ELEMENT,
		NO_RESISTOR,
		NO_VOLTAGE_SOURCE,
		NOT_CONNECTED,
		NOT_SERIES_NOT_PARALLEL,
		SHORT_CIRCUIT,
		SHORT_CIRCUIT_WITH_BATTERY,
		MERGE_FAILED,
		UNMERGE_FAILED,
		TWO_SAME_NODES,
	};

public:
	CircuitCore();
	~CircuitCore();

	Element* addWire(std::string name, std::string negativeSide, std::string positiveSide);
	Element* addResistor(std::string name, double resistance, std::string negativeSide, std::string positiveSide);
	Element* addBattery(std::string name, double voltage, std::string negativeSide, std::string positiveSide);
	Element* removeElement(std::string name);
	Element* searchElement(std::string name) const;
	mf::LinkedList<Element*> getElementsList() const;
	void solve();
	bool dirty() const;

public:
	void printElements() const;
	void printNodes() const;
	void printConnections() const;

private:
	Element* addElement(std::string name, double voltage, double current, double resistance, std::string negativeSide, std::string positiveSide);
	Element* addElement(Element* element);
	Element* removeElement(Element* element);
	Node* searchOrCreateNode(std::string name);
	Element* merge(Element* el1, Element* el2);
	void removeAndBindElement(Element* element);
	void unmerge(Element* element);

	void validate() const;
	bool isBattery(Element* element) const;
	bool isWire(Element* element) const;
	int connection(Element* el1, Element* el2) const;
	Node* searchNode(std::string name) const;

private:
	bool isDirty = false;
	mf::LinkedList<Element*> _elements;
	mf::LinkedList<Node*> _nodes;
};



#endif // MF_CIRCUIT_DEF