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


#include "KiwiConsole.h"
#include "KiwiInstance.h"

namespace Kiwi
{
    // ================================================================================ //
    //                                      CONSOLE                                     //
    // ================================================================================ //

    void Console::addListener(sListener listener) noexcept
    {
        ListenerSet<Listener>& listeners(getListeners());
        listeners.add(listener);
    }
    
    void Console::removeListener(sListener listener) noexcept
    {
        ListenerSet<Listener>& listeners(getListeners());
        listeners.remove(listener);
    }
    
    void Console::post(string const& message) noexcept
    {
#if defined(DEBUG) || defined(NO_GUI)
        cout << message << endl;
#endif
        shared_ptr<const Message> mess = make_shared<Message>(nullptr, nullptr, nullptr, Message::Post, message);
        
        ListenerSet<Listener>& listeners(getListeners());
        listeners.call(&Listener::receive, mess);
    }
        
    void Console::post(scObject object, string const& message) noexcept
    {
#if defined(DEBUG) || defined(NO_GUI)
        cerr << object->getText() << " : " << message << endl;
#endif
        scInstance instance = nullptr;
        scPatcher  patcher  = nullptr;
        if(object)
        {
            instance= object->getInstance();
            patcher = object->getPatcher();
        }
        shared_ptr<const Message> mess = make_shared<Message>(instance, patcher, object, Message::Post, message);
        
        ListenerSet<Listener>& listeners(getListeners());
        listeners.call(&Listener::receive, mess);
    }
        
    void Console::warning(string const& message) noexcept
    {
#if defined(DEBUG) || defined(NO_GUI)
        cerr << "warning : " << message << endl;
#endif
        shared_ptr<const Message> mess = make_shared<Message>(nullptr, nullptr, nullptr, Message::Warning, message);
        
        ListenerSet<Listener>& listeners(getListeners());
        listeners.call(&Listener::receive, mess);
    }
        
    void Console::warning(scObject object, string const& message) noexcept
    {
#if defined(DEBUG) || defined(NO_GUI)
        cerr << "warning : " << object->getName()->getName() << " : " << message << endl;
#endif
        shared_ptr<const Kiwi::Instance> instance = nullptr;
        shared_ptr<const Kiwi::Patcher> patcher = nullptr;
        if(object)
        {
            instance    = object->getInstance();
            patcher     = object->getPatcher();
        }
        shared_ptr<const Message> mess = make_shared<Message>(instance, patcher, object, Message::Warning, message);
        
        ListenerSet<Listener>& listeners(getListeners());
        listeners.call(&Listener::receive, mess);
    }
        
    void Console::error(string const& message) noexcept
    {
#if defined(DEBUG) || defined(NO_GUI)
        cerr << "error : " << message << endl;
#endif
        shared_ptr<const Message> mess = make_shared<Message>(nullptr, nullptr, nullptr, Message::Error, message);
        
        ListenerSet<Listener>& listeners(getListeners());
        listeners.call(&Listener::receive, mess);
    }
        
    void Console::error(scObject object, string const& message) noexcept
    {
#if defined(DEBUG) || defined(NO_GUI)
        cerr << "error : " << object->getName()->getName() << " : " << message << endl;
#endif
        shared_ptr<const Kiwi::Instance> instance = nullptr;
        shared_ptr<const Kiwi::Patcher> patcher = nullptr;
        if(object)
        {
            instance    = object->getInstance();
            patcher        = object->getPatcher();
        }
        shared_ptr<const Message> mess = make_shared<Message>(instance, patcher, object, Message::Error, message);
        
        ListenerSet<Listener>& listeners(getListeners());
        listeners.call(&Listener::receive, mess);
    }
    
}









