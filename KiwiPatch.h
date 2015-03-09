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
    //                                      PAGE                                        //
    // ================================================================================ //
    
    //! The patcher manages objects and links.
    /**
     The patcher is... ??
     */
	class Patcher : public GuiPatcher, public DspChain
	{	
    private:
        const wInstance             m_instance;
        vector<sObject>             m_objects;
        vector<sLink>               m_links;
        vector<ulong>               m_free_ids;
        mutable mutex               m_mutex;
        
        //! @internal Object and link creation.
        void createObject(Dico const& dico);
        void createLink(Dico const& dico);
        
    public:
        //! Constructor.
        /** You should never call this method except if you really know what you're doing.
         */
        Patcher(sInstance instance);
        
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
         @param objects   A vector of atoms.
         */
        void getObjects(vector<sObject>& objects) const noexcept
        {
            lock_guard<mutex> guard(m_mutex);
            objects = m_objects;
        }
        
        //! Get the links.
        /** The function retrieves the links from the patcher.
         @param links   A vector of links.
         */
        void getLinks(vector<sLink>& links) const noexcept
        {
            lock_guard<mutex> guard(m_mutex);
            links = m_links;
        }
        
        //! Creation method of the patcher.
        /** The function evaluates the inputs for the creation and returns the result in the outputs.
         @param inputs The vector of atoms to evaluate.
         @param outputs The vector of atoms to return the creation.
         */
        void create(Vector const& inputs, Vector outputs);
        
        //! Remove method of the patcher.
        /** The function evaluates the inputs for the deletion.
         @param inputs The vector of atoms to evaluate.
         */
        void remove(Vector const& inputs);
        
        //! Get method of the patcher.
        /** The function evaluates the inputs for the getting and returns the result in the outputs.
         @param inputs The vector of atoms to evaluate.
         @param outputs The vector of atoms to return the getting.
         */
        void get(Vector const& inputs, Vector outputs) const;
        
        //! Set method of the patcher.
        /** The function evaluates the inputs for the setting.
         @param inputs The vector of atoms to evaluate.
         */
        void set(Vector const& inputs);
        
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
		
		//! Notify the manager that the values of an attribute has changed.
		/** The function notifies the manager that the values of an attribute has changed.
		 @param attr An attribute.
		 @return pass true to notify changes to listeners, false if you don't want them to be notified
		 */
		virtual bool notify(sAttr attr) override {return true;}
    };
}


#endif


