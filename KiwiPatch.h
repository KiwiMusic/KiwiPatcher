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
    class Patcher : public GuiSketcher, public DspChain, public Attr::Manager
	{
    public:
        class Controller;
        typedef shared_ptr<Controller>          sController;
        typedef weak_ptr<Controller>            wController;
        typedef shared_ptr<const Controller>    scController;
        typedef weak_ptr<const Controller>      wcController;
        
        class Window;
        typedef shared_ptr<Window>              sWindow;
        typedef weak_ptr<Window>                wWindow;
        typedef shared_ptr<const Window>        scWindow;
        typedef weak_ptr<const Window>          wcWindow;
        
        class Lasso;
        typedef shared_ptr<Lasso>               sLasso;
        typedef weak_ptr<Lasso>                 wLasso;
        typedef shared_ptr<const Lasso>         scLasso;
        typedef weak_ptr<const Lasso>           wcLasso;
        
        
    private:
        const wInstance             m_instance;
        vector<sObject>             m_objects;
        vector<sLink>               m_links;
        vector<ulong>               m_free_ids;
        mutable mutex               m_mutex;
        
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
            for(vector<sObject>::size_type i = 0; i < m_objects.size(); i++)
            {
                if(m_objects[i]->getId() == _id)
                {
                    return m_objects[i];
                }
            }
            return nullptr;
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
        
        //! The draw method that should be override.
        /** The function shoulds draw some stuff in the sketch.
         @param sketch A sketch to draw.
         */
        void draw(scGuiView view, Sketch& sketch) const override;
        
        //! Create a new window for the patcher.
        /** The function creates a new window for the patcher.
         @return The window.
         */
        sGuiWindow createWindow();
        
        //! Retrieves the position of the sketcher.
        /** The function retrieves the position of the sketcher.
         @return The position of the sketcher.
         */
        inline Point getPosition() const noexcept
        {
            return getAttrValue<Point>(Tags::position);
        }
        
        //! Retrieves the size of the sketcher.
        /** The function retrieves the size of the sketcher.
         @return The size of the sketcher.
         */
        inline Size getSize() const noexcept
        {
            return getAttrValue<Size>(Tags::size);
        }
        
        //! Retrieves the bounds of the sketcher.
        /** The function retrieves the bounds of the sketcher.
         @return The bounds of the sketcher.
         */
        inline Rectangle getBounds() const noexcept
        {
            return Rectangle(getPosition(), getSize());
        }
        
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
        
        sLasso createLasso();
        
        void removeLasso(sLasso lasso);
        
    private:
        
        //! Create the controller.
        /** The function creates a controller of the patcher.
         @return The controller.
         */
        sGuiController createController() override;
    };
    
    class Patcher::Lasso : public GuiSketcher
    {
    private:
        bool						dragging;
        set<wObject,
        owner_less<wObject>>        objects;
        set<wLink,
        owner_less<wLink>>          links;
        
    public:
        
        Lasso(sGuiContext context) noexcept : GuiSketcher(context)
        {
            ;
        }
        
        virtual ~Lasso() noexcept
        {
            objects.clear();
            links.clear();
        }
        
        //! The draw method that should be override.
        /** The function shoulds draw some stuff in the sketch.
         @param ctrl    The controller that ask the draw.
         @param sketch  A sketch to draw.
         */
        void draw(scGuiView view, Sketch& sketch) const override;
    };
}


#endif


