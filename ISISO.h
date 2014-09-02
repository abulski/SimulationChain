/** \interface ISISO
 * Intefrace to be used in the simulation loop. Part of the composite design pattern.
 *
 * \par
 * Main features:
 * - simulate method with one input and one output,
 * - saving and loading state - serialization,
 * - enables recording both input and output of every object into i/o stream,
 * - can be used to create a self-organising tree,
 * - automatic parent/children relation handling, with only one parent and many children,
 * - deallocating children of the destroyed node based on smart pointers,
 * - searching objects using BFS or DFS algorithm.
*/

#define _DEBUG

#include <fstream>
#include <vector>
#include <list>
#include <memory>
#include "boost\property_tree\ptree.hpp"
#include "ObjType.h"
#include "mainwindow.h"
#include "Historian.h"

#ifndef _ISISO
#define _ISISO

class ISISO
{
public:
    /// \brief Outputs one step of the simualtion given one sample of the input.
    /// \return Next simulation output.
	virtual double Simulate(double) = 0;
	
    /// \brief Outputs all output samples recorded by far.
	virtual void GetOutputHistory(std::vector<double>&) const = 0;

    /// \brief Deletes all recorded samples of output.
	virtual void EraseOutputHistory() = 0;

    /// \brief Set output history.
    virtual void SetOutputHistory(std::vector<double>&) = 0;

    /// \brief Set input history.
    virtual void SetInputHistory(std::vector<double>&) = 0;

    /// \brief Adds internal object state data to the stream object.
	virtual void SaveState(boost::property_tree::ptree&) const = 0;

    /// \brief Load internal object state data from tree. Each object manages the data itself.
	virtual void LoadState(boost::property_tree::ptree::value_type const&) = 0;

    /// \brief Enables user to set an ID of the object. User must ensure that the name is unique.
	virtual void SetID(int) = 0;

    /// \brief Fetches object's ID.
    /// \return ID of the object.
	virtual int GetID() const = 0;

    /// \brief Registers a name with the object. User must ensure that the name is unique.
	virtual void SetName(std::string&) = 0;

    /// \brief Fetches object's name.
    /// \return Reference to the objects name.
	virtual const std::string& GetName() const = 0;

    /// \brief Builds a tree containing names and types of all children nodes.
    virtual void GetName(boost::property_tree::ptree&) const = 0;

    /// \brief Sets object's parent. There can be only one parent of the same object.
    virtual void SetParent(ISISO*) = 0;

    /// \brief Fetches object's parent pointer.
    /// \return Pointer to the parent.
	virtual ISISO* GetParent() = 0;

    /// \brief Allows adding children to the object.
	virtual void AddChild(std::shared_ptr<ISISO>) = 0;

    /// \brief Removes a child of the object.
	virtual void RemoveChild(std::shared_ptr<ISISO>) = 0;

    /// \brief Fetches the list of object's children.
	virtual void GetChildren(std::list<std::weak_ptr<ISISO> >&) const = 0;

    /// \brief Allows to remove the object. Function searches all the children of the object.
    /// \return True if object successfuly removed.
	virtual bool RemoveObject(int) = 0;

    /// \brief Searches all the children of the object to find object with the specified name.
    /// \return Pointer to found object or <b>nullptr</b>.
    virtual ISISO* SearchObject(const std::string&) = 0;

    /// \brief Searches the tree for the first regulator object. Implements BFS algorithm.
    /// \return Return pointer to first found regulator.
    virtual ISISO* FindFirstRegulator() = 0;

    /// \brief Retrive object functional type.
    /// \return Returns type of the object.
	virtual ObjType GetType() const = 0;
	
    /// \brief Set object functional type.
	virtual void SetType(ObjType) = 0;

    /// \brief Set the output stream for object output values.
	virtual void SetStreamForOutput(std::shared_ptr<std::ostream>) = 0;

    /// \brief Set the variable to store the current output value.
    virtual void SetVariableToStoreCurrentOutput(std::weak_ptr<double>) = 0;

    /// \brief Set the variable to store the current input value.
    virtual void SetVariableToStoreCurrentInput(std::weak_ptr<double>) = 0;

    /// \brief Disable the output stream for objects input values.
	virtual void RemoveStreamForOutput() = 0;

    /// \brief Move object to the front so it can be run first.
	virtual bool MoveObjectToFront(ISISO* FrontObject) = 0;

    /// \brief Resets objects memory (reset generators if present).
    virtual void ResetMemory() = 0;

    virtual ~ISISO() {}
};

#endif
