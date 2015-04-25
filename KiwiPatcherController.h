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

#ifndef __DEF_KIWI_PATCHER_CONTROLLER__
#define __DEF_KIWI_PATCHER_CONTROLLER__

#include "KiwiPatch.h"

namespace Kiwi
{
    // ================================================================================ //
    //                              PATCHER CONTROLLER                                  //
    // ================================================================================ //

    class Patcher::Controller : public GuiController, public enable_shared_from_this<Patcher::Controller>
    {
    private:
        const sPatcher  m_patcher;
        ulong           m_zoom;
        bool            m_locked;
        bool            m_presentation;
        bool            m_display_grid;
        bool            m_snap_to_grid;
        
    public:
        
        //! The patcher controller constructor.
        /** The function does nothing.
         @param patcher The patcher.
         */
        Controller(sPatcher patcher) noexcept;
        
        //! The controller destructor.
        /** The function does nothing.
         */
        ~Controller() noexcept;
        
        // ================================================================================ //
        //										BEHAVIOR                                    //
        // ================================================================================ //
        
        //! Retrieve the position of the sketcher.
        /** The function retrieves the position of the sketcher.
         @return The position of the sketcher.
         */
        virtual Point getPosition() const noexcept
        {
            return Point(0., 0.);
        }
        
        //! Retrieve the size of the sketcher.
        /** The function retrieves the size of the sketcher.
         @return The size of the sketcher.
         */
        virtual Size getSize() const noexcept
        {
            return m_patcher->getSize();
        }
        
        //! Receives if the controller wants the mouse.
        /** This function retrieves if the controller wants the mouse.
         @return true if the controller wants the mouse, othrewise false.
         */
        inline bool wantMouse() const noexcept override
        {
            return true;
        }
        
        //! Receives if the controller wants the keyboard.
        /** This function retrieves if the controller wants the keyboard.
         @return true if the controller wants the keyboard, othrewise false.
         */
        inline bool wantKeyboard() const noexcept override
        {
            return true;
        }
        
        //! Receives if the controller wants actions.
        /** This function retrieves if the controller wants the actions.
         @return true if the controller wants the actions, othrewise false.
         */
        inline bool wantActions() const noexcept override
        {
            return true;
        }
        
        // ================================================================================ //
        //										PRESENTATION                                //
        // ================================================================================ //
        
        //! Retrieve the zoom of the patcher.
        /** The function retrieves the zoom of the patcher.
         @return the zoom of the patcher in percent.
         @see setZoom
         */
        inline ulong getZoom() const noexcept
        {
            return m_zoom;
        }
        
        //! Set the zoom of the patcher.
        /** The function sets the zoom of the patcher.
         @param zoom The zoom of the patcher in percent.
         @see getZoom
         */
        void setZoom(const ulong zoom);
        
        //! Retrieve if the patcher is locked or unlocked.
        /** The function retrieves if the patcher is locked or unlocked.
         @return True if the patcher is locked, false if it is unlocked.
         @see setLockStatus
         */
        inline bool getLockStatus() const noexcept
        {
            return m_locked;
        }
        
        //! Lock or unlock the patcher.
        /** The function locks or unlocks the patcher.
         @param locked True to lock the patcher, false to unlock it.
         @see getLockStatus
         */
        void setLockStatus(const bool locked);
        
    protected:
        
        //! The paint method that can be override.
        /** The function shoulds draw some stuff in the sketch.
         @param sketch  A sketch to draw.
         */
        void draw(Sketch& sketch) override;
        
        //! The mouse receive method.
        /** The function pass the mouse event to the sketcher if it inherits from mouser.
         @param event    A mouser event.
         @return true if the class has done something with the event otherwise false
         */
        bool receive(MouseEvent const& event) override;
        
        //! The keyboard receive method.
        /** The function pass the keyboard event to the sketcher if it inherits from keyboarder.
         @param event    A keyboard event.
         @return true if the class has done something with the event otherwise false
         */
        bool receive(KeyboardEvent const& event) override;
        
        //! The keyboard focus receive method.
        /** The function pass the keyboard event to the sketcher if it inherits from keyboarder.
         @param event    A focus event.
         @return true if the class has done something with the event otherwise false
         */
        bool receive(KeyboardFocus const event) override;
        
        //! Retrieves the action codes.
        /** The function retreives the action codes from the the manager.
         @return The action codes.
         */
        vector<Action::Code> getActionCodes() override;
        
        //! Retrieves an action from the manager.
        /** The function retreives an action from the the manager.
         @param code The code of the action.
         @return The action.
         */
        Action getAction(const ulong code) override;
        
        //! Performs an action.
        /** The function performs an action.
         @param code The code of the action.
         @return true if the action has been performed, otherwise false.
         */
        bool performAction(const ulong code) override;
        
    private:
        
        //@internal
        bool mouseDown(MouseEvent const& event);
        
        //@internal
        void createObject(string const& name, Point const& position);
    };
}


#endif


