/*
 ==============================================================================
 
 This file is part of the KIWI library.
 Copyright (c) 2014 Pierre Guillot & Eliott Paris.
 
 Permission is granted to use this software under the terms of either:
 a) the GPL v2 (or any later version)
 b) the Affero GPL v3
 
 Details of these licenses can be found at: www.gnu.org/licenses
 
 KIWI is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 ------------------------------------------------------------------------------
 
 To release a closed-source product which uses KIWI, contact : guillotpierre6@gmail.com
 
 ==============================================================================
*/

#include "KiwiObject.h"
#include "KiwiPatch.h"
#include "KiwiInstance.h"
#include "KiwiConsole.h"

namespace Kiwi
{
    // ================================================================================ //
    //                                      IOLET                                       //
    // ================================================================================ //
    
    const Object::Connection Object::Iolet::getConnection(const ulong index) const noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        if(index < (ulong)m_connections.size())
        {
            return m_connections[(vector<Connection>::size_type)index];
        }
        else
        {
            return {sObject(), 0};
        }
    }
    
    Object::Connection Object::Iolet::getConnection(const ulong index) noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        if(index < (ulong)m_connections.size())
        {
            return m_connections[(vector<Connection>::size_type)index];
        }
        else
        {
            return {sObject(), 0};
        }
    }
    
    scObject Object::Iolet::getObject(const ulong index) const noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        if(index < (ulong)m_connections.size())
        {
            return m_connections[(vector<Connection>::size_type)index].object.lock();
        }
        else
        {
            return nullptr;
        }
    }
    
    sObject Object::Iolet::getObject(const ulong index) noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        if(index < (ulong)m_connections.size())
        {
            return m_connections[(vector<Connection>::size_type)index].object.lock();
        }
        else
        {
            return nullptr;
        }
    }
    

    ulong Object::Iolet::getIndex(const ulong index) const noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        if(index < (ulong)m_connections.size())
        {
            return m_connections[(vector<Connection>::size_type)index].index;
        }
        else
        {
            return 0;
        }
    }
    
    bool Object::Iolet::has(sObject object, ulong index) const noexcept
    {
        if(object)
        {
            lock_guard<mutex> guard(m_mutex);
            for(vector<Connection>::size_type i = 0; i < m_connections.size(); i++)
            {
                sObject cobject = m_connections[i].object.lock();
                if(cobject && object == cobject && m_connections[i].index == index)
                {
                    return true;
                }
            }
        }
        return false;
    }
    
    bool Object::Iolet::append(sObject object, ulong index) noexcept
    {
        if(object)
        {
            lock_guard<mutex> guard(m_mutex);
            for(vector<Connection>::size_type i = 0; i < m_connections.size(); i++)
            {
                sObject cobject = m_connections[i].object.lock();
                if(cobject && object == cobject && m_connections[i].index == index)
                {
                    return false;
                }
            }
            m_connections.push_back({object, index});
            return true;
        }
        return false;
    }
    
    bool Object::Iolet::erase(sObject object, ulong index) noexcept
    {
        if(object)
        {
            lock_guard<mutex> guard(m_mutex);
            for(vector<Connection>::size_type i = 0; i < m_connections.size(); i++)
            {
                sObject cobject = m_connections[i].object.lock();
                if(cobject && object == cobject && m_connections[i].index == index)
                {
                    m_connections.erase(m_connections.begin() + i);
                    return true;
                }
            }
        }
        return false;
    }
    
    Object::Outlet::Outlet(Io::Type type, string const& description) noexcept :
    Iolet(type, Io::Polarity::Hot, description)
    {
        ;
    }
    
    Object::Outlet::~Outlet()
    {
        ;
    }
    
    void Object::Outlet::send(Vector const& atoms) const noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        for(vector<Connection>::size_type i = 0; i < m_connections.size(); i++)
        {
            sObject receiver = m_connections[i].object.lock();
            ulong inlet      = m_connections[i].index;
            if(receiver)
            {
                if(++receiver->m_stack_count < 256)
                {
                    receiver->receive(inlet, atoms);
                }
                else if(receiver->m_stack_count  == 256)
                {
                    receiver->receive(inlet, atoms);
                }
                else
                {
                    Console::error(receiver, "Stack overflow");
                }
                receiver->m_stack_count--;
            }
        }
    }
    
    Object::Inlet::Inlet(Io::Type type, Io::Polarity polarity, string const& description) noexcept :
    Iolet(type, polarity, description)
    {
        ;
    }
    
    Object::Inlet::~Inlet()
    {
        ;
    }
    
    // ================================================================================ //
    //                                      OBJECT                                      //
    // ================================================================================ //
    
    Object::Object(Infos const& detail, sTag name) noexcept :
    GuiSketcher(detail.instance),
    m_instance(detail.instance),
    m_patcher(detail.patcher),
    m_name(name),
    m_text(detail.text),
    m_id(detail.lid),
    m_stack_count(0)
    {
        addAttr(Attr::create("presentation_position",   "Presentation Position",    "Appearance", Point(0., 0.)));
        addAttr(Attr::create("presentation_size",       "Presentation Size",        "Appearance", Size(10., 10.)));
        addAttr(Attr::create("hidden",                  "Hide on Lock",             "Appearance", BoolValue(false)));
        addAttr(Attr::create("presentation",            "Include in presentation",  "Appearance", BoolValue(false)));
        addAttr(Attr::create("ignoreclick",             "Ignore Click",             "Behavior",   BoolValue(false)));
    }
    
    Object::~Object() noexcept
    {
        m_outlets.clear();
        m_inlets.clear();
    }
    
    void Object::write(Dico& dico) const
    {
        this->save(dico);
        Attr::Manager::write(dico);
        dico[Tag::List::name]       = getName();
        dico[Tag::List::text]       = getText();
        dico[Tag::List::id]         = (long)getId();
        dico[Tag::List::ninlets]    = (long)getNumberOfInlets();
        dico[Tag::List::noutlets]   = (long)getNumberOfInlets();
    }
    
    void Object::send(const ulong index, Vector const& atoms) const noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        if(vector<sOutlet>::size_type(index) < m_outlets.size())
        {
            m_outlets[vector<sOutlet>::size_type(index)]->send(atoms);
        }
    }

    void Object::addInlet(Io::Type type, Io::Polarity polarity, string const& description)
    {
        lock_guard<mutex> guard(m_mutex);
        sInlet inlet = make_shared<Inlet>(type, polarity, description);
        if(inlet)
        {
            m_inlets.push_back(inlet);
            if(inlet->getType() & Io::Type::Signal)
            {
                DspNode* node = dynamic_cast<DspNode *>(this);
                if(node)
                {
                    node->setNumberOfInlets(ulong(m_inlets.size()));
                }
            }
        }
    }
    
    void Object::addOutlet(Io::Type type, string const& description)
    {
        lock_guard<mutex> guard(m_mutex);
        sOutlet outlet = make_shared<Outlet>(type, description);
        if(outlet)
        {
            m_outlets.push_back(outlet);
            if(outlet->getType() & Io::Type::Signal)
            {
                DspNode* node = dynamic_cast<DspNode *>(this);
                if(node)
                {
                    node->setNumberOfOutlets(ulong(m_outlets.size()));
                }
            }
        }
    }
    
    void Object::removeInlet(const ulong index)
    {
        lock_guard<mutex> guard(m_mutex);
        if(index < ulong(m_inlets.size()))
        {
            
        }
    }
    
    void Object::removeOutlet(const ulong index)
    {
        lock_guard<mutex> guard(m_mutex);
        if(index < ulong(m_outlets.size()))
        {
            
        }
    }
    
    ulong Object::getNumberOfInlets() const noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        return (ulong)m_inlets.size();
    }
    
    vector<Object::scInlet> Object::getInlets() const noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        return vector<scInlet>(m_inlets.begin(), m_inlets.end());
    }
    
    vector<Object::sInlet> Object::getInlets() noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        return m_inlets;
    }
    
    Object::scInlet Object::getInlet(ulong index) const noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        if(index < m_inlets.size())
        {
            return m_inlets[(vector<sInlet>::size_type)index];
        }
        else
        {
            return nullptr;
        }
    }
    
    Object::sInlet Object::getInlet(ulong index) noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        if(index < m_inlets.size())
        {
            return m_inlets[(vector<sInlet>::size_type)index];
        }
        else
        {
            return nullptr;
        }
    }
    
    ulong Object::getDspInletIndex(ulong index) const throw(Error&)
    {
        if(index < m_inlets.size())
        {
            if(m_inlets[index]->getType() & Object::Io::Signal)
            {
                ulong dspindex = 0;
                for(ulong i = index; i; i--)
                {
                    if(m_inlets[i-1]->getType() & Object::Io::Signal)
                    {
                        dspindex++;
                    }
                }
                return dspindex;
            }
            else
            {
                throw Error("The inlet " + to_string(index) + " isn't a dsp inlet.");
            }
        }
        else
        {
            throw Error("The inlet " + to_string(index) + " index is out of ranges.");
        }
    }

    ulong Object::getNumberOfOutlets() const noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        return (ulong)m_outlets.size();
    }
    
    vector<Object::scOutlet> Object::getOutlets() const noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        return vector<Object::scOutlet>(m_outlets.begin(), m_outlets.end());
    }
    
    vector<Object::sOutlet> Object::getOutlets() noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        return m_outlets;
    }
    
    Object::scOutlet Object::getOutlet(ulong index) const noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        if(index < m_outlets.size())
        {
            return m_outlets[(vector<sOutlet>::size_type)index];
        }
        else
        {
            return nullptr;
        }
    }
    
    Object::sOutlet Object::getOutlet(ulong index) noexcept
    {
        lock_guard<mutex> guard(m_mutex);
        if(index < m_outlets.size())
        {
            return m_outlets[(vector<sOutlet>::size_type)index];
        }
        else
        {
            return nullptr;
        }
    }
    
    ulong Object::getDspOutletIndex(ulong index) const throw(Error&)
    {
        if(index < m_outlets.size())
        {
            if(m_outlets[index]->getType() & Object::Io::Signal)
            {
                ulong dspindex = 0;
                for(ulong i = index; i; i--)
                {
                    if(m_outlets[i-1]->getType() & Object::Io::Signal)
                    {
                        dspindex++;
                    }
                }
                return dspindex;
            }
            else
            {
                throw Error("The outlet " + to_string(index) + "isn't a dsp outlet.");
            }
        }
        else
        {
            throw Error("The outlet " + to_string(index) + " index is out of ranges.");
        }
    }
    
    sGuiController Object::createController()
    {
        return make_shared<Object::Controller>(getShared());
    }
}
















