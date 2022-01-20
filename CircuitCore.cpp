#include "CircuitCore.h"
#include <iostream>
#include "math.h"

/*
================= Public realization of class Node =================
*/

Node::Node() { }

Node::Node(std::string name) : _name(name) { }

std::string Node::getName() const { return _name; }

/*
================= Public realization of class Element =================
*/

std::string Element::getName() const { return _name; }

double Element::getVoltage() const { return _voltage; }

double Element::getResistance() const { return _resistance; }

double Element::getCurrent() const { return _current; }

Element::Element(std::string name, double voltage, double current, double resistance, Node* node1, Node* node2) {
	_name = name;
	_voltage = voltage;
	_current = current;
	_resistance = resistance;
	_node1 = node1;
	_node2 = node2;
}

/*
================= Public realization of class CircuitCore =================
*/

CircuitCore::CircuitCore(){}

CircuitCore::~CircuitCore()
{
	for (Element* element : _elements) delete element;
	for (Node* node : _nodes) delete node;
}

Element* CircuitCore::addWire(std::string name, std::string negativeSide, std::string positiveSide)
{
	if (negativeSide == positiveSide)
		throw TWO_SAME_NODES;

	return addElement(name, 0, 0, 0, negativeSide, positiveSide);
}

Element* CircuitCore::addResistor(std::string name, double resistance, std::string negativeSide, std::string positiveSide)
{
	if (negativeSide == positiveSide)
		throw TWO_SAME_NODES;

	return addElement(name, 0, 0, resistance, negativeSide, positiveSide);
}

Element* CircuitCore::addBattery(std::string name, double voltage, std::string negativeSide, std::string positiveSide)
{
	if (negativeSide == positiveSide)
		throw TWO_SAME_NODES;

	return addElement(name, voltage, 0, 0, negativeSide, positiveSide);
}

Element* CircuitCore::removeElement(std::string name)
{
	Element* element = searchElement(name);
	if (element == nullptr)
		throw NO_ELEMENT_TO_REMOVE;

	removeElement(element);

	return element;
}

Element* CircuitCore::searchElement(std::string name) const
{
	for (Element* element : _elements)
	{
		if (element->getName() == name)
			return element;
	}
	return nullptr;
}

mf::LinkedList<Element*> CircuitCore::getElementsList() const
{
	return _elements;
}

void CircuitCore::solve()
{
	if (dirty())
		throw DIRTY_CIRCUIT;

	validate();

	// Remove wires
	for (int i = 0; i < _elements.getSize(); ++i)
	{
		Element* element = _elements[i];
		if (isWire(element)){
			removeAndBindElement(element);
			i = -1;
		}
	}

	bool allowMergeWithBattery = false;
	while (_elements.getSize() != 1)
	{
		for (int i = 0; i < _elements.getSize() - 1; ++i)
		{
			for (int j = i + 1; j < _elements.getSize(); ++j)
			{
				Element* el1 = _elements[i];
				Element* el2 = _elements[j];
				int cxn = connection(el1, el2);

				if (cxn == NONE)
					continue;
				if (!allowMergeWithBattery)
					if (isBattery(el1) || isBattery(el2))
						continue;

				merge(el1, el2);
				goto exit;
			}
		}
		if (!allowMergeWithBattery)
			allowMergeWithBattery = true;
		else
		{
			isDirty = true;
			throw NOT_SERIES_NOT_PARALLEL;
		}

	exit:;
	}

	Element* leftoverElement = _elements[0];
	leftoverElement->_current = leftoverElement->_voltage / leftoverElement->_resistance;

	unmerge(leftoverElement);
	isDirty = true;
}

bool CircuitCore::dirty() const
{
	return isDirty;
}

/*
================= Private realization of class CircuitCore =================
*/

