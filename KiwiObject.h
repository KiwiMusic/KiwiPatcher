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

#ifndef __DEF_KIWI_OBJECT__
#define __DEF_KIWI_OBJECT__

#include "../KiwiDsp/KiwiDsp.h"
#include "../KiwiGui/KiwiGui.h"

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
    
    namespace ActionCategories
    {
        static const char* const general       = "General";
        static const char* const editing       = "Editing";
        static const char* const view          = "View";
        static const char* const windows       = "Windows";
    }
    
    enum ActionCodes : Action::Code
    {
        newPatcher					= 0x200010,		/**< Create a new blank patcher window. */
        newTabPatcher				= 0x200020,		/**< Create a new blank tab window. */
        openFile					= 0x200030,		/**< Open a file in a new window. */
        closePatcher				= 0x200051,		/**< Close the current patcher. */
        save						= 0x200060,		/**< Save the current patcher or document. */
        saveAs						= 0x200061,		/**< Save the current patcher or document as. */
        
        minimizeWindow				= 0x201010,		/**< Reduce the current window. */
        maximizeWindow				= 0x201020,		/**< Maximise the  current window. */
        closeWindow					= 0x201031,		/**< Close the current window. */
        closeAllPatchers			= 0x201040,		/**< Close all patcher window. */
        
        showConsoleWindow			= 0x202000,		/**< Make visible the "console" window. */
        showAudioStatusWindow		= 0x202010,		/**< Make visible the "audio status" window. */
        showAboutAppWindow			= 0x202020,		/**< Make visible the "about app" window. */
        showAppSettingsWindow		= 0x202030,		/**< Make visible the "application settings" window. */
        
        duplicate					= 0xf1000a,		/**< Duplicate selected objects of the patcher and paste them on it. */
        pasteReplace				= 0xf1000b,		/**< Duplicate selected objects of the patcher and paste them on it. */
        
        toFront						= 0xf2000a,		/**< Move selected object ahead of all other objects. */
        toBack						= 0xf2000b,		/**< Move selected object behind all other objects. */
        addToPresentation			= 0xf2000c,		/**< Add selected objects to presentation. */
        removeFromPresentation		= 0xf2000d,		/**< Remove selected objects from presentation. */
        
        zoomIn						= 0xf20013,		/**< Magnify the patcher view of almost 10%. */
        zoomOut						= 0xf20014,		/**< Reduce the patcher view of almost 10%. */
        zoomNormal					= 0xf20015,		/**< Restore the patcher view zoom to 100%. */
        
        editModeSwitch				= 0xf20100,		/**< Toggle Lock/Unlock patcher view. */
        presentationModeSwitch		= 0xf20101,		/**< Toggle Presentation patcher view mode. */
        
        gridModeSwitch				= 0xf20200,		/**< Toggle grid patcher mode. */
        enableSnapToGrid			= 0xf20201,		/**< Toggle snap to grid patcher mode. */
        
        newObject					= 0xf30300,		/**< Add a new "object" to the patcher. */
        newMessage					= 0xf30301,		/**< Add a new "message" object object to the patcher. */
        newFlonum					= 0xf30302,		/**< Add a new "flonum" object object to the patcher. */
        newNumber					= 0xf30303,		/**< Add a new "number" object object to the patcher. */
        newComment					= 0xf30304,		/**< Add a new "comment" object object to the patcher. */
        newBang						= 0xf30305,		/**< Add a new "button" object object to the patcher. */
        newToggle					= 0xf30306,		/**< Add a new "toggle" object object to the patcher. */
        
        showPatcherInspector		= 0xf20400,		/**< Shows the patcher properties inspector. */
        
        showObjectInspector			= 0xf20500,		/**< Shows the selected objects properties inspector. */
        openObjectHelp				= 0xf20510		/**< Open selected object help patcher. */
    };
    
    struct Infos
    {
        const sInstance         instance;
        const sPatcher          patcher;
        const ulong             lid;
        const sTag              name;
        const string            text;
        const Dico				dico;
        const Vector			args;
        
        Infos() noexcept :
        instance(nullptr), patcher(nullptr), lid(0), name(Tag::create("")), text(""), dico(), args({})
        {
            ;
        }
        
        Infos(sInstance _instance, sPatcher _patcher, const ulong _id, sTag _name, const string _text, Dico const& _dico, Vector const& _args) :
        instance(_instance), patcher(_patcher), lid(_id), name(_name), text(_text), dico(_dico), args(_args)
        {
            ;
        }
    };
    
    // ================================================================================ //
    //                                      OBJECT                                      //
    // ================================================================================ //
    
    //! The object is a graphical object.
    /**
     The object is a graphical class that aims to be instantiate in a patcher.
     */
    class Object : virtual public Beacon::Castaway, public GuiSketcher, virtual public Attr::Manager
    {
    public:
        
        struct Io
        {
            enum Type
            {
                Message = 0,
                Signal  = 1,
                Both    = 2
            };
            
            enum Polarity : bool
            {
                Cold   = 0,
                Hot    = 1
            };
        };
        
    
        class Iolet;
        
        class Inlet;
        typedef shared_ptr<Inlet>       sInlet;
        typedef weak_ptr<Inlet>         wInlet;
        typedef shared_ptr<const Inlet> scInlet;
        typedef weak_ptr<const Inlet>   wcInlet;
        
        class Outlet;
        typedef shared_ptr<Outlet>      sOutlet;
        typedef weak_ptr<Outlet>        wOutlet;
        typedef shared_ptr<const Outlet>scOutlet;
        typedef weak_ptr<const Outlet>  wcOutlet;
        
        class Controller;
        typedef shared_ptr<Controller>          sController;
        typedef weak_ptr<Controller>            wController;
        typedef shared_ptr<const Controller>    scController;
        typedef weak_ptr<const Controller>      wcController;
    
    private:
        struct Connection
        {
            wObject object;
            ulong index;
        };
        
        const wInstance         m_instance;
        const wPatcher          m_patcher;
        const sTag				m_name;
        const string            m_text;
        const ulong             m_id;
        
        vector<sOutlet>			m_outlets;
        vector<sInlet>			m_inlets;
        atomic_ullong			m_stack_count;
        mutable mutex			m_mutex;
        
    public:
        
        //! Constructor.
        /** You should never call this method except if you really know what you're doing.
         */
        Object(Infos const& detail, sTag name) noexcept;
        
        //! Destructor.
        /** You should never call this method except if you really know what you're doing.
         */
        virtual ~Object() noexcept;
        
    protected:
        
        //! Retrieve the shared pointer of the attribute manager.
        /** The function retrieves the shared pointer of the attribute manager.
         @return The shared pointer of the attribute manager.
         */
        sObject getShared() noexcept
        {
            return dynamic_pointer_cast<Object>(Attr::Manager::shared_from_this());
        }
        
        //! Retrieve the shared pointer of the attribute manager.
        /** The function retrieves the shared pointer of the attribute manager.
         @return The shared pointer of the attribute manager.
         */
        scObject getShared() const noexcept
        {
            return dynamic_pointer_cast<const Object>(Attr::Manager::shared_from_this());
        }
        
    public:
        
        //! Retrieve the instance that manages the patcher of the object.
        /** The function retrieves the instance that manages the patcher of the object.
         @return The instance that manages the patcher of the object.
         */
        inline sInstance getInstance() const noexcept
        {
            return m_instance.lock();
        }
        
        //! Retrieve the patcher that manages the object.
        /** The function retrieves the patcher that manages the object.
         @return The patcher that manages the object.
         */
        inline sPatcher getPatcher() const noexcept
        {
            return m_patcher.lock();
        }
        
        //! Retrieve the name of the object.
        /** The function retrieves the name of the object as a tag.
         @return The name of the object as a tag.
         */
        inline sTag getName() const noexcept
        {
            return m_name;
        }
        
        //! Retrieve the text of the object.
        /** The function retrieves the text of the object.
         @return The text of the object.
         */
        inline string getText() const noexcept
        {
            return m_text;
        }
        
        //! Retrieve the id of the object.
        /** The function retrieves the id of the object.
         @return The id of the object.
         */
        inline ulong getId() const noexcept
        {
            return m_id;
        }
        
        //! Retrieve the expression of the object.
        /** The function retrieves the expression of the object as a string.
         @return The expression of the object as a string.
         */
        virtual string getExpression() const noexcept
        {
            return "error";
        }
        
        //! Retrieve the number of inlets of the object.
        /** The functions retrieves the number of inlets of the object.
         @return The number of inlets.
         */
        ulong getNumberOfInlets() const noexcept;
        
        //! Retrieve the inlets of the object.
        /** The functions retrieves the inlets of the object.
         @return The inlets.
         */
        vector<scInlet> getInlets() const noexcept;
        
        //! Retrieve the inlets of the object.
        /** The functions retrieves the inlets of the object.
         @return The inlets.
         */
        vector<sInlet> getInlets() noexcept;
        
        //! Retrieve an inlet.
        /** The functions retrieves an inlet.
         @param index The inlet's index.
         @return The inlet.
         */
        scInlet getInlet(ulong index) const noexcept;
        
        //! Retrieve an inlet.
        /** The functions retrieves an inlet.
         @param index The inlet's index.
         @return The inlet.
         */
        sInlet getInlet(ulong index) noexcept;
        
        //! Retrieve the dsp index of an inlet.
        /** The functions retrieves the dsp index of an inlet.
         @param index The inlet's index.
         @return The dsp index of the inlet.
         */
        ulong getDspInletIndex(ulong index) const throw(Error&);
        
        //! Retrieve the number of outlets of the object.
        /** The functions retrieves the number of outlets of the object.
         @return The number of outlets.
         */
        ulong getNumberOfOutlets() const noexcept;
        
        //! Retrieve the outlets of the object.
        /** The functions retrieves the outlets of the object.
         @return The outlets.
         */
        vector<scOutlet> getOutlets() const noexcept;
        
        //! Retrieve the outlets of the object.
        /** The functions retrieves the outlets of the object.
         @return The outlets.
         */
        vector<sOutlet> getOutlets() noexcept;
        
        //! Retrieve an outlet.
        /** The functions retrieves an outlet.
         @param index The outlet's index.
         @return The outlet.
         */
        scOutlet getOutlet(ulong index) const noexcept;
        
        //! Retrieve an outlet.
        /** The functions retrieves an outlet.
         @param index The outlet's index.
         @return The outlet.
         */
        sOutlet getOutlet(ulong index) noexcept;
        
        //! Retrieve the dsp index of an outlet.
        /** The functions retrieves the dsp index of an outlet.
         @param index The outlet's index.
         @return The dsp index of the outlet.
         */
        ulong getDspOutletIndex(ulong index) const throw(Error&);
        
        //! The receive method that should be override.
        /** The function shoulds perform some stuff.
         @param atoms    A list of atoms to pass.
         */
        virtual void receive(ulong index, Vector const& atoms) = 0;
        
        //! Write the object in a dico.
        /** The function writes the object in a dico.
         @param dico The dico.
         */
        void write(Dico& dico) const;
        
    protected:
        
        //! Send a vector of atoms via an outlet.
        /** The function sends a vector of atoms via an outlet and dispatches it to all the connected inlets.
         @param index The index of the outlet.
         @param atoms A list of atoms to pass.
         */
        void    send(const ulong index, Vector const& atoms) const noexcept;
        
        //! Add a new inlet to the object.
        /** The function adds a new inlet to the object.
         @param type The type of the inlet.
         @param description The description of the inlet.
         */
        void    addInlet(Io::Type type, Io::Polarity polarity, string const& description = "");
        
        //! Add a new outlet the the object.
        /** The function adds a new outlet the the object.
         @param type The type of the outlet.
         @param description The description of the outlet.
         */
        void    addOutlet(Io::Type type, string const& description = "");
        
        //! Remove an inlet from the object.
        /** The function removes an inlet from the object.
         @param index The index of the inlet.
         */
        void    removeInlet(const ulong index);
        
        //! Remove an outlet from the object.
        /** The function removes an outlet from the object.
         @param index The outlet of the inlet.
         */
        void    removeOutlet(const ulong index);

    private:
        
        //! The write method that should be override.
        /** The function writes the object in a dico.
         @param dico The dico.
         */
		virtual void save(Dico& dico) const {};
		
        //! The read method that should be override.
        /** The function reads a dico to initalize the objecte.
         @param dico The dico.
         */
		virtual void load(Dico const& dico) {};
        
        //! Create the controller.
        /** The function creates a controller depending on the inheritance.
         @return The controller.
         */
        sGuiController createController() override;
        
    public:
        
        //! Retrieves if the box should be hidden when the patcher is locked.
        /** The function retrieves if the box should be hidden when the patcher is locked.
         @return True if the box should be hidden when the patcher is locked, false otherwise.
         */
        inline bool isHiddenOnLock() const noexcept
        {
            return getAttrTyped<BoolValue>("hidden")->getValue();
        }
        
        //! Retrieve if the box should be displayed in presentation.
        /** The function retrieves if the box should be displayed in presentation.
         @return True if the box should be displayed in presentation, otherwise false.
         */
        inline bool isIncludeInPresentation() const noexcept
        {
            return getAttrTyped<BoolValue>("presentation")->getValue();
        }
        
        //! Retrieve the "ignoreclick" attribute value of the box.
        /** The function retrieves the "ignoreclick" attribute value of the box.
         @return The "ignoreclick" attribute value of the box.
         */
        inline bool getIgnoreClick() const noexcept
        {
            return getAttrTyped<BoolValue>("ignoreclick")->getValue();
        }
        
        //! Retrieve the position of the object.
        /** The function retrieves the position of the object.
         @return The position of the object.
         */
        inline Point getPosition() const noexcept
        {
            return getAttrTyped<PointValue>("position")->getValue();
        }
        
        //! Retrieve the size of the object.
        /** The function retrieves the size of the object.
         @return The size of the object.
         */
        inline Size getSize() const noexcept
        {
            return getAttrTyped<SizeValue>("size")->getValue();
        }
        
        //! Retrieve the bounds of the object.
        /** The function retrieves the bounds of the object.
         @return The bounds of the object.
         */
        inline Rectangle getBounds() const noexcept
        {
            return Rectangle(getAttrTyped<PointValue>("position")->getValue(), getAttrTyped<SizeValue>("size")->getValue());
        }
        
        //! Retrieve the position of the box when the patcherview is in presentation mode.
        /** The function retrieves the position of the box when the patcherview is in presentation mode.
         @return The position of the box when the patcherview is in presentation mode.
         */
        inline Point getPresentationPosition() const noexcept
        {
            return getAttrTyped<PointValue>("presentation_position")->getValue();
        }
        
        //! Retrieve the size of the box when the patcherview is in presentation mode.
        /** The function retrieves the size of the box when the patcherview is in presentation mode.
         @return The size of the box when the patcherview is in presentation mode.
         */
        inline Size getPresentationSize() const noexcept
        {
            return getAttrTyped<SizeValue>("presentation_size")->getValue();
        }
        
        //! Retrieve the bounds of the box when the patcherview is in presentation mode.
        /** The function retrieves the bounds of the box when the patcherview is in presentation mode.
         @return The bounds of the box when the patcherview is in presentation mode.
         */
        inline Rectangle getPresentationBounds() const noexcept
        {
            return Rectangle(getAttrTyped<PointValue>("presentation_position")->getValue(), getAttrTyped<SizeValue>("presentation_size")->getValue());
        }
    };
    
    //! The outlet owns a set of links.
    /**
     The outlet owns a set of links that are used to manage links in a object. It also have a type and a description.
     */
    class Object::Iolet : public enable_shared_from_this<Iolet>
    {
    protected:
        vector<Connection>  m_connections;
        const Io::Type      m_type;
        const Io::Polarity  m_polarity;
        const string        m_description;
        mutable mutex       m_mutex;
        
    public:
        //! Check if a connection is in the iolet.
        /** The functions checks if a connection is in the iolet.
         @param object The object.
         @param index the iolet's index.
         @return true if the connection is in the iolet, otherwise false.
         */
        bool has(sObject object, ulong index) const noexcept;
        
        //! Append a new connection to the iolet.
        /** The functions appends a new connection to the iolet.
         @param object The object.
         @param index the iolet's index.
         @return true if the connection has been added, otherwise false.
         */
        bool append(sObject object, ulong index) noexcept;
        
        //! Remove a connection from the iolet.
        /** The functions removes a connection from the iolet.
         @param object The object.
         @param index the iolet's index.
         @return true if the connection has been removed, otherwise false.
         */
        bool erase(sObject object, ulong index) noexcept;
    
        //! Constructor.
        /** You should never call this method except if you really know what you're doing.
         */
        Iolet(Io::Type type, Io::Polarity polarity, string const& description) noexcept :
        m_type(type),
        m_polarity(polarity),
        m_description(description)
        {
            ;
        }
        
        //! Destructor.
        /** You should never call this method except if you really know what you're doing.
         */
        ~Iolet()
        {
            m_connections.clear();
        }
        
        //! Retrieve the type of the iolet.
        /** The functions retrieves the type of the iolet.
         @return The type of the iolet.
         */
        inline Io::Type getType() const noexcept
        {
            return m_type;
        }
        
        //! Retrieve the polarity of the iolet.
        /** The functions retrieves the polarity of the iolet.
         @return The polarity of the iolet.
         */
        inline Io::Polarity getPolarity() const noexcept
        {
            return m_polarity;
        }
        
        //! Retrieve the description of the iolet.
        /** The functions retrieves the description of the iolet.
         @return The description of the iolet.
         */
        inline string getDescription() const noexcept
        {
            return m_description;
        }
        
        //! Retrieve the number of connections.
        /** The functions retrieves the number of connections of the iolet.
         @return The number of connections.
         */
        inline ulong getNumberOfConnection() const noexcept
        {
            lock_guard<mutex> guard(m_mutex);
            return (ulong)m_connections.size();
        }
        
        //! Retrieve a connection.
        /** The functions retrieves a connection.
         @param index The index of the connection.
         @return The connection.
         */
        const Connection getConnection(const ulong index) const noexcept;
        
        //! Retrieve a connection.
        /** The functions retrieves a connection.
         @param index The index of the connection.
         @return The connection.
         */
        Connection getConnection(const ulong index) noexcept;
        
        //! Retrieve the object of a connection.
        /** The functions retrieves the object of a connection.
         @param index The index of the connection.
         @return The object of a connection.
         */
        scObject getObject(const ulong index) const noexcept;
        
        //! Retrieve the object of a connection.
        /** The functions retrieves the object of a connection.
         @param index The index of the connection.
         @return The object of a connection.
         */
        sObject getObject(const ulong index) noexcept;
        
        //! Retrieve the iolet's index of a connection.
        /** The functions retrieves the iolet's index of a connection.
         @param index The index of the connection.
         @return The iolet's index of a connection.
         */
        ulong getIndex(const ulong index) const noexcept;
    };
    
    // ================================================================================ //
    //                                      INLET                                       //
    // ================================================================================ //
    
    //! The inlet owns a set of links.
    /**
     The inlet owns a set of links that are used to manage links in a object. It also have a type and a description.
     */
    class Object::Inlet : public Iolet
    {
    public:
        
        //! Constructor.
        /** You should never call this method except if you really know what you're doing.
         */
        Inlet(Io::Type type, Io::Polarity polarity, string const& description) noexcept;
        
        //! Destructor.
        /** You should never call this method except if you really know what you're doing.
         */
        ~Inlet();
    };
    
    // ================================================================================ //
    //                                      OUTLET                                      //
    // ================================================================================ //
    
    //! The outlet owns a set of links.
    /**
     The outlet owns a set of links that are used to manage links in a object. It also have a type and a description.
     */
    class Object::Outlet : public Iolet
    {
    public:
        //! Constructor.
        /** You should never call this method except if you really know what you're doing.
         @param type        The type of the outlet.
         @param description The description of the outlet.
         */
        Outlet(Io::Type type, string const& description) noexcept;
        
        //! Destructor.
        /** You should never call this method except if you really know what you're doing.
         */
        ~Outlet();
        
        //! Send a vector of atoms to the connected inlets.
        /** The function sends of atoms to the connected inlets.
         @param atoms The vector of atoms.
         */
        void send(Vector const& atoms) const noexcept;
    };
}


#endif


