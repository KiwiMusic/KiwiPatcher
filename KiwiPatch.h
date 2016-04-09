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

#ifndef __DEF_KIWI_PATCH__
#define __DEF_KIWI_PATCH__

#include "KiwiLink.h"

namespace Kiwi
{
    // ================================================================================ //
    //                                      PATCHER                                     //
    // ================================================================================ //
    
    //! The patcher manages objects and links.
    /**
     The patcher is... ??
     */
    class Patcher : public GuiModel, public DspChain, public Attr::Manager
	{
    public:        
        class Controller;
        typedef shared_ptr<Controller>          sController;
        typedef weak_ptr<Controller>            wController;
        typedef shared_ptr<const Controller>    scController;
        typedef weak_ptr<const Controller>      wcController;
        
        class Listener;
        typedef shared_ptr<Listener>            sListener;
        typedef weak_ptr<Listener>              wListener;
        typedef shared_ptr<const Listener>      scListener;
        typedef weak_ptr<const Listener>        wcListener;
        
    private:
        const wInstance             m_instance;
        vector<sObject>             m_objects;
        vector<sLink>               m_links;
        vector<ulong>               m_free_ids;
        mutable mutex               m_mutex;
        ListenerSet<Listener>       m_listeners;

        //! @internal Object and link creation.
        void createObject(Dico& dico);
        void createLink(Dico const& dico);
        
    public:
        //! Constructor.
        /** You should never call this method except if you really know what you're doing.
         */
        Patcher(sInstance instance) noexcept;
        
        //! Destructor.
        /** You should never call this method except if you really know what you're doing.
         */
        ~Patcher();
        
        //! The patcher creation method.
        /** The function allocates a patcher and initialize the defaults objects.
         @param instance The instance that will manage the patcher.
         @param dico The dico that will initialize the patcher.
         @return The patcher.
         */
        static sPatcher create(sInstance instance, Dico& dico);
        
        //! Add a patcher's listener.
        /** The function adds a patcher's listener.
         If the listener was already listening the patcher, the function has no effect.
         @param listener The listener to add.
         */
        void addListener(sListener listener) noexcept
        {
            m_listeners.add(listener);
        }
        
        //! Remove a patcher's listener.
        /** The function removes a patcher's listener.
         If the listener wasn't listening the patcher, the function has no effect.
         @param listener The listener to add.
         */
        void removeListener(sListener listener) noexcept
        {
            m_listeners.remove(listener);
        }
		
        //! Retrieve the instance that manages the patcher.
        /** The function retrieves the instance that manages the patcher.
         @return The instance that manages the patcher.
         */
        inline sInstance getInstance() const noexcept
        {
            return m_instance.lock();
        }
            
        //! Retrieve the shared pointer of the patcher.
        /** The function retrieves the shared pointer of the patcher.
         @return The shared pointer of the patcher.
         */
        inline scPatcher getShared() const noexcept
        {
            return static_pointer_cast<const Patcher>(DspChain::shared_from_this());
        }
        
        //! Retrieve the shared pointer of the patcher.
        /** The function retrieves the shared pointer of the patcher.
         @return The shared pointer of the patcher.
         */
        inline sPatcher getShared() noexcept
        {
            return static_pointer_cast<Patcher>(DspChain::shared_from_this());
        }

        //! Get the objects.
        /** The function retrieves the objects from the patcher.
         @return A vector with the objects.
         */
        inline vector<sObject> getObjects() const noexcept
        {
            lock_guard<mutex> guard(m_mutex);
            return m_objects;
        }
        
        //! Get an object with the id.
        /** The function retrieves an object with an id.
         @param id   The id of the object.
         */
        inline sObject getObjectWithId(ulong const _id) const noexcept
        {
            lock_guard<mutex> guard(m_mutex);
            for(auto obj : m_objects)
            {
                if(obj->getId() == _id)
                {
                    return obj;
                }
            }
            return nullptr;
        }
        
        //! Get the links.
        /** The function retrieves the links from the patcher.
         @return A vector of links.
         */
        vector<sLink> getLinks() const noexcept
        {
            lock_guard<mutex> guard(m_mutex);
            return m_links;
        }
        
        //! Append a dico.
        /** The function reads a dico and add the objects and links to the patcher.
         @param dico The dico.
         */
        void add(Dico const& dico);
        
        //! Free a object.
        /** The function removes a object from the patcher.
         @param object        The pointer to the object.
         */
        void remove(sObject object);
        
        //! Free a link.
        /** The function removes a link from the patcher.
         @param link        The pointer to the link.
         */
        void remove(sLink link);
        
        //! Bring a object to the front of the patcher.
        /** The function brings a object to the front of the patcher. The object will be setted as if it was the last object created and will be the last object of the vector of objects.
         @param object        The pointer to the object.
         */
        void toFront(sObject object);
        
        //! Bring a object to the back of the patcher.
        /** The function brings a object to the back of the patcher. The object will be setted as if it was the first object created and will be the first object of the vector of objects.
         @param object        The pointer to the object.
         */
        void toBack(sObject object);
        
        //! Write the patcher in a dico.
        /** The function writes the patcher in a dico.
         @param dico The dico.
         */
        void write(Dico& dico) const;
        
        //! Retrieve the "gridsize" attribute value of the patcher.
        /** The function retrieves the "gridsize" attribute value of the patcher.
         @return The "gridsize" attribute value of the patcher.
         */
        inline long getGridSize() const noexcept
        {
            return getAttrValue<long>(Tags::gridsize);
        }
        
        //! Retrieve the "locked_bgcolor" attribute value of the patcher.
        /** The function retrieves the "locked_bgcolor" attribute value of the patcher.
         @return The "locked_bgcolor" attribute value of the patcher.
         */
        inline Color getLockedBackgroundColor() const noexcept
        {
            return getAttrValue<Color>(Tags::locked_bgcolor);
        }
        
        //! Retrieve the "locked_bgcolor" attribute value of the patcher.
        /** The function retrieves the "locked_bgcolor" attribute value of the patcher.
         @return The "locked_bgcolor" attribute value of the patcher.
         */
        inline Color getUnlockedBackgroundColor() const noexcept
        {
            return getAttrValue<Color>(Tags::unlocked_bgcolor);
        }
        
    private:
        
        //! Create the controller.
        /** The function creates a controller of the patcher.
         @return The controller.
         */
        sGuiController createController() override;
    };
    
    // ================================================================================ //
    //                                  PATCHER LISTENER                                //
    // ================================================================================ //
    
    //! The patcher listener is an abstract class that subclasses should inherit from to receive notifications.
    /**
     The patcher listener is an abstract class that subclasses should inherit from to receive notifications related to object and link creation and deletion
     */
    class Patcher::Listener
    {
    public:
        //! The destructor.
        virtual ~Listener() {}
        
        //! Receive the notification that an object has been created.
        /** The function is called by the patcher when an object has been created.
         @param object     The object.
         */
        virtual void objectCreated(sPatcher patcher, sObject object) = 0;
        
        //! Receive the notification that an object has been removed.
        /** The function is called by the patcher when an object has been removed.
         @param object     The object.
         */
        virtual void objectRemoved(sPatcher patcher, sObject object) = 0;
        
        //! Receive the notification that a link has been created.
        /** The function is called by the patcher when a link has been created.
         @param link     The link.
         */
        virtual void linkCreated(sPatcher patcher, sLink link) = 0;
        
        //! Receive the notification that a link has been removed.
        /** The function is called by the patcher when a link has been removed.
         @param link    The link.
         */
        virtual void linkRemoved(sPatcher patcher, sLink link) = 0;
    };
}


#endif