Element* CircuitCore::addElement(std::string name, double voltage, double current, double resistance, std::string negativeSide, std::string positiveSide)
{
	if (searchElement(name))
		throw ELEMENT_ALREADY_EXIST;

	Node* node1 = searchOrCreateNode(negativeSide);
	Node* node2 = searchOrCreateNode(positiveSide);
	Element* element = new Element(name, voltage, current, resistance, node1, node2);

	_elements.pushFront(element);
	node1->_elements.pushBack(element);
	node2->_elements.pushBack(element);
	return element;
}

Element * CircuitCore::addElement(Element * element)
{
	if (searchElement(element->getName()))
		throw ELEMENT_ALREADY_EXIST;

	// Create nodes
	Node* node1 = searchOrCreateNode(element->_node1->getName());
	Node* node2 = searchOrCreateNode(element->_node2->getName());

	// Attach element to circuit
	_elements.pushFront(element);

	// Attach element to nodes
	node1->_elements.pushBack(element);
	node2->_elements.pushBack(element);

	return element;
}

Element* CircuitCore::removeElement(Element * element)
{
	element->_node1->_elements.remove(element);
	element->_node2->_elements.remove(element);
	_elements.remove(element);

	return element;
}

void CircuitCore::removeAndBindElement(Element* element)
{
	// We save a node, attach other elements to it, and remove the other one
	Node* savedNode = element->_node1;
	Node* notSavedNode = element->_node2;

	// Make sure the element is not parallel to anything
	// then deattach all elements connected to notSavedNode
	// and connect them to savedNode.

	bool isParallel = false;
	for (Element* neighbor : element->_node1->_elements)
	{
		if (neighbor == element) continue;
		if (connection(element, neighbor) == PARALLEL)
		{
			isParallel = true;
			break;
		}
	}

	if(!isParallel)
		for (Element* neighbor : element->_node2->_elements)
		{
			if (neighbor == element) continue;
			if (connection(element, neighbor) == PARALLEL)
			{
				isParallel = true;
				break;
			}
		}

	if (isParallel && isWire(element))
		throw SHORT_CIRCUIT;

	if (!isParallel)
	{
		while (notSavedNode->_elements.getSize() != 0)
		{
			Element* other = notSavedNode->_elements.getHead()->getData();

			if (other == element)
			{
				notSavedNode->_elements.remove(element);
				continue;
			}

			// Attach the savedNode to the element
			if (other->_node1 == notSavedNode)
			{
				other->_node1 = savedNode;
			}
			else if (other->_node2 == notSavedNode)
			{
				other->_node2 = savedNode;
			}

			// Deattach the other element from the old node
			notSavedNode->_elements.remove(other);
			// Attach the other element to the saved node (new node)
			savedNode->_elements.pushFront(other);
		}
	}

	// Deattach the leftover empty node from circuit
	if (notSavedNode->_elements.getSize() == 0)
	{
		_nodes.remove(notSavedNode);
		notSavedNode = nullptr;
	}

	// Deattach the element from its node
	element->_node1->_elements.remove(element);
	if (notSavedNode != nullptr)
		element->_node2->_elements.remove(element);

	// Deattach from circuit
	_elements.remove(element);
	
}

void CircuitCore::validate() const
{
	if (_elements.getSize() == 0)
		throw NO_ELEMENT;

	int batteryCount = 0;
	int resistorCount = 0;
	for (Element* element : _elements)
	{
		if (isBattery(element)) ++batteryCount;
		if (element->_resistance > 0) ++resistorCount;

		if (element->_node1->_elements.getSize() == 1)
			throw NOT_CONNECTED;
		if (element->_node2->_elements.getSize() == 1)
			throw NOT_CONNECTED;
	}

	if (batteryCount == 0) throw NO_VOLTAGE_SOURCE;
	if (resistorCount == 0) throw NO_RESISTOR;
}

