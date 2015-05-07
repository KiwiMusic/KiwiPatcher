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

#ifndef __DEF_KIWI_OBJECT_CONTROLLER__
#define __DEF_KIWI_OBJECT_CONTROLLER__

#include "KiwiObject.h"

namespace Kiwi
{
    // ================================================================================ //
    //                              OBJECT CONTROLLER                                   //
    // ================================================================================ //
    
    class Object::Controller : public GuiController
    {
    private:
        const sObject   m_object;
        ulong           m_zoom;
        bool            m_locked;
        bool            m_presentation;
        bool            m_display_grid;
        bool            m_snap_to_grid;
        
    public:
        
        //! The object controller constructor.
        /** The function does nothing.
         @param object The object.
         */
        Controller(sObject object) noexcept;
        
        //! The controller destructor.
        /** The function does nothing.
         */
        ~Controller() noexcept;
        
        // ================================================================================ //
        //										PRESENTATION                                //
        // ================================================================================ //
        
        //! Retrieve the zoom of the object.
        /** The function retrieves the zoom of the object.
         @return the zoom of the object in percent.
         @see setZoom
         */
        inline ulong getZoom() const noexcept
        {
            return m_zoom;
        }
        
        //! Set the zoom of the object.
        /** The function sets the zoom of the object.
         @param zoom The zoom of the object in percent.
         @see getZoom
         */
        void setZoom(const ulong zoom);
        
        //! Retrieve if the object is locked or unlocked.
        /** The function retrieves if the object is locked or unlocked.
         @return True if the object is locked, false if it is unlocked.
         @see setLockStatus
         */
        inline bool getLockStatus() const noexcept
        {
            return m_locked;
        }
        
        //! Lock or unlock the object.
        /** The function locks or unlocks the object.
         @param locked True to lock the object, false to unlock it.
         @see getLockStatus
         */
        void setLockStatus(const bool locked);
        
    protected:
        
        //! The paint method that can be override.
        /** The function shoulds draw some stuff in the sketch.
         @param view    The view that ask to draw.
         @param sketch  A sketch to draw.
         */
        void draw(sGuiView view, Sketch& sketch) override;
    };
}


#endif


