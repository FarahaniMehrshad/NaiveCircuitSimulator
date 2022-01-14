# Naive Circuit Simulator
It's a simple electric circuit solver written in C++. It actually can not solve all kind of circuits, but only the series and parallel ones. You can easily draw a circuit including a number of wires, resistors and batteries, and it calculates the current and voltage through resistors and batteries.

**You can download binaries here:**  link

![Naive Circuit Simulator](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/main.png)

 # Table of contents
 

 - [How to use?](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/#how-to-use)
 - [How to build?](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/#how-to-build)
 - [Circuit Core](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/#circuit-core)
 - [Circuit Gui](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/#circuit-gui)
 - [How does it work?](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/#how-does-it-work)

# How to use?
There is a toolbar at the right side. You can select different items like: wire, resistor, voltage source(battery), and blank(for removing an element).
You can also use shortcut keys: **W** for wire, **R** for resistor, **V** for voltage source, **B** for blank, and **ESC** to unselect an item.
After you done with drawing, move your cursor on an element to see its current and voltage.

![Naive Circuit Simulator](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/CircuitGif.gif)

# How to build?
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
You can use the core itself to solve circuits without using gui.
Consider this circuit:

![Simple Electric Circuit](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/Circuit.PNG)

First we give all nodes and elements a unique name like so:

![](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/Circuit2.png)

Then we ask the core to solve the circuit for us:

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
		// You can also use printElements() function
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

You could also omit the wires by giving the nodes that have same potential a same name:

![](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/Circuit3.png)

    circuit->addResistor("R1", 5, "a", "b");
	circuit->addResistor("R2", 5, "c", "d");
	circuit->addBattery("B1", 24, "b", "c");
	circuit->addBattery("B2", 12, "a", "d");

***Caution**: Once a circuit gets solved, it will be marked as **dirty** and you can not modify it. You should delete the circuit and create another one.*
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
The graphical part works completely separate from core. You may want to use the program's gui and implement the circuit solving algorithm yourself.
Fine! just take care of these functions. These are the functions which are responsible for creating a circuit and reading data from it.

    void CircuitGui::createAndSolveCircuit()
    void CircuitGui::drawItemInfo(Item & item)

# How does it work?
While it is not very easy to explain the algorithm in details, but let's have a try.
Consider this circuit:

![](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/SampleCircuit.PNG)

Now let's analyze it:

![](https://github.com/FarahaniMehrshad/NaiveCircuitSimulator/blob/master/Pics/CircuitAnalyze.png)

We can calculate the equivalent resistance with some basic formulas:
Series: **Rt = R1 + R2**
Parallel Rt = (**R1 ^ -1 + R2 ^ -1) ^ -1**

Now we continue merging elements and calculate the equivalent resistance in each steps, till there's only one element left.
At this point we can calculate the total current in circuit using **I = V / Rt** formula.
Then we continue unmerging elements and spread the current through each elements.

If you focus, this process is actually a tree like structure on its nature.

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