Element * CircuitCore::merge(Element * el1, Element * el2)
{
	int cxn = connection(el1, el2);
	if (cxn == NONE)
		throw MERGE_FAILED;
	/*
		Are batteries in the same direction or not?
		if not, negative the voltage of one of them
	*/
	if (isBattery(el1) && isBattery(el2))
		if (el1->_node1 == el2->_node1 || el1->_node2 == el2->_node2)
			el2->_voltage *= -1;

	// Construct new element
	std::string name = el1->getName() + "+" + el2->getName();
	double voltage = 0.0;
	double current = 0.0;
	double resistance = 0.0;
	Node * node1 = nullptr;
	Node * node2 = nullptr;

	if (cxn == SERIES)
	{
		resistance = el1->_resistance + el2->_resistance;
		voltage = el1->_voltage + el2->_voltage;

		// Remove the common node and save the other nodes for further using
		Node* commonNode = nullptr;
		if (el1->_node1 == el2->_node1) commonNode = el1->_node1;
		if (el1->_node2 == el2->_node2) commonNode = el1->_node2;
		if (el1->_node1 == el2->_node2) commonNode = el1->_node1;
		if (el1->_node2 == el2->_node1) commonNode = el1->_node2;

		node1 = el1->_node1 == commonNode ? el1->_node2 : el1->_node1;
		node2 = el2->_node1 == commonNode ? el2->_node2 : el2->_node1;

		/*
		   The first node of battery, is its negative side
		   and the second node of battery is its possitive side
		   If an element tries to be merged with a battery, we need to make sure
		   the negative and positive sides stay the same
		*/
		if (!isBattery(el1) && isBattery(el2))
		{
			if (!(node1 == el2->_node1 || node2 == el2->_node2))
			{
				Node* temp = node1;
				node1 = node2;
				node2 = temp;
			}
		}

		if (isBattery(el1) && !isBattery(el2))
		{
			if (!(node1 == el1->_node1 || node2 == el1->_node2))
			{
				Node* temp = node1;
				node1 = node2;
				node2 = temp;
			}
		}

		if (isBattery(el1) && isBattery(el2))
		{
			if (node1 != el1->_node1 && node1 != el2->_node1)
			{
				Node* temp = node1;
				node1 = node2;
				node2 = temp;
			}
		}
	}

	if (cxn == PARALLEL)
	{
		// Check short circuit. It may happen when an element is parallel with a battery
		if (el1->_resistance < 0.000001 || el2->_resistance < 0.000001)
			throw SHORT_CIRCUIT_WITH_BATTERY;

		if (abs(el1->_voltage) > 0.00001 || abs(el2->_voltage) > 0.00001)
			throw NOT_SERIES_NOT_PARALLEL;

		resistance = 1.0 / (1.0 / el1->_resistance + 1.0 / el2->_resistance);
		node1 = el1->_node1;
		node2 = el1->_node2;
	}

	if (node1 == nullptr || node2 == nullptr)
		throw MERGE_FAILED;

	Element * newElement = addElement(name, voltage, current, resistance, node1->getName(), node2->getName());

	newElement->_left = el1;
	newElement->_right = el2;
	newElement->_childrenConnections = cxn;

	removeElement(el1);
	removeElement(el2);

	if (node1->_elements.getSize() < 2)
		throw SHORT_CIRCUIT;
	if (node2->_elements.getSize() < 2)
		throw SHORT_CIRCUIT;

	return newElement;
}

void CircuitCore::unmerge(Element * element)
{
	if (element->_left == nullptr && element->_right == nullptr)
	{
		if (!_elements.find(element))
			addElement(element);
		if (!element->_node1->_elements.find(element))
			element->_node1->_elements.pushFront(element);
		if (!element->_node2->_elements.find(element))
			element->_node2->_elements.pushFront(element);
		if (!isBattery(element))
			element->_voltage = element->_current * element->_resistance;

		return;
	}

	Element* left = element->_left;
	Element* right = element->_right;
	double current = element->_current;

	if (left == nullptr || right == nullptr)
		throw UNMERGE_FAILED;

	// Divide current
	if (element->_childrenConnections == SERIES)
	{
		left->_current = current;
		right->_current = current;
	}

	if (element->_childrenConnections == PARALLEL)
	{
		// Check short circuit
		if (left->_resistance < 0.000001)
			left->_current = current;
		else if (right->_resistance < 0.000001)
			right->_current = current;
		else {
			double ratio = left->_resistance / right->_resistance;

			left->_current = current / (ratio + 1);
			right->_current = ratio * current / (ratio + 1);
		}
	}

	unmerge(left);
	unmerge(right);

	// Remove element footsteps from circuit
	element->_node1->_elements.remove(element);
	element->_node2->_elements.remove(element);
	_elements.remove(element);

	delete element;
}

