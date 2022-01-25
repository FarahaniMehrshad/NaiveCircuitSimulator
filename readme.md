# Naive Circuit Simulator
It's an electric circuit solver written in C++, and it's called naive because it can only solve series and parallel circuits. You can use it to calculate the voltage and current of each element in the circuit.

**You can download binaries [here](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/releases/tag/v1.1)** 

![Naive Circuit Simulator](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/main.png)

 # Table of contents
 

 - [How to use?](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/#how-to-use)
 - [Build](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/#build)
 - [Circuit Core](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/#circuit-core)
 - [Circuit Gui](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/#circuit-gui)
 - [The algorithm](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/#how-does-it-work)
 - [Thanks](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/#thanks)

# How to use?
There is a toolbar on the right. You can choose between different items such as wire, resistor, a voltage source (battery), and blank (to remove elements in the circuit). By moving the mouse pointer on each element, you can quickly draw a circuit and see the current and voltage. You can also use the shortcut keys corresponding to each item.

![Naive Circuit Simulator](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/CircuitGif.gif)

# Build
First make sure you're Mingw compiler is up-to-date. Then put all files into the same folder and follow the instructions:
### Windows
Build:

    g++ -o NaiveCircuitSimulator.exe main.cpp CircuitCore.cpp CircuitGui.cpp -luser32 -lgdi32 -lopengl32 -lgdiplus -lShlwapi -ldwmapi -lstdc++fs -static -std=c++17
 Run:
 

    NaiveCircuitSimulator.exe

### Linux
##### Ubuntu 20.04 and distros based on Ubuntu 20.04
Update and install the compiler, related build tools, and libraries required to build.

    sudo apt update
    sudo apt install build-essential libglu1-mesa-dev libpng-dev

Build:

    g++ -o NaiveCircuitSimulator main.cpp CircuitCore.cpp CircuitGui.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17
Run:

    ./NaiveCircuitSimulator

# Circuit Core
You can use the circuit core to solve circuits without the need for a graphical environment.
Consider this circuit:

![Simple Electric Circuit](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/Circuit.PNG)

First, we give all nodes and elements a unique name like so:

![](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/Circuit2.png)

Then we ask the core to solve the circuit for us.

 ``` cpp
 #include "CircuitCore.h"
#include "mfLinkedList.h"
#include <iostream>
#include <math.h>

int main()
{
	CircuitCore* circuit = new CircuitCore();
	
	circuit->addResistor("R1", 5, "b", "c");
	circuit->addResistor("R2", 5, "f", "g");
	circuit->addBattery("B1", 24, "d", "e");
	circuit->addBattery("B2", 12, "j", "i");
	circuit->addWire("W1", "a", "k");
	circuit->addWire("W2", "a", "b");
	circuit->addWire("W3", "c", "d");
	circuit->addWire("W4", "e", "f");
	circuit->addWire("W5", "g", "h");
	circuit->addWire("W6", "h", "i");
	circuit->addWire("W7", "j", "k");
	
	try
	{
		circuit->solve();
		
		auto elementsList = circuit->getElementsList();
	
		for(Element* element : elementsList)
		{
			std::cout << element->getName() << " ";
			std::cout << "I: " << abs(element->getCurrent()) << " ";
			std::cout << "V: " << abs(element->getVoltage()) << " ";
			std::cout << "R: " << element->getResistance() << " ";
			std::cout << std::endl;
		}
	}
	catch(CircuitCore::Errors error)
	{
		switch(error)
		{
		case CircuitCore::Errors::NO_RESISTOR:
			break;

		case CircuitCore::Errors::NO_VOLTAGE_SOURCE:
			break;

		case CircuitCore::Errors::NOT_CONNECTED:
			break;

		case CircuitCore::Errors::NOT_SERIES_NOT_PARALLEL:
			break;

		case CircuitCore::Errors::SHORT_CIRCUIT:
			break;

		case CircuitCore::Errors::SHORT_CIRCUIT_WITH_BATTERY:
			break;
		}
	}
	
	delete circuit;
	circuit = nullptr;
	
	return 0;
}
 
 ```

Will print:

    R1 I: 1.2 V: 6 R: 5
    B1 I: 1.2 V: 24 R: 0
    R2 I: 1.2 V: 6 R: 5
    B2 I: 1.2 V: 12 R: 0

You could also omit the wires by giving the nodes with the same potential an identical name:

![](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/Circuit3.png)

    circuit->addResistor("R1", 5, "a", "b");
	circuit->addResistor("R2", 5, "c", "d");
	circuit->addBattery("B1", 24, "b", "c");
	circuit->addBattery("B2", 12, "a", "d");

***Caution**: Once a circuit gets solved, it will be marked as **dirty** and you cannot modify it. You should delete the circuit and create another one.*
### List of functions
Here is the list of functions you can use:

    Element* addWire(std::string name, std::string negativeSide, std::string positiveSide)
	
    Element* addResistor(std::string name, double resistance, std::string negativeSide, std::string positiveSide)
    
	Element* addBattery(std::string name, double voltage, std::string negativeSide, std::string positiveSide)
    
	Element* removeElement(std::string name)
    
	Element* searchElement(std::string name) const
    
	mf::LinkedList<Element*> getElemenetsList() const
    
	void solve()
    
	bool dirty()

# Circuit Gui
The code of the graphic part of the program is written entirely independent of the core. You may prefer to use only the program graphics and implement the circuit-solving algorithm yourself. There are only two functions that communicate with the core, and by changing these functions, you can reach your goal.

    void CircuitGui::createAndSolveCircuit()
    void CircuitGui::drawItemInfo(Item & item)

# The algorithm
While it is not very easy to explain the algorithm in detail, let's try.
Consider this circuit:

![](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/SampleCircuit.PNG)

Now let's analyze it:

![](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/CircuitAnalyze.png)

We can calculate the equivalent resistance with some basic formulas:
Series: **Rt = R1 + R2**
Parallel Rt = (**R1 ^ -1 + R2 ^ -1) ^ -1**

We keep merging elements and calculate the equivalent resistance in each step till there's only one element left.
Using the I = V / Rt formula, we can calculate the total current in circuit.
Then we continue unmerging elements and spread the current through each element.

Now If you focus, this process is a tree-like structure in its nature.

The **“—”** symbol is used here to represent **“series,”** just as the **“||”** symbol is used to represent **“parallel.”**

![](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/Tree.png)

Now we came up with a structure like this:

```
    Element
    {
	    name
	    current
	    voltage
	    resistance
	    child1
	    child2
	    children_connection(series or parallel)
    }
```

# Thanks
Special thanks to my professor at the University of Isfahan, Dr. Kamal Jamshidi, who inspired me to make this program.















