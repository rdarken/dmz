#include <dmzObjectAttributeMasks.h>
#include <dmzObjectModule.h>
#include "dmzObjectPluginFlagGroup.h"
#include <dmzRuntimeConfigToTypesBase.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>

/*!

\class dmz::ObjectPluginFlagGroup
\brief Insures that only a predefined number of object have a give flag enabled.
\ingroup Object
\details This Plugin ensures that only a predefined number of object have a give flag
enabled. When an object has the flag enabled and that pushes the group size over the
define limit, the first object with its flag enabled in the group has the flag
disabled. The default group size is one.
\code
<local>
   <group size="Number of Objects permitted to have the flag enabled"/>
   <flag attribute="Attribute Name of the flag"/>
</local>
\endcode
*/


namespace {

static dmz::Int32 LocalData = 0;

};

//! \cond
dmz::ObjectPluginFlagGroup::ObjectPluginFlagGroup (
      const PluginInfo &Info,
      Config &local) :
      Plugin (Info),
      ObjectObserverUtil (Info, local),
      _log (Info),
      _undo (Info),
      _maxGroupSize (1),
      _flagAttrHandle (0) {

   _init (local);
}


dmz::ObjectPluginFlagGroup::~ObjectPluginFlagGroup () {

   _flagGroup.clear ();
}


// Object Observer Interface
void
dmz::ObjectPluginFlagGroup::destroy_object (
      const UUID &Identity,
      const Handle ObjectHandle) {

   _flagGroup.remove (ObjectHandle);
}


void
dmz::ObjectPluginFlagGroup::remove_object_attribute (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Mask &AttrMask) {

   if (AttrMask.contains (ObjectFlagMask)) {

      _flagGroup.remove (ObjectHandle);
   }
}


void
dmz::ObjectPluginFlagGroup::update_object_flag (
      const UUID &Identity,
      const Handle ObjectHandle,
      const Handle AttributeHandle,
      const Boolean Value,
      const Boolean *PreviousValue) {

   if (Value) {

      _flagGroup.store (ObjectHandle, &LocalData);

      if (!_undo.is_in_undo ()) {

         if (_flagGroup.get_count () > _maxGroupSize) {

            HashTableHandleIterator it;

            if (_flagGroup.get_first (it)) {

               ObjectModule *objMod (get_object_module ());

               if (objMod) {

                  objMod->store_flag (it.get_hash_key (), _flagAttrHandle, False);
               }
            }
         }
      }
   }
   else { _flagGroup.remove (ObjectHandle); }
}


void
dmz::ObjectPluginFlagGroup::_init (Config &local) {

   _maxGroupSize = config_to_int32 ("group.size", local, _maxGroupSize);

   activate_default_object_attribute (ObjectDestroyMask);

   _flagAttrHandle = activate_object_attribute (
      config_to_string ("flag.attribute", local),
      ObjectRemoveAttributeMask | ObjectFlagMask);
}
//! \endcond


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzObjectPluginFlagGroup (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::ObjectPluginFlagGroup (Info, local);
}

};