int CircuitCore::connection(Element * el1, Element * el2) const
{
	// If there are only 2 elements left in the circuit
	// they are series and parallel at the same time
	// but we have to consider them series in order to calculate the current
	// with I = V / R formula
	if (_elements.getSize() == 2) return SERIES;

	mf::LinkedList<Node*> commonNodes;
	if (el1->_node1 == el2->_node1) commonNodes.pushBack(el1->_node1);
	if (el1->_node2 == el2->_node2) commonNodes.pushBack(el1->_node2);
	if (el1->_node1 == el2->_node2) commonNodes.pushBack(el1->_node1);
	if (el1->_node2 == el2->_node1) commonNodes.pushBack(el1->_node2);

	// Series
	if (commonNodes.getSize() == 1)
	{
		Node* commonNode = commonNodes[0];
		if (commonNode->_elements.getSize() == 2) return SERIES;
	}

	// Parallel
	if (commonNodes.getSize() == 2) return PARALLEL;

	return NONE;
}

Node* CircuitCore::searchNode(std::string name) const
{
	for (Node* node : _nodes)
	{
		if (node->getName() == name)
			return node;
	}
	return nullptr;
}

Node* CircuitCore::searchOrCreateNode(std::string name)
{
	Node* node = searchNode(name);

	if (node == nullptr)
	{
		node = new Node(name);
		_nodes.pushBack(node);
	}
	return node;
}

bool CircuitCore::isBattery(Element * element) const
{
	if (element->_voltage > 0.00001 || element->_voltage < -0.00001)
		return true;

	return false;
}

bool CircuitCore::isWire(Element* element) const
{
	if (!isBattery(element) && element->_resistance < 0.00001)
		return true;
	return false;
}

void CircuitCore::printElements() const
{
	std::cout << std::endl;
	std::cout << "------------------------" << std::endl;
	std::cout << "------ Elements --------" << std::endl;
	std::cout << "------------------------" << std::endl;

	for (Element* element : _elements)
	{
		std::cout << element->getName() << " ";
		std::cout << "V: " << element->_voltage << " ";
		std::cout << "I: " << element->_current << " ";
		std::cout << "R: " << element->_resistance << " ";
		std::cout << element->_node1->getName() << " " << element->_node2->getName() << std::endl;
	}
	std::cout << std::endl;
}

void CircuitCore::printNodes() const 
{
	std::cout << std::endl;
	std::cout << "------------------------" << std::endl;
	std::cout << "-------- Nodes ---------" << std::endl;
	std::cout << "------------------------" << std::endl;

	for (Node* node : _nodes) {
		std::cout << node->getName() << ": ";
		for (Element* element : node->_elements) {
			std::cout << element->getName() << " ";
		}
		std::cout << std::endl;
	}

	std::cout << std::endl;
}

void CircuitCore::printConnections() const 
{
	std::cout << std::endl;
	std::cout << "------------------------" << std::endl;
	std::cout << "------ Connections -----" << std::endl;
	std::cout << "------------------------" << std::endl;

	for (int i = 0; i < _elements.getSize() - 1; ++i) 
	{
		for (int j = i + 1; j < _elements.getSize(); ++j)
		{
			std::cout << _elements[i]->getName() << " & " << _elements[j]->getName() << ": ";
			if (connection(_elements[i], _elements[j]) == SERIES) std::cout << "Series";
			if (connection(_elements[i], _elements[j]) == PARALLEL) std::cout << "Parallel";
			if (connection(_elements[i], _elements[j]) == NONE) std::cout << "None";
			std::cout << std::endl;
		}
	}
}

