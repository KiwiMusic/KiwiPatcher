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

#ifndef __DEF_KIWI_CONSOLE__
#define __DEF_KIWI_CONSOLE__

#include "../KiwiCore/KiwiCore.h"

namespace Kiwi
{
    class Object;
    typedef shared_ptr<Object>          sObject;
    typedef weak_ptr<Object>            wObject;
    typedef shared_ptr<const Object>    scObject;
    typedef weak_ptr<const Object>      wcObject;
    
    class Link;
    typedef shared_ptr<Link>            sLink;
    typedef shared_ptr<const Link>      scLink;
    typedef weak_ptr<Link>              wLink;
    typedef weak_ptr<const Link>        wcLink;
    
    class Patcher;
    typedef shared_ptr<Patcher>        sPatcher;
    typedef weak_ptr<Patcher>          wPatcher;
    typedef shared_ptr<const Patcher>  scPatcher;
    typedef weak_ptr<const Patcher>    wcPatcher;
    
    class Instance;
    typedef shared_ptr<Instance>        sInstance;
    typedef weak_ptr<Instance>          wInstance;
    typedef shared_ptr<const Instance>  scInstance;
    typedef weak_ptr<const Instance>    wcInstance;
    
    // ================================================================================ //
    //                                      CONSOLE                                     //
    // ================================================================================ //
    
    //! The console is an interface that receives messages and dispatches them to console listeners.
    /**
     The console is created with a kiwi instance then retrieve it and bind a console listener to get the posts, warnings and errors.
     @see Console::Message
     @see Console::Listener
     */
    class Console : public enable_shared_from_this<Console>
    {
    public:
        class Listener;
        typedef shared_ptr<Listener>        sListener;
        typedef weak_ptr<Listener>          wListener;
        typedef shared_ptr<const Listener>  scListener;
        typedef weak_ptr<const Listener>    wcListener;
        
        class Message;
            
    private:
        
        static inline ListenerSet<Listener>& getListeners() noexcept
        {
            static ListenerSet<Listener> _listeners;
            return _listeners;
        }
        
        static inline vector<Message>& getMessages() noexcept
        {
            static vector<Message> _messages;
            return _messages;
        }
        
        static inline mutex& getMutex() noexcept
        {
            static mutex _mutex;
            return _mutex;
        }
    public:
        
        //! Adds an console listener in the binding list of the console.
        /** The function adds an console listener in the binding list of the console.
         @param listener The listener.
         @see unbind
         */
        void addListener(sListener listener) noexcept;
        
        //! Removed an console listener from the binding list of the console.
        /** The function removes an console listener from the binding list of the console.
         @param listener  The listener.
         @see bind
         */
        void removeListener(sListener listener) noexcept;
        
        //! Post a standard message.
        /** The function post a message and notify the console listeners that a message has been received.
         @param message  The message in the string format.
         */
        static void post(string const& message) noexcept;
        
        //! Post a standard message that refers to a specific object.
        /** The function post a message and notify the console listeners that a message has been received from this object.
         @param object  The object sender.
         @param message The message in the string format.
         */
        static void post(const scObject object, string const& message) noexcept;
        
        //! Post a warning.
        /** The function post a warning and notify the console listeners that a warning has been received.
         @param message  The warning in the string format.
         */
        static void warning(string const& message) noexcept;
        
        //! Post a warning that refers to a specific object.
        /** The function post a warning and notify the console listeners that a warning has been received from this object.
         @param object  The object sender.
         @param message The warning in the string format.
         */
        static void warning(const scObject object, string const& message) noexcept;
        
        //! Post an error.
        /** The function post a error and notify the console listeners that a error has been received.
         @param message  The error in the string format.
         */
        static void error(string const& message) noexcept;
        
        //! Post an error that refers to a specific object.
        /** The function post a error and notify the console listeners that a error has been received from this object.
         @param object  The object sender.
         @param message The error in the string format.
         */
        static void error(const scObject object, string const& message) noexcept;
    };
    
    // ================================================================================ //
    //                                  INSTANCE LISTENER                               //
    // ================================================================================ //
    
    //! The console listener is a virtual class that can bind itself to a console and be notified of the sevreal messages.
    /**
     The console listener is a very light class with three methods that can receive the post, warning and error messages notifications from consoles.
     @see Console
     @see Console::Message
     */
    class Console::Listener
    {
    public:
        //! The destructor.
        /** The destructor does nothing.
         */
        virtual inline ~Listener() {}
        
        //! Receive the messages.
        /** The function is called by the console when a message has been received.
         @param console The console that received the message.
         @param message The message.
         */
        virtual void receive(shared_ptr<const Message> message) = 0;
    };
    
    // ================================================================================ //
    //                                  CONSOLE MESSAGE                                 //
    // ================================================================================ //
    
    //! The console message owns all the informations of a message posted via a console.
    /**
     The console message is used to send the content, the type and the sender of a message posted via a console to the console listeners.
     @see Console
     @see Console::Listener
     @see Console::History
     */
    class Console::Message
    {
    public:
        enum Kind
        {
            Empty   = 0,
            Post    = 1,
            Warning = 2,
            Error   = 3
        };
        
        const string        content;
        const Kind          kind;
        const wcObject      object;
        const wcPatcher     patcher;
        const wcInstance    instance;
        
        //! The constructor.
        /** The constructor initialize the members.
         */
        Message(scInstance instance, scPatcher patcher, scObject object, Kind kind, string const& content) noexcept :
        content(content), kind(kind), object(object), patcher(patcher), instance(instance)
        {
            ;
        }
        
        //! The destructor.
        /** The destructor free the members.
         */
        ~Message()
        {
            ;
        }
    };
    
    typedef shared_ptr<const Console::Message> scConsoleMessage;

};


#endif


